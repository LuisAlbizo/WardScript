/* Objects (Data Types) definition file: object.h
 * Luis Albizo "07-26-18"
 */
#ifndef obj_h
#define obj_h

//#include <stdint.h>
#include "dict.h"
#include "stack.h"
#include "scope.h"

#define W_BYTE     1
#define W_NODE     2
#define W_NIL      3
#define W_FUNCTION 4

unsigned int _pow(unsigned int, unsigned int);

//typedef uint8_t byte_t;
typedef unsigned char byte_t;

struct W_Node {
	char type;
	dict *members;
};

struct W_Byte {
	char type;
	byte_t byte;
};

struct W_Nil {
	char type;
};

typedef struct W_Node	W_Node;
typedef struct W_Byte	W_Byte;
typedef struct W_Nil	W_Nil;
typedef struct W_Nil	W_Object;

#define W_FUNCTYPE 100
#define C_FUNCTYPE 101

struct W_F {
	char type;
	char ftype;
	char return_name[MAX_DICT_KEY];
	stack *argnames;
	stack *code_block;
	Scope *state; // For closure functionality
	W_Object* (*cfunc)(stack *, Scope *);
};

typedef struct W_F W_Function;

/* Node */

W_Object *new_wnode(dict *);
/* Gets a member of the Node */
dict_Data *Wnode_Get(W_Node *, const char *);
/* Set the value of a Node member, but don't
 * creates new memebers, can only modify existent
 * members (Inmutability) */
void Wnode_Set(W_Node *, const char *, dict_Data *);

/* Nil */

W_Object *new_wnil();

/* Byte */

W_Object *new_wbyte(byte_t);

/* Function */

W_Object *new_wfunction(const char *, stack *, stack *, Scope *);
W_Object *new_cfunction(W_Object* (*)(stack *, Scope *));

/* Pseudo-Integer */

W_Object *new_wnumber(unsigned int);
unsigned int wnumber_to_int(W_Node *);

/* Pseudo-String */

W_Object *new_char(char);
W_Object *new_string(const char *);

/* Pseudo-List */

W_Object *new_ListItem(W_Object *);
W_Object *new_List(stack *);

void free_obj(W_Object *);

#endif
