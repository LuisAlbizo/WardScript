/* Default BackWard Interpreter configuration file: bward.h
 *
 */
#ifndef bward_h
#define bward_h

#include <stdio.h>
#include <stdarg.h>
#include "scope.h"
#include "object.h"
#include "stack.h"

extern int yylineno;

void yyerror(char *s, ...) {
	va_list ap;
	va_start(ap, s);

	fprintf(stderr, "%d: error: ", yylineno);
	vfprintf(stderr, s, ap);
	fprintf(stderr, "\n"); 
}

/* Built-in Functions */

B_Object *present(stack *args, Scope *S) {
	B_Object *arg = (B_Object *) stack_pop(args);
	while (arg) {
		switch (arg->type) {
			case B_BYTE:
				printf("%d ", (int) ((B_Byte *) arg)->byte);
				break;
			case B_NIL:
				printf("<nil> ");
				break;
			case B_NODE:
				printf("<node> ");
				break;
			case B_FUNCTION:
				printf("<function> ");
				break;
		}
		arg = (B_Object *) stack_pop(args);
	}
	printf("\n");
	return new_bnil();
}

/* Global Scope */

int main() {
	Scope *GS = newScope(NULL);

	Scope_Set(GS, "present",	(Scope_Object *) new_cfunction(&present));
	Scope_Set(GS, "nil",		(Scope_Object *) new_bnil());
	Scope_Set(GS, "exit_code",	(Scope_Object *) new_bbyte(0));
	printf("backward\n");
	return 0;
}

#endif
