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

void Scope_Set(Scope *s, char *key, Scope_Object *o) {
	dict_update(s->vars, key, (dict_Data *) o);
}

Scope_Object *Scope_Get(Scope *s, char *key) {
	printf("scope get\n");
	if (!s)
		return NULL;
	printf("scope is not null\n");
	dict_node *match = dict_search(s->vars, key);
	printf("scope match\n");
	if (!match)
		return Scope_Get(s->upscope, key);
	return (Scope_Object *) match->data;
}

