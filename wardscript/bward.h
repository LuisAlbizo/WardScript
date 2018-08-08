/* Default Ward Interpreter configuration file: bward.h
 *
 */
#ifndef bward_h
#define bward_h

#include <stdio.h>
#include <stdarg.h>
#include "scope.h"
#include "object.h"
#include "stack.h"
#include "ast.h"
#include "evaluator.h"

extern int yylineno;
extern int yyparse();
extern FILE *yyin;
extern st_block *program;

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
			default:
				printf("<unknown: %d> ", arg->type);
				break;
		}
		arg = (B_Object *) stack_pop(args);
	}
	printf("\n");
	return new_bnil();
}

B_Object *print(stack *args, Scope *S) {
	B_Object *arg = (B_Object *) stack_pop(args);
	while (arg) {
		switch (arg->type) {
			case B_BYTE:
				printf("%c", ((B_Byte *) arg)->byte);
				break;
			case B_NODE:;
				B_Node *chr = (B_Node *) arg;
				while (1) {
					if (chr->type != B_NODE)
						break;
					printf("%c", ((B_Byte *) Bnode_Get(chr, "$char"))->byte);
					chr = (B_Node *) Bnode_Get(chr, "$next");
				}
			default:
				break;
		}
		arg = (B_Object *) stack_pop(args);
	}
	return new_bnil();
}

B_Object *import(stack *args, Scope *S) {
	char fname[129];
	int i = 0;
	B_Node *chr = (B_Node *) stack_pop(args);
	while (i <= 128) {
		if (chr->type != B_NODE)
			break;
		fname[i++] = ((B_Byte *) Bnode_Get(chr, "$char"))->byte;
		chr = (B_Node *) Bnode_Get(chr, "$next");
	}
	fname[i] = '\0';
	
	yyin = fopen(fname, "r");
	if (!yyin) {
		perror(fname);
		exit(13);
	}
	if (yyparse())
		exit(33);

	Scope *MS = newScope(S);

	st_st *modfunc = new_object(new_bfunction("module$", newstack(), program->block));

	B_Object *module = ((st_object *) eva_(new_call(modfunc, newstack()), MS))->obj;

	return module;
}

/* Global Scope */

int main(int argc, char **argv) {
	if (argc < 2) {
		perror("No input");
		exit(1);
	}
	yyin = fopen(argv[1], "r");
	if (!yyin) {
		perror(argv[1]);
		exit(13);
	}
	if (yyparse())
		exit(33);

	Scope *GS = newScope(NULL);

	Scope_Set(GS, "present",	(Scope_Object *) new_cfunction(&present));
	Scope_Set(GS, "print",		(Scope_Object *) new_cfunction(&print));
	Scope_Set(GS, "import",		(Scope_Object *) new_cfunction(&import));
	Scope_Set(GS, "nil",		(Scope_Object *) new_bnil());
	Scope_Set(GS, "exit_code",	(Scope_Object *) new_bbyte(0));

	st_st *mainfunc = new_object(new_bfunction("exit_code", newstack(), program->block));

	B_Byte *return_code = (B_Byte *) ((st_object *) eva_(new_call(mainfunc, newstack()), GS))->obj;

	return (int) return_code->byte;
}

#endif
