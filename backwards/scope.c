/* Scope implementation file: scope.c
 * Luis Albizo "07-25-18"
 */

#include "scope.h"
#include <stdio.h>
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

void Scope_Set(Scope *s, char *key, object *o) {
	dict_update(s->vars, key, (dict_Data *) o);
}

object *Scope_Get(Scope *s, char *key) {
	if (!s)
		return NULL;
	object *match = (object *) dict_search(s->vars, key)->data;
	if (!match)
		return Scope_Get(s->upscope, key);
	else
		return match;
}

