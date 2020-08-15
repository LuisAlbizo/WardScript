/* Objects (Data Types) definition file: object.h
 * Luis Albizo "07-26-18"
 */
#ifndef obj_h
#define obj_h

//#include <stdint.h>
#include "dict.h"
#include "stack.h"
#include "scope.h"

#define W_NODE 55
#define W_BYTE 56
#define W_NIL 57
#define W_FUNCTION 58

/* Definition:
 * 	A node is the most simple Abstract Data Type
 *
 */

//typedef uint8_t byte_t;
typedef unsigned char byte_t;

struct W_Node {
	unsigned int type;
	dict *members;
};

struct W_Byte {
	unsigned int type;
	byte_t byte;
};

struct W_Nil {
	unsigned int type;
};

typedef struct W_Node	W_Node;
typedef struct W_Byte	W_Byte;
typedef struct W_Nil	W_Nil;
typedef struct W_Nil	W_Object;

#define W_FUNCTYPE 100
#define C_FUNCTYPE 101

struct W_F {
	unsigned int type;
	unsigned int ftype;
	char return_name[MAX_DICT_KEY];
	stack *argnames;
	stack *code_block;
	Scope *state; // For closure functionality
	W_Object* (*cfunc)(stack *, Scope *);
};

typedef struct W_F	W_Function;

/* Node */

W_Object *new_wnode(dict *);
dict_Data *Wnode_Get(W_Node *, char *); /* Gets a member of the Node */
void Wnode_Set(W_Node *, char *, dict_Data *); /* Set the value of a Node member, but don't
						   creates new memebers, can only modify existent
						   members (Inmutability)
						   */

/* Nil */

W_Object *new_wnil();

/* Byte */

W_Object *new_wbyte(byte_t);

/* Function */

W_Object *new_wfunction(char *, stack *, stack *, Scope *);
W_Object *new_cfunction(W_Object* (*)(stack *, Scope *));

/* Pseudo-String */

W_Object *new_char(char);
W_Object *new_string(const char *);

/* Pseudo-List */

W_Object *new_ListItem(W_Object *);
W_Object *new_List(stack *);

void free_obj(W_Object *);

#endif
