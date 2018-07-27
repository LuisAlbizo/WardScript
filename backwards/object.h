/* Objects (Data Types) definition file: object.h
 * Luis Albizo "07-26-18"
 */
#ifndef obj_h
#define obj_h

#include "dict.h"
#include "stack.h"

#define B_NODE 55
#define B_NUMBER 56
#define B_NIL 57
#define B_FUNCTION 58

/* Definition:
 * 	A node is the most simple Abstract Data Type
 *
 */

struct B_Node {
	unsigned int type;
	dict *members;
};

struct B_Number {
	unsigned int type;
	long number;
};

struct B_Nil {
	unsigned int type;
};

struct B_F {
	unsigned int type;
	char return_name[MAX_DICT_KEY];
	stack *argnames;
	st_block *code_block;
};

typedef struct B_Node	B_Node;
typedef struct B_Number	B_Number;
typedef struct B_Nil	B_Nil;
typedef struct B_Nil	B_Object;
typedef struct B_F	B_Function;

/* Node */

B_Object *new_bnode(dict *);
dict_Data *Bnode_Get(B_Node *, char *); /* Gets a member of the Node */
void *Bnode_Set(B_Node *, char *, dict_Data *); /* Set the value of a Node member, but don't
						   creates new memebers, can only modify existent
						   member (Inmutability)
						   */

/* Nil */

B_Object *new_bnil();

/* Number */

B_Object *new_bnumber(long);

/* Function */

B_Object *new_bfunction(char *, stack *, st_block *);

void free_obj(B_Object *);

#endif
