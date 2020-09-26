/* Objects (Data Types) implementation file: object.c
 * Luis Albizo "07-27-18"
 */

#include "object.h"
#include "dict.h"
#include "error.h"
#include "stack.h"
#include <stdlib.h>
#include <string.h>

unsigned int _pow(unsigned int x, unsigned int y) {
	unsigned int r = 1;
	for (int i = 0; i < y; i++)
		r = r * x;
	return r;
}

/* Node Functions */

W_Object *new_wnode(dict *d) {
	W_Node *n = malloc(sizeof(W_Node));
	if (!n)
		raiseError(MEMORY_ERROR, "can't create new Object Node");
	n->type = W_NODE;
	n->members = d;
	return (W_Object *) n;
}

dict_Data *Wnode_Get(W_Node *n, char *key) {
	dict_node *dn = dict_search(n->members, key);
	if (!dn) {
		char errorm[300] = "";
		strncpy(errorm, "node has no member called: ", 100);
		strncat(errorm, key, 200);
		raiseError(UNDECLARED_ERROR, errorm);
	}
	return dn->data;
}

void Wnode_Set(W_Node *n, char *key, dict_Data *d) {
	dict_node *dn = dict_search(n->members, key);
	if (!dn) {
		char errorm[300] = "";
		strncpy(errorm, "node has no member called: ",100);
		strncat(errorm, key, 200);
		raiseError(UNDECLARED_ERROR, errorm);
	}
	dn->data = d;
}

/* Nil */

W_Object *new_wnil() {
	W_Nil *n = malloc(sizeof(W_Nil));
	if (!n)
		raiseError(MEMORY_ERROR, "can't create new Object Nil");
	n->type = W_NIL;
	return (W_Object *) n;
}

/* Byte */

W_Object *new_wbyte(byte_t n) {
	W_Byte *u = malloc(sizeof(W_Byte));
	if (!u)
		raiseError(MEMORY_ERROR, "can't create new Object Byte");
	u->type = W_BYTE;
	u->byte = n;
	return (W_Object *) u;
}

/* Function */

W_Object *new_wfunction(char *return_name, stack *argnames, stack *code, Scope *state) {
	W_Function *f = malloc(sizeof(W_Function));
	if (!f)
		raiseError(MEMORY_ERROR, "can't create new Object Function");
	f->type = W_FUNCTION;
	f->ftype = W_FUNCTYPE;
	strncpy(f->return_name, return_name, MAX_DICT_KEY);
	f->argnames = argnames;
	f->code_block = code;
	f->state = state;
	return (W_Object *) f;
}

W_Object *new_cfunction(W_Object* (*cfunc)(stack *, Scope *)) {
	W_Function *f = malloc(sizeof(W_Function));
	if (!f)
		raiseError(MEMORY_ERROR, "can't create new Object C Function");
	f->type = W_FUNCTION;
	f->ftype = C_FUNCTYPE;
	f->cfunc = cfunc;
	return (W_Object *) f;
}

/* Integers */

W_Object *new_wnumber(unsigned int n) {
	// Get the bytes
	char bytes[4];
	for (int i = 0; i < 4; i++) {
		bytes[i] = (char) ((n % _pow(256, i+1)) / _pow(256, i));
		n = n - (unsigned int) bytes[i];
	}
	// Create the nodes
	W_Node *root = (W_Node *) new_wnode(newdict());
	W_Node *cur = (W_Node *) root;
	dict_update(cur->members, "$byte", (dict_Data *) new_wbyte(bytes[3]));
	dict_update(cur->members, "$nextbyte", (dict_Data *) new_wnil());
	root = cur, cur = (W_Node *) new_wnode(newdict());
	dict_update(cur->members, "$byte", (dict_Data *) new_wbyte(bytes[2]));
	dict_update(cur->members, "$nextbyte", (dict_Data *) root);
	root = cur, cur = (W_Node *) new_wnode(newdict());
	dict_update(cur->members, "$byte", (dict_Data *) new_wbyte(bytes[1]));
	dict_update(cur->members, "$nextbyte", (dict_Data *) root);
	root = cur, cur = (W_Node *) new_wnode(newdict());
	dict_update(cur->members, "$byte", (dict_Data *) new_wbyte(bytes[0]));
	dict_update(cur->members, "$nextbyte", (dict_Data *) root);
	return (W_Object *) cur;
}

unsigned int wnumber_to_int(W_Node *root) {
	unsigned int r = 0;
	W_Node *cur = root;
	for (int i = 0; i < 4; i++) {
		r = r + ((unsigned int) ((W_Byte *)
			Wnode_Get(cur, "$byte"))->byte * _pow(256, i));
		cur = (W_Node *) Wnode_Get(cur, "$nextbyte");
	}
	return r;
}

/* Pseudo-String */

W_Object *new_char(char chr) {
	dict *d = newdict();
	dict_update(d, "$char", (dict_Data *) new_wbyte(chr));
	dict_update(d, "$next", (dict_Data *) new_wnil());
	return new_wnode(d);
}

W_Object *new_string(const char *str) {
	W_Node *stroot = (W_Node *) new_char(str[0]);
	W_Node *tail = stroot;
	for (int i = 1; str[i] != '\0'; i++) {
		Wnode_Set(tail, "$next", (dict_Data *) new_char(str[i]));
		tail = (W_Node *) Wnode_Get(tail, "$next");
	}
	return (W_Object *) stroot;
}

/* Pseudo-List */

W_Object *new_ListItem(W_Object *data) {
	dict *m = newdict();
	W_Object *n = new_wnil();
	dict_update(m, "$data", (dict_Data *) data);
	dict_update(m, "$next", (dict_Data *) n);
	return new_wnode(m);
}

W_Object *new_List(stack *objects) {
	W_Object *item = (W_Object *) stack_pop(objects);
	dict *m = newdict();
	W_Node *l = (W_Node *) new_wnode(m);
	if (!item) {
		dict_update(m, "$root", (dict_Data *) new_wnil());
		return (W_Object *) l;
	} else
		dict_update(m, "$root", (dict_Data *) new_ListItem(item));
	W_Node *tail = (W_Node *) Wnode_Get(l, "$root");
	item = (W_Object *) stack_pop(objects);
	while (item) {
		Wnode_Set(tail, "$next", (dict_Data *) new_ListItem(item));
		tail = (W_Node *) Wnode_Get(tail, "$next");
		item = (W_Object *) stack_pop(objects);
	}
	return (W_Object *) l;
}

void free_obj(W_Object *o) {
	// TODO
	free(o);
}


