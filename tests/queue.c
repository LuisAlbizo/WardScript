#include <stdlib.h>
#include <stdio.h>

struct node {
	int data;
	//char *data;
	struct node *next;
};

struct list {
	struct node *root;
	struct node *tail;
};

struct list *newlist();
struct node *newnode(int);
void push(struct list *, int);
int pop(struct list *);
void free_node(struct node *);
void free_list(struct list *);
void impr(struct list *);

int main () {
	struct list *l = newlist();

	push(l, 2);
	impr(l);
	push(l, 4);
	impr(l);
	push(l, 8);
	impr(l);
	push(l, 16);
	impr(l);
	printf("pop: %d, ", pop(l));
	impr(l);
	printf("pop: %d, ", pop(l));
	impr(l);
	printf("pop: %d, ", pop(l));
	impr(l);
	printf("pop: %d, ", pop(l));
	impr(l);
	free(l);
}
/*
int main3() {
	char v[1024];
	struct list *l = newlist();
	printf("v: %p, list: %p\n", v, l);

	do {
		printf("cadena: ");
		scanf("%s", v);
		push(l, v);
		impr(l);
	} while (strcmp(v, "end") != 0);

	free(l);
	return 0;
}

int main2() {
	char v[100];
	struct list *l = newlist();

	int opc;
	do {
		printf(
		"Opciones:\n"
		"  1: insertar\n"
		"  2: remover\n"
		"  3: recorrer\n"
		"  4: salir\n"
		"\nopcion> "
		);

		scanf("%d", &opc);
		switch(opc){
			case 4:
				break;
			case 1:
				printf("valor> ");
				scanf("%s", v);
				//printf("Valor: %d, %s\n", opc, v);
				push(l, v);
				break;
			case 2:
				printf("Valor eliminado: %s", pop(l));
				break;
			case 3:
				impr(l);
				break;
			default:
				printf("Error: opcion invalida\n");
				break;
		}
	} while (opc != 4);

	free_list(l);
	return 0;
}*/

struct list *newlist() {
	struct list *nl = malloc(sizeof(struct list));
	if (!nl) {
		printf("Falta de memoria\n");
		exit(1);
	}
	nl->root = NULL;
	nl->tail = NULL;
	return nl;
}

struct node *newnode(int v) {
	printf("newnode...");
	struct node *nn = malloc(sizeof(struct node));
	if (!nn) {
		printf("Falta de memoria\n");
		exit(1);
	}
	nn->data = v;
	nn->next = NULL;
	printf("success\n");
	return nn;
}

void push(struct list *l, int v) {
	printf("push...");
	if (!l->root) {
		l->root = newnode(v);
		l->tail = l->root;
	} else {
		l->tail->next = newnode(v);
		l->tail = l->tail->next;
	}
	printf("success\n");
}

int pop(struct list *l) {
	if (!l->root)
		return 0;
	int v = l->root->data;
	struct node *r = l->root;
	l->root = l->root->next;
	if (!l->root)
		l->tail = NULL;
	free(r);
	return v;
}

void free_node(struct node *n) {
	free(n);
}

void free_list(struct list *l) {
	struct node *cur = l->root, *next;
	while (cur != NULL) {
		next = cur->next;
		free(cur);
		cur = next;
	}
	free(l);
}

void impr(struct list *l) {
	struct node *cur = l->root;
	printf("lista: ");
	while (cur) {
		printf("%i  ", cur->data);
		cur = cur->next;
	}
	printf("\n");
}

