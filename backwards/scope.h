/* Scope definition file: scope.h
 * Luis Albizo "07-24-18"
 */
#ifndef scope_h
#define scope_h
#include "dict.h"

/* The scope is the enviroment where a variable exists
 */

struct scope {
	struct scope *upscope;
	dict *vars;
};

typedef struct scope Scope;
typedef dict_Data object;

/* Scope Functions to get variables */

void Scope_Set(Scope *, char *, object *); /* Set (assign) a name in the Scope

					      If the name already exists only change the object associated
					      with that name.

					      */
object *Scope_Get(Scope *, char *); /* Returns the object stored in the scope associated with a name.

				       Search in the Scope if a name exists then return the object
				       else check in the upscope recursively, if reaches the root and
				       can't found the name then raises an error of Undeclared Variable.

				       */



#endif
