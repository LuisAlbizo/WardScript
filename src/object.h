/* Objects (Data Types) definition file: object.h
 * Luis Albizo "07-26-18"
 */
#ifndef obj_h
#define obj_h

//#include <stdint.h>
#include "dict.h"
#include "stack.h"
#include "scope.h"

#define B_NODE 55
#define B_BYTE 56
#define B_NIL 57
#define B_FUNCTION 58

/* Definition:
 * 	A node is the most simple Abstract Data Type
 *
 */

//typedef uint8_t byte_t;
typedef unsigned char byte_t;

struct B_Node {
	unsigned int type;
	dict *members;
};

struct B_Byte {
	unsigned int type;
	byte_t byte;
};

struct B_Nil {
	unsigned int type;
};

typedef struct B_Node	B_Node;
typedef struct B_Byte	B_Byte;
typedef struct B_Nil	B_Nil;
typedef struct B_Nil	B_Object;

#define B_FUNCTYPE 100
#define C_FUNCTYPE 101

struct B_F {
	unsigned int type;
	unsigned int ftype;
	char return_name[MAX_DICT_KEY];
	stack *argnames;
	stack *code_block;
	Scope *state; // For closure functionality
	B_Object* (*cfunc)(stack *, Scope *);
};

typedef struct B_F	B_Function;

/* Node */

B_Object *new_bnode(dict *);
dict_Data *Bnode_Get(B_Node *, char *); /* Gets a member of the Node */
void Bnode_Set(B_Node *, char *, dict_Data *); /* Set the value of a Node member, but don't
						   creates new memebers, can only modify existent
						   members (Inmutability)
						   */

/* Nil */

B_Object *new_bnil();

/* Byte */

B_Object *new_bbyte(byte_t);

/* Function */

B_Object *new_bfunction(char *, stack *, stack *, Scope *);
B_Object *new_cfunction(B_Object* (*)(stack *, Scope *));

/* Pseudo-String */

B_Object *new_char(char);
B_Object *new_string(const char *);

/* Pseudo-List */

B_Object *new_ListItem(B_Object *);
B_Object *new_List(stack *);

void free_obj(B_Object *);

#endif
