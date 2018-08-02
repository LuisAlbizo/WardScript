/* Objects (Data Types) implementation file: object.c
 * Luis Albizo "07-27-18"
 */

#include "object.h"
#include "dict.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>

/* Node Functions */

B_Object *new_bnode(dict *d) {
	B_Node *n = malloc(sizeof(B_Node));
	if (!n)
		raiseError(MEMORY_ERROR, "can't create new Object Node");
	n->type = B_NODE;
	n->members = d;
	return (B_Object *) n;
}

dict_Data *Bnode_Get(B_Node *n, char *key) {
	dict_node *dn = dict_search(n->members, key);
	if (!dn) {
		char errorm[200];
		strcpy(errorm, "node has no member called: ");
		strcat(errorm, key);
		raiseError(UNDECLARED_ERROR, errorm);
	}
	return dn->data;
}

void Bnode_Set(B_Node *n, char *key, dict_Data *d) {
	dict_node *dn = dict_search(n->members, key);
	if (!dn) {
		char errorm[200];
		strcpy(errorm, "node has no member called: ");
		strcat(errorm, key);
		raiseError(UNDECLARED_ERROR, errorm);
	}
	dn->data = d;
}

/* Nil */

B_Object *new_bnil() {
	B_Nil *n = malloc(sizeof(B_Nil));
	if (!n)
		raiseError(MEMORY_ERROR, "can't create new Object Nil");
	n->type = B_NIL;
	return (B_Object *) n;
}

/* Byte */

B_Object *new_bbyte(char n) {
	B_Byte *u = malloc(sizeof(B_Byte));
	if (!u)
		raiseError(MEMORY_ERROR, "can't create new Object Byte");
	u->type = B_BYTE;
	u->byte = n;
	return (B_Object *) u;
}

/* Function */

B_Object *new_bfunction(char *return_name, stack *argnames, stack *code) {
	B_Function *f = malloc(sizeof(B_Function));
	if (!f)
		raiseError(MEMORY_ERROR, "can't create new Object Function");
	f->type = B_FUNCTION;
	f->ftype = B_FUNCTYPE;
	strncpy(f->return_name, return_name, MAX_DICT_KEY);
	f->argnames = argnames;
	f->code_block = code;
	return (B_Object *) f;
}

B_Object *new_cfunction(B_Object* (*cfunc)(stack *, Scope *)) {
	B_Function *f = malloc(sizeof(B_Function));
	if (!f)
		raiseError(MEMORY_ERROR, "can't create new Object C Function");
	f->type = B_FUNCTION;
	f->ftype = C_FUNCTYPE;
	f->cfunc = cfunc;
	return (B_Object *) f;
}

void free_obj(B_Object *o) {
	free(o);
}


