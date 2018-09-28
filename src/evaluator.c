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
		char e[200] = "use of name '";
		strcat(e, n->name);
		strcat(e, "' before assignment");
		raiseError(UNDECLARED_ERROR, e);
	}
	return (st_st *) new_object((B_Object *) o);
}

/* BOP */
B_Object *bop_BYTE(byte_t op, B_Byte *self, B_Object *o) {
	byte_t r = self->byte;
	switch (op) {
		case '+':
			switch (o->type) {
				case B_BYTE:
					r = r + ((B_Byte *) o)->byte;
					break;
				default:
					raiseError(OPERATOR_ERROR, "Operator + only valid on bytes");
					break;
			}
			break;
		case '-':
			switch (o->type) {
				case B_BYTE:
					r = r - ((B_Byte *) o)->byte;
					break;
				default:
					raiseError(OPERATOR_ERROR, "Operator - only valid on bytes");
					break;
			}
			break;
		case '<':
			switch (o->type) {
				case B_BYTE:
					r = (byte_t) r << ((B_Byte *) o)->byte;
					break;
				default:
					raiseError(OPERATOR_ERROR, "Operator << only valid on bytes");
					break;
			}
			break;
		case '>':
			switch (o->type) {
				case B_BYTE:
					r = (byte_t) r >> ((B_Byte *) o)->byte;
					break;
				default:
					raiseError(OPERATOR_ERROR, "Operator >> only valid on bytes");
					break;
			}
			break;
		case 'l':
			switch (o->type) {
				case B_BYTE:
					r = r < ((B_Byte *) o)->byte;
					break;
				default:
					raiseError(OPERATOR_ERROR, "Operator < only valid on bytes");
					break;
			}
			break;
		case 'L':
			switch (o->type) {
				case B_BYTE:
					r = r <= ((B_Byte *) o)->byte;
					break;
				default:
					raiseError(OPERATOR_ERROR, "Operator <= only valid on bytes");
					break;
			}
			break;
		case 'g':
			switch (o->type) {
				case B_BYTE:
					r = r > ((B_Byte *) o)->byte;
					break;
				default:
					raiseError(OPERATOR_ERROR, "Operator > only valid on bytes");
					break;
			}
			break;
		case 'G':
			switch (o->type) {
				case B_BYTE:
					r = r >= ((B_Byte *) o)->byte;
					break;
				default:
					raiseError(OPERATOR_ERROR, "Operator >= only valid on bytes");
					break;
			}
			break;
		case 'e':
			switch (o->type) {
				case B_BYTE:
					r = r == ((B_Byte *) o)->byte;
					break;
				default:
					r = 0;
					break;
			}
			break;
		case 'n':
			switch (o->type) {
				case B_BYTE:
					r = r != ((B_Byte *) o)->byte;
					break;
				default:
					r = 1;
					break;
			}
			break;
		case 'A':
			r = ((B_Byte *) to_bool(o))->byte && r;
			break;
		case 'O':
			r = ((B_Byte *) to_bool(o))->byte || r;
			break;
	}
	return new_bbyte(r);
}

B_Object *bop_NODE(byte_t op, B_Node *self, B_Object *o) {
	byte_t r = 1;
	switch (op) {
		case 'e':
			switch (o->type) {
				case B_NODE:
					r = self == (B_Node *) o;
					break;
				default:
					r = 0;
					break;
			}
			break;
		case 'n':
			switch (o->type) {
				case B_NODE:
					r = self != (B_Node *) o;
					break;
				default:
					r = 1;
					break;
			}
			break;
		case 'A':
			r = r && ((B_Byte *) to_bool(o))->byte;
			break;
		case 'O':
			r = r || ((B_Byte *) to_bool(o))->byte;
			break;
		default:
			raiseError(OPERATOR_ERROR, "Invalid Operator for Node");
			break;
	}
	return new_bbyte(r);
}

B_Object *bop_NIL(byte_t op, B_Nil *self, B_Object *o) {
	byte_t r = 0;
	switch (op) {
		case 'e':
			switch (o->type) {
				case B_NIL:
					r = 1;
					break;
				default:
					r = 0;
					break;
			}
			break;
		case 'n':
			switch (o->type) {
				case B_NIL:
					r = 0;
					break;
				default:
					r = 1;
					break;
			}
			break;
		case 'A':
			r = r && ((B_Byte *) to_bool(o))->byte;
			break;
		case 'O':
			r = r || ((B_Byte *) to_bool(o))->byte;
			break;
		default:
			raiseError(OPERATOR_ERROR, "Invalid Operator for Nil");
			break;
	}
	return new_bbyte(r);
}

B_Object *bop_FUNCTION(byte_t op, B_Function *self, B_Object *o) {
	byte_t r = 1;
	switch (op) {
		case 'A':
			r = r && ((B_Byte *) to_bool(o))->byte;
			break;
		case 'O':
			r = r || ((B_Byte *) to_bool(o))->byte;
			break;
		default:
			raiseError(OPERATOR_ERROR, "Invalid Operator for Function");
			break;
	}
	return new_bbyte(r);
}

st_st *eva_bop(st_bop *bop, Scope *S) {
	B_Object *left = ((st_object *) eva_(bop->left, S))->obj;
	B_Object *right = ((st_object *) eva_(bop->right, S))->obj;
	B_Object *result = NULL;
	switch (left->type) {
		case B_BYTE:
			result = bop_BYTE(bop->op, (B_Byte *) left, right);
			break;
		case B_NODE:
			result = bop_NODE(bop->op, (B_Node *) left, right);
			break;
		case B_NIL:
			result = bop_NIL(bop->op, (B_Nil *) left, right);
			break;
		case B_FUNCTION:
			result = bop_FUNCTION(bop->op, (B_Function *) left, right);
			break;
	}
	return new_object(result);
}

/* UOP */

st_st *eva_uop(st_uop *uop, Scope *S) {
	unsigned int rnumber;
	st_st *right = eva_(uop->right, S);
	switch (uop->op) {
		case '!':
			switch (((st_object *) right)->obj->type) {
				case B_BYTE:
					rnumber = (unsigned int) ((B_Byte *) ((st_object *) right)->obj)->byte;
					rnumber = (rnumber == 0) ? 1 : 0;
					break;
				case B_NIL:
					rnumber = 1;
					break;
				case B_FUNCTION:
					rnumber = 0;
					break;
				case B_NODE:
					rnumber = 0;
					break;
			}
			right = new_object(new_bbyte((char) rnumber));
			break;
		case '#':
			uop->right = right;
			break;
	}
	return right;
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

/* NONLOCAL ASSIGNMENT */

st_st *eva_nonlocal_assignment(st_assignment *assi, Scope *S) {
	stack_node *nod = assi->assigns->top;
	B_Object *o;
	while (nod) {
		o = ((st_object *) eva_(((assign *) nod->data)->value, S))->obj;
		Scope_NLSet(S, ((assign *) nod->data)->name, (Scope_Object *) o);
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
	Scope *FS;
	stack_node *argval;
	switch (f->ftype) {
		case B_FUNCTYPE:
			FS = newScope(f->state); // The first-level scope is the one that existed
						// in the context where function was created
			Scope_Concat(FS, S); // For closure functionality
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
			FS = newScope(S);
			stack *evalargs = newstack(), *passargs = newstack();
			stack_Data *arg = stack_pop(call->args);
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
	st_st *result = eva_call(call, S);
	stack_pop(call->args);
	return result;
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

/* FUNCTION CONSTRUCT */

st_st *eva_function_construct(st_function_construct *fc, Scope *S) {
	return new_object(new_bfunction(fc->return_name, fc->argnames, fc->code_block, S));
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
		case AST_NL_NAME:
			ret = eva_name((st_name *) stat, S->upscope);
			break;
		case AST_NODE_C:
			ret = eva_node_construct((st_node_construct *) stat, S);
			break;
		case AST_LIST_C:
			ret = eva_list_construct((st_list_construct *) stat, S);
			break;
		case AST_FUNC_C:
			ret = eva_function_construct((st_function_construct *) stat, S);
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
		case AST_NL_ASSIGN:
			ret = eva_nonlocal_assignment((st_assignment *) stat, S);
			break;
		default:
			ret = new_st();
			break;
	}
	return ret;
}


