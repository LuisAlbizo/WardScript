/* Statement Evaluator implementation file: evaluator.c
 * Luis Albizo "07-29-18"
 */

#include "evaluator.h"
#include "ast.h"
#include "stack.h"
#include "scope.h"
#include "error.h"
#include "object.h"
#include "dict.h"
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
	st_st *result = NULL;
	B_Object *left = ((st_object *) eva_(bop->left, S))->obj;
	B_Object *right = ((st_object *) eva_(bop->right, S))->obj;
	if (left->type == B_BYTE && right->type == B_BYTE) {
		unsigned int lnumber = (unsigned int) ((B_Byte *) left)->byte;
		unsigned int rnumber = (unsigned int) ((B_Byte *) right)->byte;
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
	} else {
		raiseError(OPERATOR_ERROR, "invalid operation for no-number types");
	}
	return result;
}

/* UOP */

st_st *eva_uop(st_uop *uop, Scope *S) {
	unsigned int rnumber;
	st_st *right = eva_(uop->right, S);
	switch (((st_object *) right)->obj->type) {
		case B_BYTE:
			rnumber = (unsigned int) ((B_Byte *) ((st_object *) right)->obj)->byte;
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
		case B_NODE:
			switch (uop->op) {
				case '!':
					rnumber = 0;
					break;
				default:
					break;
			}
			break;
	}
	return new_object(new_bbyte((char) rnumber));
}

/* ASSIGNMENT */

st_st *eva_assignment(st_assignment *assi, Scope *S) {
	stack_node *nod = assi->assigns->top;
	B_Object *o;
	while (nod) {
		o = ((st_object *) eva_(((assign *) nod->data)->value, S))->obj;
		Scope_Set(S, ((assign *) nod->data)->name, (Scope_Object *) o);

		nod = nod->next;
	}
	return new_st();
}

/* IF */

st_st *eva_if(st_if *ifst, Scope *S) {
	B_Object *cond = to_bool(((st_object *) eva_(ifst->condition, S))->obj);
	st_block *blck = (((B_Byte *) cond)->byte ? ifst->block_if : ifst->block_else);
	//free(cond);
	st_st *ex = new_st();
	if (!blck) {
		return ex;
	}
	stack_node *stat = blck->block->top;
	while (stat) {
		if (eva_((st_st *) stat->data, S)->type == AST_EXIT) {
			ex->type = AST_EXIT;
			break;
		} else
			stat = stat->next;
	}
	return ex;
}

/* FOREVER */

st_st *eva_forever(st_forever *rvr, Scope *S) {
	if (!rvr->block)
		raiseError(LOOP_ERROR, "infinite loop detected");
	stack_node *stat = rvr->block->block->top;
	st_st *s = NULL;
	while (1) {
		if (!stat)
			stat = rvr->block->block->top;
		s = eva_((st_st *) (stat->data), S);
		if (s->type == AST_EXIT)
			break;
		else
			stat = stat->next;
	}
	return new_st();
}

/* CALL */

st_st *eva_call(st_call *call, Scope *S) {
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
			// Setting the arguments on the FScope
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
				while (stat) {
					if (eva_((st_st *) stat->data, FS)->type == AST_EXIT) break;
					else stat = stat->next;
				}
			}
			return_obj = (B_Object *) Scope_Get(FS, f->return_name);
			if (!return_obj)
				raiseError(UNDECLARED_ERROR, "return name not declared");
			break;
		case C_FUNCTYPE:;
			stack *evalargs = newstack(), *passargs = newstack();
			stack_Data *arg = stack_pop(call->args);
			int i = 0;
			while (arg) {
				stack_push(evalargs, arg);
				arg = stack_pop(call->args);
			}
			arg = stack_pop(evalargs);
			while (arg) {
				stack_push(call->args, arg);
				stack_push(passargs, 
						(stack_Data *) ((st_object *) eva_((st_st *) arg, S))->obj);
				arg = stack_pop(evalargs);
			}
			return_obj = f->cfunc(passargs, FS);
			break;
	}
	return new_object(return_obj);
}

/* METHODCALL */

st_st *eva_methodcall(st_methodcall *mcall, Scope *S) {
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

/* NODE CONSTRUCT */

st_st *eva_node_construct(st_node_construct *nodecons, Scope *S) {
	// create the member's dict for the node
	dict *members = newdict();
	// get the first assign
	stack_node *member = ((st_assignment *) nodecons->members)->assigns->top;
	assign *assi;
	while (member) { // while the stack still have assigns left
		assi = (assign *) member->data;
		// set the name and evaluate the value
		dict_update(members, assi->name, (dict_Data *) ((st_object *) eva_(assi->value, S))->obj);
		// get the next assign
		member = member->next;
	}
	return new_object(new_bnode(members));
}

/* LIST CONSTRUCT */

st_st *eva_list_construct(st_list_construct *lc, Scope *S) {
	stack *evalitems = newstack(), *passitems = newstack();
	stack_Data *item = stack_pop(lc->items);
	while (item) {
		stack_push(evalitems, item);
		item = stack_pop(lc->items);
	}
	item = stack_pop(evalitems);
	while (item) {
		stack_push(lc->items, item);
		stack_push(passitems, (stack_Data *) ((st_object *) eva_((st_st *) item, S))->obj);
		item = stack_pop(evalitems);
	}
	return new_object(new_List(passitems));
}

/* MEMBER */

st_st *eva_member(st_member *member, Scope *S) {
	B_Object *o = ((st_object *) eva_(member->object, S))->obj;
	if (o->type != B_NODE)
		raiseError(TYPE_ERROR, "only Node Objects had members");
	dict_Data *dd = Bnode_Get((B_Node *) o, member->name);
	o = (B_Object *) dd;
	return new_object(o);
}

/* MEMBER ASSIGN */

st_st *eva_member_assign(st_member_assign *massign, Scope *S) {
	B_Object *o = ((st_object *) eva_(massign->member->object, S))->obj;
	if (o->type != B_NODE)
		raiseError(TYPE_ERROR, "only can assign a member of a Node Object");
	Bnode_Set((B_Node *) o, massign->member->name, (dict_Data *) ((st_object *) eva_(massign->object, S))->obj);
	return new_st();
}

/* GENERAL EVALUATOR */

st_st *eva_(st_st *stat, Scope *S) {
	if (!stat)
		return NULL;
	st_st *ret = NULL;
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
		case AST_NODE_C:
			ret = eva_node_construct((st_node_construct *) stat, S);
			break;
		case AST_LIST_C:
			ret = eva_list_construct((st_list_construct *) stat, S);
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
		default:
			ret = new_st();
			break;
	}
	return ret;
}


