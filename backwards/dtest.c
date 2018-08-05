#include "dict.h"
#include <stdio.h>
#include <stdlib.h>

struct data {
	int data;
};

void d_set(dict *, char *, int);
int d_get(dict *, char *);

int main() {
	dict *mys = newdict();

	d_set(mys, "uno", 1);
	d_set(mys, "dos", 2);
	d_set(mys, "tres", 3);
	d_set(mys, "cuatro", 4);
	d_set(mys, "cinco", 5);

	printf("get uno : %d\n", d_get(mys,"uno"));
	printf("get dos : %d\n", d_get(mys,"dos"));
	printf("get tres : %d\n", d_get(mys,"tres"));
	printf("get four : %d\n", d_get(mys,"four"));
	printf("get uno : %d\n", d_get(mys,"uno"));

	return 0;
}

void d_set(dict *d, char *key, int v) {
	struct data *data = malloc(sizeof(struct data));
	data->data = v;
	dict_update(d, key, (dict_Data *) data);
}

int d_get(dict *d, char *key) {
	dict_node *m = dict_search(d, key);
	if (!m) {
		printf("error, %s not found\n", key);
		return -1;
	}
	return ((struct data *) m->data)->data;
}

