/* General-purpose Stack Data Structure definition file: dict.h
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

void stack_push(stack *, stack_Data *);
stack_Data *stack_pop(stack *);
stack_data *traverse(stack *);

/* Stack Memory Free functions */

void freestack_Data(stack_Data *);
void freestack_node(stack_node *);
void freestack(stack *);


