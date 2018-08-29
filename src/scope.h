/* Scope definition file: scope.h
 * Luis Albizo "07-24-18"
 */
#ifndef scope_h
#define scope_h
#include "dict.h"

/* The scope is a enviroment where a name has one object associated.
 *
 * There can be multiple objects in multiple enviroments with the same name,
 * only a name is updated when it is modified in the same enviroment otherwise is
 * declared.
 */

struct scope {
	struct scope *upscope;
	dict *vars;
};

typedef struct scope Scope;
typedef dict_Data Scope_Object;

Scope *newScope(Scope *);
void free_Scope(Scope *);

/* Scope Functions to set/get variables */

void Scope_Set(Scope *, char *, Scope_Object *); /* Set (assign) a name in the Scope

					      If the name already exists only change the object associated
					      with that name.

						*/

void Scope_NLSet(Scope *, char *, Scope_Object *); /* Set (assign) a name searching the name recursively
						      starting from the up-scope of 'Scope *'
						*/

Scope_Object *Scope_Get(Scope *, char *); /* Returns the object stored in the scope associated with a name.

				       Search in the Scope if a name exists then return the object
				       else check in the upscope recursively, if reaches the root and
				       can't found the name then raises an error of Undeclared Variable.

				       */

void Scope_Concat(Scope *, Scope *); /* This concats two scopes, the first is the scope
					most closely, the second is the scope more distant.

					This is made it for implementing closures.
					*/

#endif
