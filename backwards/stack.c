/* General-purpose Stack Data Structure implementation file: stack.c
 * Luis Albizo "07-22-18"
 */
#include "stack.h"
#include <stdlib.h>

/* Stack Memory Allocation functions */

stack_node *newstack_node(stack_Data *data, stack_node *next) {
	stack_node *node = malloc(sizeof(stack_node));
	node->data = data;
	node->next = next;
	return node;
}

stack *newstack() {
	stack *s = malloc(sizeof(stack));
	s->count = 0;
	s->top = NULL;
	return s;
}

/* Stack Functions */

void stack_push(stack *s, stack_Data *d) {
	stack_node *n = newstack_node(d, s->top);
	s->top = n;
}

stack_Data *stack_pop(stack *s) {
	if (!s->top)
		return NULL;
	stack_Data *d = s->top->data;
	free(s->top);
	s->top = NULL;
	return d;
}

/* Stack Memory Free functions */

// XXX TODO

void freestack_Data(stack_Data *d) {
	free(d);
}

void freestack_node(stack_node *n) {
	free(n);
}

void freestack(stack *s) {
	free(s);
}

