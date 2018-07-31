/* Statement Evaluator implementation file: evaluator.c
 * Luis Albizo "07-29-18"
 */

#include "evaluator.h"
#include "ast.h"
#include "stack.h"
#include "scope.h"
#include "error.h"
#include "object.h"
#include <stdlib.h>
#include <string.h>

B_Object *to_bool(B_Object *o) {
	B_Object *l;
	switch (o->type) {
		case B_NODE:
		case B_FUNCTION:
			l = new_bbyte(1);
			break;
		case B_BYTE:
			l = new_bbyte(((((B_Byte *) o)->byte == 0) ? 0 : 1));
			break;
		case B_NIL:
			l = new_bbyte(0);
			break;
	}
	return l;
}

st_st *eva_name(st_name *n, Scope *S) {
	Scope_Object *o = Scope_Get(S, n->name);
	if (!o) {
		char e[200];
		strcat(e, "use of name '");
		strcat(e, n->name);
		strcat(e, "' before assignment");
		raiseError(UNDECLARED_ERROR, e, NULL);
	}
	return (st_st *) new_object((B_Object *) o);
}

/* BOP */

st_st *eva_bop(st_bop *bop) {
	st_st *result;
	if ( ((st_object *) bop->left)->obj->type == B_BYTE && ((st_object *) bop->right)->obj->type == B_BYTE ) {
		unsigned int lnumber = (unsigned int) ((B_Byte *) ((st_object *) bop->left)->obj)->byte;
		unsigned int rnumber = (unsigned int) ((B_Byte *) ((st_object *) bop->right)->obj)->byte;
		switch (bop->op) {
			case '+':
				lnumber = lnumber + rnumber;
				break;
			case '-':
				lnumber = lnumber - rnumber;
				break;
			case '*':
				lnumber = lnumber * rnumber;
				break;
			case '/':
				lnumber = lnumber / rnumber;
				break;
			case '%':
				lnumber = lnumber % rnumber;
				break;
			case '^':
				if (rnumber < 0) {
					for (int i = 0; i > rnumber; i--)
						lnumber = 1 / lnumber;
				} else if (rnumber > 0) {
					for (int i = 0; i < rnumber; i++)
						lnumber = lnumber * lnumber;
				} else
					lnumber = 1;
				break;
			case 'l': // <
				lnumber = lnumber < rnumber;
				break;
			case 'L': // <=
				lnumber = lnumber <= rnumber;
				break;
			case 'g': // >
				lnumber = lnumber > rnumber;
				break;
			case 'G': // >=
				lnumber = lnumber >= rnumber;
				break;
			case 'e': // ==
				lnumber = lnumber == rnumber;
				break;
			case 'n': // =/
				lnumber = lnumber != rnumber;
				break;
			case 'A': // &&
				lnumber = lnumber && rnumber;
				break;
			case 'O': // ||
				lnumber = lnumber || rnumber;
				break;
			default:
				break;
		}
		result = new_object(new_bbyte((char) lnumber));
	} else
		raiseError(OPERATOR_ERROR, "invalid operation for no-number types", NULL);
	return result;
}

/* UOP */

st_st *eva_uop(st_uop *uop) {
	unsigned int rnumber;
	switch (((st_object *) uop->right)->obj->type) {
		case B_BYTE:
			rnumber = (unsigned int) ((B_Byte *) ((st_object *) uop->right)->obj)->byte;
			switch (uop->op) {
				case '!':
					rnumber = (rnumber == 0) ? 1 : 0;
					break;
				default:
					break;
			}
			break;
		case B_NIL:
			switch (uop->op) {
				case '!':
					rnumber = 1;
					break;
				default:
					break;
			}
			break;
		case B_FUNCTION:
			switch (uop->op) {
				case '!':
					rnumber = 0;
					break;
				default:
					break;
			}
			break;
		default:
			raiseError(OPERATOR_ERROR, "invalid unary operator for node type", NULL);
			break;
	}
	return new_object(new_bbyte((char) rnumber));
}

/* ASSIGNMENT */

st_st *eva_assignment(st_assignment *assi, Scope *S) {
	assign *a = (assign *) stack_pop(assi->assigns);
	B_Object *o;
	while (a) {
		o = ((st_object *) eva_(a->value, S))->obj;
		Scope_Set(S, a->name, (Scope_Object *) o);
		a = (assign *) stack_pop(assi->assigns);
	}
	return NULL;
}

/* IF */

st_st *eva_if(st_if *ifst, Scope *S) {
	B_Object *cond = to_bool(((st_object *) eva_(ifst->condition, S))->obj);
	st_block *blck = (((B_Byte *)cond)->byte ? ifst->block_if : ifst->block_else);
	free(cond);
	stack_node *stat = blck->block->top;
	while (stat) {
		if (eva_((st_st *) (stat->data), S)->type == AST_EXIT)
			return eva_((st_st *) (stat->data), S);
		else
			stat = stat->next;
	}
	return NULL;
}

/* FOREVER */

st_st *eva_forever(st_forever *rvr, Scope *S) {
	stack_node *stat = rvr->block->block->top;
	while (1) {
		if (!stat)
			stat = rvr->block->block->top;
		if (eva_((st_st *) (stat->data), S)->type == AST_EXIT)
			break;
		else
			stat = stat->next;
	}
	return NULL;
}

/* CALL */

st_st *eva_call(st_call *call, Scope *S) {

}

/* METHODCALL */

st_st *eva_methodcall(st_methodcall *mcall, Scope *S) {

}

/* MEMBER */

st_st *eva_member(st_member *member, Scope *S) {

}

/* MEMBER ASSIGN */

st_st *eva_member_assign(st_member_assign *massign, Scope *S) {

}

/* GENERAL EVALUATOR */

st_st *eva_(st_st *stat, Scope *S) {

}





