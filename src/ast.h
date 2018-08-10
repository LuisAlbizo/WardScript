/* Syntax Tree Data Structure definition file: ast.h
 * Luis Albizo "07-20-18"
 */
#ifndef ast_h
#define ast_h

#include "dict.h"
#include "stack.h"
#include "object.h"
#include <stdlib.h>

	/* AST Nodetypes */
#define AST_BLOCK	1
#define AST_ASSIGNMENT	2
#define AST_BOP		3
#define AST_UOP		4
#define AST_CALL	5
#define AST_METHODCALL	6
#define AST_IF		7
#define AST_FOREVER	8
#define AST_EXIT	9
#define AST_MEMBER_A	10
#define AST_MEMBER	11
#define AST_NAME	12
#define AST_NAMES	13
#define AST_NODE_C	14
#define AST_LIST_C	15
#define AST_OBJECT	16

/* AST struct definition for each type of statement
*/

struct st {
	unsigned int type;
};

struct block {
	unsigned int type; // AST_BLOCK
	stack *block; // Stack of statements
};

struct assign {
	char name[MAX_DICT_KEY];
	struct st *value;
};

struct assignment {
	unsigned int type; // AST_ASSIGNMENT
	stack *assigns; // Stack of struct assign
};

struct bop {
	unsigned int type; // AST_BOP
	char op;
	struct st *left;
	struct st *right;
};

struct uop {
	unsigned int type; // AST_UOP
	char op;
	struct st *right;
};

struct name {
	unsigned int type; // AST_NAME
	char name[MAX_DICT_KEY]; // From dict.h
};

struct call {
	unsigned int type; // AST_CALL
	struct st *callable;
	stack *args; // Stack of statements
};

struct node_c {
	unsigned int type; // AST_NODE_C
	struct assignment *members;
};

struct list_c {
	unsigned int type; // AST_LIST_C
	stack *items;
};

struct member {
	unsigned int type; // AST_MEMBER
	struct st *object;
	char name[MAX_DICT_KEY];
};

struct member_a {
	unsigned int type; // AST_MEMBER_A
	struct member *member;
	struct st *object;
};

struct methodcall {
	unsigned int type; // AST_METHODCALL
	struct st *object; // Object to pass as a self parameter (node)
	char method[MAX_DICT_KEY]; // Member name of object callable
	stack *args; // stack of statements
};

struct argnames {
	unsigned int type; // AST_NAMES
	stack *names;
};

struct if_ {
	unsigned int type; // AST_IF
	struct st *condition;
	struct block *block_if;
	struct block *block_else;
};

struct forever {
	unsigned int type; // AST_FOREVER
	struct block *block;
};

struct object {
	unsigned int type; // AST_OBJECT
	B_Object *obj;
};

typedef struct assign		assign;
typedef struct st		st_st;
typedef struct block		st_block;
typedef struct assignment	st_assignment;
typedef struct bop		st_bop;
typedef struct uop		st_uop;
typedef struct name		st_name;
typedef struct call		st_call;
typedef struct member		st_member;
typedef struct node_c		st_node_construct;
typedef struct list_c		st_list_construct;
typedef struct member_a		st_member_assign;
typedef struct methodcall	st_methodcall;
typedef struct argnames		st_argnames;
typedef struct if_		st_if;
typedef struct forever		st_forever;
typedef struct object		st_object;

/* AST Creation */

void *astalloc(size_t); /* Check if there is disponible memory for allocate a new ast
			   if there is memory left then return the block of memory (void *)
			   otherwise prints an error and finish the program with exit code 1
			   */
st_st *new_st();
st_st *new_block(stack *);
assign *new_assign(char *, st_st *);
st_st *new_assignment(stack *);
st_st *new_bop(char, st_st *, st_st *);
st_st *new_uop(char, st_st *);
st_st *new_name(char *);
st_st *new_call(st_st *, stack *);
st_st *new_node_construct(st_assignment *);
st_st *new_list_construct(stack *);
st_st *new_member(st_st *, char *);
st_st *new_member_assign(st_member *, st_st *);
st_st *new_methodcall(st_st *, char *, stack *);
st_st *new_argnames(stack *);
st_st *new_if(st_st *, st_block *, st_block *);
st_st *new_forever(st_block *);
st_st *new_object(B_Object *);

/* AST Deletion */

void free_assign(assign *);
void free_st(st_st *);

#endif
