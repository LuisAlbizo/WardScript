#include "dict.h"
#include <stdio.h>
#include <stdlib.h>

struct data {
	int data;
};

void d_update(dict *, char *, int);
int d_search(dict *, char *);

int main() {
	dict *mydict = newdict();

	d_update(mydict, "cinco", 5);
	d_update(mydict, "cinco", -5);
	d_update(mydict, "seis", 6);
	d_update(mydict, "cincuenta", 50);
	d_update(mydict, "uno", 1);
	d_update(mydict, "menos uno", -1);

	printf("search: menos uno = %d\n", d_search(mydict, "menos uno"));
	printf("search: uno = %d\n", d_search(mydict, "uno"));
	printf("search: cinco = %d\n", d_search(mydict, "cinco"));
	printf("search: dos = %d\n", d_search(mydict, "dos"));

	return 0;
}

void d_update(dict *d, char *key, int v) {
	struct data *data = malloc(sizeof(struct data));
	data->data = v;
	dict_update(d, key, (dict_Data *) data);
}

int d_search(dict *d, char *key) {
	dict_node *m = dict_search(d, key);
	if (!m) {
		printf("error, %s not found\n", key);
		return -1;
	}
	return ((struct data *) m->data)->data;
}

