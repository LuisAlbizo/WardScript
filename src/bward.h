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

B_Object *w_present(stack *args, Scope *S) {
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
				printf("<node at %p> ", arg);
				break;
			case B_FUNCTION:
				printf("<function at %p> ", arg);
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

B_Object *w_print(stack *args, Scope *S) {
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

B_Object *w_bool(stack *args, Scope *S) {
	return to_bool((B_Object *) stack_pop(args));
}

B_Object *w_import(stack *args, Scope *S) {
	if (!(args->count)) raiseError(ARGCOUNT_ERROR, "import expected one argument");
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

	st_st *modfunc = new_object(new_bfunction("$module", newstack(), program->block, NULL));

	B_Object *module = ((st_object *) eva_(new_call(modfunc, newstack()), MS))->obj;

	return module;
}

#define MAX_INPUT 1024

B_Object *w_input(stack *args, Scope *S) {
	size_t size = MAX_INPUT;
	char *s = malloc(MAX_INPUT);
	if (!s) {
		fprintf(stderr, "Can't allocate memory for input\n");
		exit(1);
	}
	w_print(args, S);
	if (getline(&s, &size, stdin) == -1) {
		fprintf(stderr, "Error: No Line\n");
		exit(1);
	}
	s[MAX_DICT_KEY-1] = '\0';
	return new_string(s);
}

B_Object *w_finish(stack *args, Scope *S) {
	B_Byte *e = (B_Byte *) stack_pop(args);
	int code;
	if (!e) e = (B_Byte *) Scope_Get(S, "exit_code");
	code = e->byte;
	exit(code);
	return NULL;
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

	Scope_Set(GS, "present",	(Scope_Object *) new_cfunction(&w_present));
	Scope_Set(GS, "print",		(Scope_Object *) new_cfunction(&w_print));
	Scope_Set(GS, "input",		(Scope_Object *) new_cfunction(&w_input));
	Scope_Set(GS, "import",		(Scope_Object *) new_cfunction(&w_import));
	Scope_Set(GS, "finish",		(Scope_Object *) new_cfunction(&w_finish));
	Scope_Set(GS, "bool",		(Scope_Object *) new_cfunction(&w_bool));
	Scope_Set(GS, "nil",		(Scope_Object *) new_bnil());
	Scope_Set(GS, "exit_code",	(Scope_Object *) new_bbyte(0));

	st_st *mainfunc = new_object(new_bfunction("exit_code", newstack(), program->block, NULL));

	B_Byte *return_code = (B_Byte *) ((st_object *) eva_(new_call(mainfunc, newstack()), GS))->obj;

	return (int) return_code->byte;
}

#endif
