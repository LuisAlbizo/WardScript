/* Default Ward Interpreter configuration file: ward.h
 *
 */
#ifndef WARD_H
#define WARD_H

#include <stdio.h>
#include <stdarg.h>
#include <dlfcn.h>
#include "scope.h"
#include "object.h"
#include "stack.h"
#include "ast.h"
#include "evaluator.h"

extern int yylineno;
extern int yyparse();
extern FILE *yyin;
extern st_block *program;

/*********************
*                    *
* Built-in Functions *
*                    *
*********************/

/* present */
W_Object *w_present(stack *args, Scope *S) {
	W_Object *arg = (W_Object *) stack_pop(args);
	while (arg) {
		switch (arg->type) {
			case W_BYTE:
				printf("%d ", (int) ((W_Byte *) arg)->byte);
				break;
			case W_NIL:
				printf("<nil> ");
				break;
			case W_NODE:
				printf("<node at %p> ", arg);
				break;
			case W_FUNCTION:
				printf("<function at %p> ", arg);
				break;
			default:
				printf("<unknown: %d> ", arg->type);
				break;
		}
		arg = (W_Object *) stack_pop(args);
	}
	printf("\n");
	return new_wnil();
}

/* spresent */
W_Object *w_spresent(stack *args, Scope *S) {
	W_Object *arg = (W_Object *) stack_pop(args);
	char s[40];
	switch (arg->type) {
		case W_BYTE:
			sprintf(s, "%d", (int) ((W_Byte *) arg)->byte);
			break;
		case W_NIL:
			sprintf(s, "<nil>");
			break;
		case W_NODE:
			sprintf(s, "<node at %p>", arg);
			break;
		case W_FUNCTION:
			sprintf(s, "<function at %p>", arg);
			break;
		default:
			sprintf(s, "<unknown: %d>", arg->type);
			break;
	}
	return new_string(s);
}

/* type */
W_Object *w_type(stack *args, Scope *S) {
	if (!(args->count)) raiseError(ARGCOUNT_ERROR, "type expected one argument");
	W_Object *arg = (W_Object *) stack_pop(args);
	return new_wbyte(arg->type);
}

/* print */
W_Object *w_print(stack *args, Scope *S) {
	W_Object *arg = (W_Object *) stack_pop(args);
	while (arg) {
		switch (arg->type) {
			case W_BYTE:
				printf("%c", (char) ((W_Byte *) arg)->byte);
				break;
			case W_NODE:;
				W_Node *chr = (W_Node *) arg;
				while (1) {
					if (chr->type != W_NODE)
						break;
					printf("%c", (char) ((W_Byte *)
						Wnode_Get(chr, "$char"))->byte);
					chr = (W_Node *) Wnode_Get(chr, "$next");
				}
			default:
				break;
		}
		arg = (W_Object *) stack_pop(args);
	}
	return new_wnil();
}

/* bool */
W_Object *w_bool(stack *args, Scope *S) {
	if (!(args->count))
		raiseError(ARGCOUNT_ERROR, "bool expected one argument");
	return to_bool((W_Object *) stack_pop(args));
}

/* hasmember */
W_Object *w_has_member(stack *args, Scope *S) {
	if (args->count != 2)
		raiseError(ARGCOUNT_ERROR, "hasmember expected 2 arguments: node, membername");
	W_Node *n = (W_Node *) stack_pop(args);
	if (n->type != W_NODE)
		raiseError(TYPE_ERROR, "node must be a node");
	W_Node *m = (W_Node *) stack_pop(args);
	if (n->type != W_NODE)
		raiseError(TYPE_ERROR, "membername must be a node");
	char mname[256];
	int i = 0;
	while (i <= 255) {
		if (m->type != W_NODE)
			break;
		mname[i++] = ((W_Byte *) Wnode_Get(m, "$char"))->byte;
		m = (W_Node *) Wnode_Get(m, "$next");
	}
	mname[i] = '\0';
	if (dict_search(n->members, mname) != NULL)
		return new_wbyte(1);
	else
		return new_wbyte(0);
}

/* getmember */
W_Object *w_get_member(stack *args, Scope *S) {
	if (args->count != 2)
		raiseError(ARGCOUNT_ERROR, "getmember expected 2 arguments: node, membername");
	W_Node *n = (W_Node *) stack_pop(args);
	if (n->type != W_NODE)
		raiseError(TYPE_ERROR, "node must be a node");
	W_Node *m = (W_Node *) stack_pop(args);
	if (n->type != W_NODE)
		raiseError(TYPE_ERROR, "membername must be a node");
	char mname[256];
	int i = 0;
	while (i <= 255) {
		if (m->type != W_NODE)
			break;
		mname[i++] = ((W_Byte *) Wnode_Get(m, "$char"))->byte;
		m = (W_Node *) Wnode_Get(m, "$next");
	}
	mname[i] = '\0';
	return (W_Object *) Wnode_Get(n, mname);
}

/* import */
W_Object *w_import(stack *args, Scope *S) {
	if (!(args->count)) raiseError(ARGCOUNT_ERROR, "import expected one argument");
	char fname[129];
	char pfname[130];
	int i = 0;
	W_Node *chr = (W_Node *) stack_pop(args);
	W_Object *module;
	while (i <= 128) {
		if (chr->type != W_NODE)
			break;
		fname[i++] = ((W_Byte *) Wnode_Get(chr, "$char"))->byte;
		chr = (W_Node *) Wnode_Get(chr, "$next");
	}
	fname[i] = '\0';
	pfname[0] = '.';
	memcpy(&pfname[1], fname, 129);
	// Checks if the private variable exists to avoid recursive imports
	if(Scope_Get(S, pfname)!=NULL)
		return (W_Object *) Scope_Get(S, pfname);
	// Define the private module variable if not exists
	Scope_Set(S, pfname, (Scope_Object *) new_wnil());
	// Check in the so libraries
	char solib[149] = "";
	sprintf(solib, "libward_%s.so", fname);
	void *lib_so = dlopen(solib, RTLD_LAZY);
	if (lib_so) {
		W_Object *(*module_loader)(stack *args, Scope *S);
		module_loader = dlsym(lib_so, "module_loader");
		char *error_msg = dlerror();
		if (error_msg) {
			fprintf(stderr, "Error loading so module: %s\n", error_msg);
			exit(1);
		}
		module = (*module_loader)(args, S);
		return module;
	}
	// Check the standard ward library
	Scope *MS;
	st_st *modfunc;
	sprintf(solib, "%s/lib/ward/%s.ward", getenv("PREFIX"), fname);
	yyin = fopen(solib, "r");
	if (yyin) {
		if (yyparse())
			exit(33);
		MS = newScope(S);
		modfunc = new_object(new_wfunction("$module", newstack(), program->block, NULL));
		module = ((st_object *) eva_(new_call(modfunc, newstack()), MS))->obj;
		return module;
	}
	// Check in the current directory
	sprintf(solib, "%s.ward", fname);
	yyin = fopen(solib, "r");
	if (!yyin) {
		perror(solib);
		exit(13);
	}
	if (yyparse())
		exit(33);

	MS = newScope(S);
	modfunc = new_object(new_wfunction("$module", newstack(), program->block, NULL));
	module = ((st_object *) eva_(new_call(modfunc, newstack()), MS))->obj;
	Scope_Set(S, pfname, (Scope_Object *) module);
	return module;
}

/* input */
#define MAX_INPUT 255
W_Object *w_input(stack *args, Scope *S) {
	size_t size = MAX_INPUT;
	char *s = malloc(MAX_INPUT);
	if (!s) {
		fprintf(stderr, "Can't allocate memory for input\n");
		exit(1);
	}
	w_print(args, S);
	int input_size = getline(&s, &size, stdin);
	if (input_size == -1) {
		fprintf(stderr, "Error: No Line\n");
		exit(1);
	}
	s[input_size-1] = '\0';
	return new_string(s);
}

/* finish */
W_Object *w_finish(stack *args, Scope *S) {
	W_Byte *e = (W_Byte *) stack_pop(args);
	int code;
	if (!e) e = (W_Byte *) Scope_Get(S, "exit_code");
	code = e->byte;
	exit(code);
	return NULL;
}

/*********************
*                    *
*    Program Sets    *
*                    *
*********************/

int main(int argc, char **argv) {
	// Command: ward
	if (argc < 2) {
		perror("No input file");
		exit(1);
	}
	yyin = fopen(argv[1], "r");
	if (!yyin) {
		perror(argv[1]);
		exit(13);
	}
	if (yyparse()) {
		exit(33);
	}
	// Global Scope
	Scope *GS = newScope(NULL);
	// -Built In Functions
	Scope_Set(GS, "present", (Scope_Object *) new_cfunction(&w_present));
	Scope_Set(GS, "spresent",(Scope_Object *) new_cfunction(&w_spresent));
	Scope_Set(GS, "print",   (Scope_Object *) new_cfunction(&w_print));
	Scope_Set(GS, "input",   (Scope_Object *) new_cfunction(&w_input));
	Scope_Set(GS, "import",  (Scope_Object *) new_cfunction(&w_import));
	Scope_Set(GS, "finish",  (Scope_Object *) new_cfunction(&w_finish));
	Scope_Set(GS, "bool",    (Scope_Object *) new_cfunction(&w_bool));
	Scope_Set(GS, "type",    (Scope_Object *) new_cfunction(&w_type));
	Scope_Set(GS, "hasmember",(Scope_Object *) new_cfunction(&w_has_member));
	Scope_Set(GS, "getmember",(Scope_Object *) new_cfunction(&w_get_member));
	// -Predefined Variables
	Scope_Set(GS, "nil",      (Scope_Object *) new_wnil());
	Scope_Set(GS, "exit_code",(Scope_Object *) new_wbyte(0));
	// --Args
	stack *args = newstack();
	argc = argc - 2;
	while (argc) {
		stack_push(args, (stack_Data *) new_string(argv[argc+1]));
		argc--;
	}
	Scope_Set(GS, "args",    (Scope_Object *) new_List(args));
	// --Types
	Scope_Set(GS, "NIL",     (Scope_Object *) new_wbyte(W_NIL));
	Scope_Set(GS, "NODE",    (Scope_Object *) new_wbyte(W_NODE));
	Scope_Set(GS, "BYTE",    (Scope_Object *) new_wbyte(W_BYTE));
	Scope_Set(GS, "FUNCTION",(Scope_Object *) new_wbyte(W_FUNCTION));

	st_st *mainfunc = new_object(new_wfunction("exit_code",
			newstack(), program->block, NULL));
	W_Byte *return_code = (W_Byte *) ((st_object *) eva_(
			new_call(mainfunc, newstack()), GS))->obj;
	return (int) return_code->byte;
}

#endif
