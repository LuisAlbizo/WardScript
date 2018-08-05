#include "dict.h"
#include "scope.h"
#include <stdio.h>
#include <stdlib.h>

struct data {
	int data;
};

void s_set(Scope *, char *, int);
int s_get(Scope *, char *);

int main() {
	Scope *mys = newScope(NULL);

	s_set(mys, "uno", 1);
	s_set(mys, "dos", 2);
	s_set(mys, "tres", 3);
	s_set(mys, "cuatro", 4);
	s_set(mys, "cinco", 5);

	printf("get uno : %d\n", s_get(mys,"uno"));
	printf("get dos : %d\n", s_get(mys,"dos"));
	printf("get tres : %d\n", s_get(mys,"tres"));
	printf("get four : %d\n", s_get(mys,"four"));
	printf("get uno : %d\n", s_get(mys,"uno"));

	return 0;
}

void s_set(Scope *S, char *key, int v) {
	struct data *data = malloc(sizeof(struct data));
	data->data = v;
	Scope_Set(S, key, (Scope_Object *) data);
}

int s_get(Scope *S, char *key) {
	Scope_Object *m = Scope_Get(S, key);
	if (!m) {
		printf("error, %s not found\n", key);
		return -1;
	}
	return ((struct data *) m)->data;
}

