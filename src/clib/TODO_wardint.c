/* C Integer Types library file: wardint.c
 * Luis Albizo "08-13-18"
 */

#include <stdlib.h>
#include <ward/dict.h>
#include <ward/scope.h>
#include <ward/object.h>
#include <ward/stack.h>
#include <ward/error.h>

/*******************
* Type Definitions *
*                  *
*******************/

/* Integer Types */

#define W_UINT   10
#define W_INT    11

/* Type Structures */

struct W_uint {
	char type;
	unsigned int data;
};

struct W_int {
	char type;
	int data;
};

/* Type Constructors */

W_Object *new_wuint(unsigned int n) {
	struct W_uint *u = malloc(sizeof(struct W_uint));
	if (!u)
		raiseError(MEMORY_ERROR, "can't create new Object UINT");
	u->type = W_UINT;
	u->data = n;
	return (W_Object *) u;
}

W_Object *new_wint(int n) {
	struct W_int *u = malloc(sizeof(struct W_int));
	if (!u)
		raiseError(MEMORY_ERROR, "can't create new Object INT");
	u->type = W_INT;
	u->data = n;
	return (W_Object *) u;
}

/*******************
*     Functions    *
*                  *
*******************/

/* uint */
W_Object *ward_integer_UINT(stack *args, Scope *S) {
	if (!args->count)
		return new_wuint(0);
	W_Object *n = (W_Object *) stack_pop(args);
	if (n->type == W_BYTE)
		return new_wuint(((W_Byte *) n)->byte);
}

/* int */
W_Object *ward_integer_INT(stack *args, Scope *S) {
	if (!args->count)
		return new_wint(0);
	W_Object *n = (W_Object *) stack_pop(args);
	if (n->type == W_BYTE)
		return new_wint(((W_Byte *) n)->byte);
}

/* ulong */
W_Object *ward_integer_ULONG(stack *args, Scope *S) {
	if (!args->count)
		return new_wulong(0);
W_Object *n = (W_Object *) stack_pop(args);
	if (n->type == W_BYTE)
		return new_wulong(((W_Byte *) n)->byte);
}

/* long */
W_Object *ward_integer_LONG(stack *args, Scope *S) {
	if (!args->count)
		return new_wlong(0);
	W_Object *n = (W_Object *) stack_pop(args);
	if (n->type == W_BYTE)
		return new_wlong(((W_Byte *) n)->byte);
}

/* ushort */
W_Object *ward_integer_USHORT(stack *args, Scope *S) {
	if (!args->count)
		return new_wushort(0);
	W_Object *n = (W_Object *) stack_pop(args);
	if (n->type == W_BYTE)
		return new_wushort(((W_Byte *) n)->byte);
}

/* short */
W_Object *ward_integer_SHORT(stack *args, Scope *S) {
	if (!args->count)
		return new_wshort(0);
	W_Object *n = (W_Object *) stack_pop(args);
	if (n->type == W_BYTE)
		return new_wshort(((W_Byte *) n)->byte);
}

// Operations

W_Object *ward_integer_add(stack *args, Scope *S) {
	if (args->count != 2)
		raiseError(ARGCOUNT_ERROR,"add takes exactly 2 arguments");
	W_Object *x = (W_Object *) stack_pop(args);
	W_Object *y = (W_Object *) stack_pop(args);
}

/*******************
*  Module Loader   *
*                  *
*******************/
W_Object *module_loader(stack *args, Scope *S) {
	while (S->upscope)
		S = S->upscope;

	dict *d = newdict();
	// Variables
	// -Types
	dict_update(d, "UINT",  (dict_Data *) new_wbyte(W_UINT));
	dict_update(d, "INT",   (dict_Data *) new_wbyte(W_INT));
	// Functions
	// -Constructors
	dict_update(d, "uint",  (dict_Data *) new_cfunction(ward_integer_UINT));
	dict_update(d, "int",   (dict_Data *) new_cfunction(ward_integer_INT));
	// -Operations
	// --Arithmethic
	dict_update(d, "add", (dict_Data *) new_cfunction(ward_integer_add));
	dict_update(d, "sub", (dict_Data *) new_cfunction(ward_integer_sub));
	dict_update(d, "mul", (dict_Data *) new_cfunction(ward_integer_mul));
	dict_update(d, "div", (dict_Data *) new_cfunction(ward_integer_div));
	dict_update(d, "mod", (dict_Data *) new_cfunction(ward_integer_mod));
	dict_update(d, "pow", (dict_Data *) new_cfunction(ward_integer_pow));
	// --Bitwise
	return new_wnode(d);
}

