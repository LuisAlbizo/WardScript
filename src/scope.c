/* Scope implementation file: scope.c
 * Luis Albizo "07-25-18"
 */

#include "scope.h"
#include "error.h"
#include <string.h>
#include <stdlib.h>

Scope *newScope(Scope *s) {
	Scope *ns = malloc(sizeof(Scope));
	ns->upscope = s;
	ns->vars = newdict();
	return ns;
}

void free_Scope(Scope *s) {
	free_dict(s->vars);
	free(s);
}

/* Scope Functions to get variables */

void Scope_Set(Scope *s, char *key, Scope_Object *o) {
	dict_update(s->vars, key, (dict_Data *) o);
}

void Scope_NLSet(Scope *s, char *key, Scope_Object *o) {
	if (!(s->upscope)) {
		char error_msg[200] = "Nonlocal variable: '";
		strcat(error_msg, key);
		strcat(error_msg, "' not found");
		raiseError(UNDECLARED_ERROR, error_msg);
	}
	dict_node *match = dict_search(s->upscope->vars, key);
	if (!match)
		Scope_NLSet(s->upscope, key, o);
	else
		match->data = o;
}

Scope_Object *Scope_Get(Scope *s, char *key) {
	if (!s)
		return NULL;
	dict_node *match = dict_search(s->vars, key);
	if (!match)
		return Scope_Get(s->upscope, key);
	return (Scope_Object *) match->data;
}

bool Scope_CheckIn(Scope *S, Scope *SS) {
	bool in = false;
	Scope *last = SS;
	while (last) {
		if (S == last) {
			in = true;
			break;
		} else last = last->upscope;
	}
	return in;
}

void Scope_Concat(Scope *S, Scope *US) {
	Scope *last = S;
	while (last->upscope) {
		if (Scope_CheckIn(last, US)) return;
		last = last->upscope;
	}
	last->upscope = US;
}

