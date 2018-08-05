/* Syntax Tree Data Structure implementation file: ast.c
 * Luis Albizo "07-25-18"
 */
#include "dict.h"
#include "scope.h"
#include "stack.h"
#include "ast.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>

/* AST Creation */

void *astalloc(size_t size) {
	void *mem = malloc(size);
	if (!mem)
		raiseError(MEMORY_ERROR, "can't create new AST");
	return mem;
}

st_st *new_st() {
	printf("new AST_ST\n");
	return (st_st *) astalloc(sizeof(st_st));
}

st_st *new_block(stack *s) {
	printf("new AST_BLOCK\n");
	st_block *nb = astalloc(sizeof(st_block));
	nb->type = AST_BLOCK;
	nb->block = s;
	return (st_st *) nb;
}

assign *new_assign(char *name, st_st *value) {
	printf("new ASSIGN\n");
	assign *a = astalloc(sizeof(assign));
	strncpy(a->name, name, MAX_DICT_KEY);
	a->value = value;
	return a;
}

st_st *new_assignment(stack *s) {
	printf("new AST_ASSIGNMENT\n");
	st_assignment *na = astalloc(sizeof(st_assignment));
	na->type = AST_ASSIGNMENT;
	na->assigns = s;
	return (st_st *) na;
}

st_st *new_bop(char op, st_st *l, st_st *r) {
	printf("new AST_BOP\n");
	st_bop *bop = astalloc(sizeof(st_bop));
	bop->type = AST_BOP;
	bop->op = op;
	bop->left = l;
	bop->right = r;
	return (st_st *) bop;
}

st_st *new_uop(char op, st_st *r) {
	printf("new AST_UOP\n");
	st_uop *uop = astalloc(sizeof(st_uop));
	uop->type = AST_UOP;
	uop->op = op;
	uop->right = r;
	return (st_st *) uop;
}

st_st *new_name(char *name) {
	printf("new AST_NAME\n");
	st_name *n = astalloc(sizeof(st_name));
	n->type = AST_NAME;
	strncpy(n->name, name, MAX_DICT_KEY);
	return (st_st *) n;
}

st_st *new_call(st_st *callable, stack *args) {
	printf("new AST_CALL\n");
	st_call *c = astalloc(sizeof(st_call));
	c->type = AST_CALL;
	c->callable = callable;
	c->args = args;
	return (st_st *) c;
}

st_st *new_node_construct(st_assignment *mems) {
	printf("new NODE CONSTRUCT\n");
	st_node_construct *nc = astalloc(sizeof(st_node_construct));
	nc->type = AST_NODE_C;
	nc->members = mems;
	return (st_st *) nc;
}

st_st *new_member(st_st *obj, char *mname) {
	printf("new AST_MEMBER\n");
	st_member *m = astalloc(sizeof(st_member));
	m->type = AST_MEMBER;
	m->object = obj;
	strncpy(m->name, mname, MAX_DICT_KEY);
	return (st_st *) m;
}

st_st *new_member_assign(st_member *member, st_st *obj) {
	printf("new AST_MEMBER_A\n");
	st_member_assign *m = astalloc(sizeof(st_member_assign));
	m->type = AST_MEMBER_A;
	m->member = member;
	m->object = obj;
	return (st_st *) m;
}

st_st *new_methodcall(st_st *obj, char *mname, stack *args) {
	printf("new AST_METHODCALL\n");
	st_methodcall *mc = astalloc(sizeof(st_methodcall));
	mc->type = AST_METHODCALL;
	mc->object = obj;
	strncpy(mc->method, mname, MAX_DICT_KEY);
	mc->args = args;
	return (st_st *) mc;
}

st_st *new_argnames(stack *names) {
	printf("new AST_NAMES\n");
	st_argnames *argns = astalloc(sizeof(st_argnames));
	argns->type = AST_NAMES;
	argns->names = names;
	return (st_st *) argns;
}

st_st *new_if(st_st *condition, st_block *block_if, st_block *block_else) {
	printf("new AST_IF\n");
	st_if *if_ = astalloc(sizeof(st_if));
	if_->type = AST_IF;
	if_->condition = condition;
	if_->block_if = block_if;
	if_->block_else = block_else;
	return (st_st *) if_;
}

st_st *new_forever(st_block *block) {
	printf("new AST_FOREVER\n");
	st_forever *forever = astalloc(sizeof(st_forever));
	forever->type = AST_FOREVER;
	forever->block = block;
	return (st_st *) forever;
}

st_st *new_object(B_Object *obj) {
	printf("new AST_OBJECT\n");
	st_object *o = astalloc(sizeof(st_object));
	o->type = AST_OBJECT;
	o->obj = obj;
	return (st_st *) o;
}

/* AST Deletion */

void free_assign(assign *a) {
	// XXX TODO
	free(a);
}

void free_st(st_st *st) {
	// XXX TODO
	free(st);
}

