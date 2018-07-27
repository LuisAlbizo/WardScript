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
		raiseError(MEMORY_ERROR, "can't create new Object Node", NULL);
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
		raiseError(UNDECLARED_ERROR, errorm, NULL);
	}
	return dn->data;
}

void *Bnode_Set(B_Node *n, char *key, dict_Data *d) {
	dict_node *dn = dict_search(n->members, key);
	if (!dn) {
		char errorm[200];
		strcpy(errorm, "node has no member called: ");
		strcat(errorm, key);
		raiseError(UNDECLARED_ERROR, errorm, NULL);
	}
	dn->data = d;
}

/* Nil */

B_Object *new_bnil() {
	B_Nil *n = malloc(sizeof(B_Nil));
	if (!n)
		raiseError(MEMORY_ERROR, "can't create new Object Nil", NULL);
	n->type = B_NIL;
	return (B_Object *) n;
}

/* Number */

B_Object *new_bnumber(long n) {
	B_Number *u = malloc(sizeof(B_Number));
	if (!u)
		raiseError(MEMORY_ERROR, "can't create new Object Number", NULL);
	n->type = B_NUMBER;
	n->number = n;
	return (B_Object *) u;
}

/* Function */

B_Object *new_bfunction(char *return_name, stack *argnames, st_block *code) {
	B_Function *f = malloc(sizeof(B_Function));
	if (!f)
		raiseError(MEMORY_ERROR, "can't create new Object Function", NULL);
	f->type = B_FUNCTION;
	strncpy(f->return_name, return_name, MAX_DICT_KEY);
	f->argnames = argnames;
	f->code_block = code;
	return (B_Object *) f;
}

void free_obj(B_Object *o) {
	free(o);
}

#endif
