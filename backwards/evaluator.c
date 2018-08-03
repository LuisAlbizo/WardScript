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
#include <stdio.h>

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
	printf("eva NAME: %s\n", n->name);
	Scope_Object *o = Scope_Get(S, n->name);
	if (!o) {
		char e[200];
		strcat(e, "use of name '");
		strcat(e, n->name);
		strcat(e, "' before assignment");
		raiseError(UNDECLARED_ERROR, e);
	}
	return (st_st *) new_object((B_Object *) o);
}

/* BOP */

st_st *eva_bop(st_bop *bop, Scope *S) {
	printf("eva BOP: %c\n", bop->op);
	st_st *result;
	bop->left = eva_(bop->left, S);
	bop->right = eva_(bop->right, S);
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
		raiseError(OPERATOR_ERROR, "invalid operation for no-number types");
	return result;
}

/* UOP */

st_st *eva_uop(st_uop *uop, Scope *S) {
	printf("eva UOP: %c\n", uop->op);
	unsigned int rnumber;
	uop->right = eva_(uop->right, S);
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
			raiseError(OPERATOR_ERROR, "invalid unary operator for node type");
			break;
	}
	return new_object(new_bbyte((char) rnumber));
}

/* ASSIGNMENT */

st_st *eva_assignment(st_assignment *assi, Scope *S) {
	printf("eva ASSIGNMENT\n");
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
	printf("eva IF\n");
	B_Object *cond = to_bool(((st_object *) eva_(ifst->condition, S))->obj);
	st_block *blck = (((B_Byte *)cond)->byte ? ifst->block_if : ifst->block_else);
	free(cond);
	if (!blck)
		return NULL;
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
	printf("eva FOREVER\n");
	if (!rvr->block)
		raiseError(LOOP_ERROR, "infinite loop detected");
	stack_node *stat = rvr->block->block->top;
	while (1) {
		if (!stat) stat = rvr->block->block->top;
		if (eva_((st_st *) (stat->data), S)->type == AST_EXIT) break;
		else stat = stat->next;
	}
	return NULL;
}

/* CALL */

st_st *eva_call(st_call *call, Scope *S) {
	printf("eva CALL\n");
	B_Function *f = (B_Function *) ((st_object *) eva_(call->callable, S))->obj;
	//B_Function *f = (B_Function *) call->callable;
	if (f->type != B_FUNCTION)
		raiseError(UNCALLABLE_ERROR, "not a function expression");
	stack_node *stat;
	B_Object *return_obj;
	Scope *FS = newScope(S);
	stack_node *argval;
	switch (f->ftype) {
		case B_FUNCTYPE:
			printf("eva B_Function\n");
			// Setting the arguments on the FScope
			printf("argnames: %d, args: %d\n", f->argnames->count, call->args->count);
			if (f->argnames->count != call->args->count)
				raiseError(ARGCOUNT_ERROR, "");
			argval = call->args->top;
			stack_node *argname = f->argnames->top;
			while (argval && argname) {
				Scope_Set(FS, ((st_name *) argname->data)->name,
					(Scope_Object *) ((st_object *) eva_((st_st *) argval->data, S))->obj);
				argval = argval->next;
				argname = argname->next;
			}
			// Evaluating the code of the function
			if (f->code_block) {
				stat = f->code_block->top;
				printf("evaluating...\n");
				while (stat) {
					if (eva_((st_st *) stat->data, S)->type == AST_EXIT) break;
					else stat = stat->next;
				}
			}
			return_obj = (B_Object *) Scope_Get(FS, f->return_name);
			if (!return_obj)
				raiseError(UNDECLARED_ERROR, "return name not declared");
			break;
		case C_FUNCTYPE:;
			printf("eva C_Function\n");
			stack *evalargs = newstack();
			stack_Data *arg = stack_pop(call->args);
			while (arg) {
				stack_push(evalargs, arg);
				arg = stack_pop(call->args);
			}
			arg = stack_pop(evalargs);
			while (arg) {
				stack_push(call->args, 
						(stack_Data *) ((st_object *) eva_((st_st *) arg, S))->obj);
				arg = stack_pop(evalargs);
			}
			return_obj = f->cfunc(call->args, FS);
			break;
	}
	return new_object(return_obj);
}

/* METHODCALL */

st_st *eva_methodcall(st_methodcall *mcall, Scope *S) {
	printf("eva METHODCALL\n");
	st_call *call;
	B_Object *o = ((st_object *) eva_(mcall->object, S))->obj;
	if (o->type != B_NODE)
		raiseError(TYPE_ERROR, "required a Node Object to call a method");
	B_Object *f = (B_Object *) dict_search(((B_Node *) o)->members, mcall->method)->data;
	if (f->type != B_FUNCTION)
		raiseError(TYPE_ERROR, "method must be a Function");
	call = (st_call *) new_call(new_object(f), mcall->args);
	stack_push(call->args, (stack_Data *) new_object(o));
	return eva_call(call, S);
}

/* MEMBER */

st_st *eva_member(st_member *member, Scope *S) {
	printf("eva MEMBER\n");
	B_Object *o = ((st_object *) eva_(member->object, S))->obj;
	if (o->type != B_NODE)
		raiseError(TYPE_ERROR, "only Node Objects had members");
	dict_node *dd = dict_search(((B_Node *) o)->members, member->name);
	if (!dd)
		raiseError(UNDECLARED_ERROR, strcat("node has no member ", member->name));
	o = (B_Object *) dd->data;
	return new_object(o);
}

/* MEMBER ASSIGN */

st_st *eva_member_assign(st_member_assign *massign, Scope *S) {
	printf("eva MEMBER_ASSIGN\n");
	B_Object *o = ((st_object *) eva_(massign->member->object, S))->obj;
	if (o->type != B_NODE)
		raiseError(TYPE_ERROR, "only can assign a member of a Node Object");
	dict_node *dd = dict_search(((B_Node *) o)->members, massign->member->name);
	if (!dd)
		raiseError(UNDECLARED_ERROR, strcat("can't assign member not declared ", massign->member->name));
	o = ((st_object *) eva_(massign->object, S))->obj;
	dd->data = (dict_Data *) o;
	return NULL;
}

/* GENERAL EVALUATOR */

st_st *eva_(st_st *stat, Scope *S) {
	if (!stat)
		return NULL;
	printf("eva _ (%i)\n", stat->type);
	st_st *ret;
	switch (stat->type) {
		case AST_EXIT:
			ret = stat;
			break;
		case AST_OBJECT:
			ret = stat;
			break;
		case AST_BOP:
			ret = eva_bop((st_bop *) stat, S);
			break;
		case AST_UOP:
			ret = eva_uop((st_uop *) stat, S);
			break;
		case AST_NAME:
			ret = eva_name((st_name *) stat, S);
			break;
		case AST_MEMBER:
			ret = eva_member((st_member *) stat, S);
			break;
		case AST_MEMBER_A:
			ret = eva_member_assign((st_member_assign *) stat, S);
			break;
		case AST_IF:
			ret = eva_if((st_if *) stat, S);
			break;
		case AST_FOREVER:
			ret = eva_forever((st_forever *) stat, S);
			break;
		case AST_CALL:
			ret = eva_call((st_call *) stat, S);
			break;
		case AST_METHODCALL:
			ret = eva_methodcall((st_methodcall *) stat, S);
			break;
		case AST_ASSIGNMENT:
			ret = eva_assignment((st_assignment *) stat, S);
			break;
	}
	return ret;
}


