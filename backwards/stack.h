/* General-purpose Stack Data Structure definition file: dict.h
 * Luis Albizo "07-22-18"
 */
#ifndef stack_h
#define stack_h

/* Stack is usefull in:
 * 	- Saving recursive calls
 * 	- Saving statements in code blocks like: 
 * 	loops, if-statements, functions or a program
 * 	- Assignments
 */

struct stack_Data {
	unsigned int type;
};

struct stack_node {
	struct stack_Data *data;
	struct stack_node *next;
};

struct stack {
	unsigned int count;
	struct stack_node *top;
};

typedef struct stack_Data stack_Data;
typedef struct stack_node stack_node;
typedef struct stack stack;

/* Stack Memory Allocation functions */
stack_node *newstack_node(stack_Data *, stack_node *);
stack *newstack();
/* Stack Functions */
void stack_push(stack *, stack_Data *);
stack_Data *stack_pop(stack *);
stack_Data *traverse(stack *);
/* Stack Memory Free functions */
void freestack_Data(stack_Data *);
void freestack_node(stack_node *);
void freestack(stack *);

#endif
