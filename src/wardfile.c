/* File manipulation library file: wardfile.c
 * Luis Albizo "08-13-18"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dict.h"
#include "scope.h"
#include "object.h"
#include "stack.h"
#include "error.h"

/* Open Modes */
#define READ 1
#define WRITE 2
#define APPEND 3
#define READ_PLUS 4
#define WRITE_PLUS 5
#define APPEND_PLUS 6
/* Seek Options */
#define SET 11
#define CUR 12
#define END 13

struct w_file {
	unsigned int openmode;
	FILE *file;
};

typedef struct w_file w_file;

w_file *new_file(const char *fname, char omode) {
	w_file *wf = malloc(sizeof(w_file));
	if (!wf)
		raiseError(MEMORY_ERROR, "can't create a new file");
	FILE *f = NULL;
	switch (omode) {
		case READ:
			f = fopen(fname, "rb");
			break;
		case READ_PLUS:
			f = fopen(fname, "rb+");
			break;
		case WRITE:
			f = fopen(fname, "wb");
			break;
		case WRITE_PLUS:
			f = fopen(fname, "wb+");
			break;
		case APPEND:
			f = fopen(fname, "ab");
			break;
		case APPEND_PLUS:
			f = fopen(fname, "ab+");
			break;
		default:
			break;
	}
	if (!f) {
		return NULL;
	}
	wf->openmode = omode;
	wf->file = f;
	return wf;
}

B_Object *int_to_number(int number) {
	B_Node *root = (B_Node *) new_bnode();


}

/* Interface (Ward Functions) */

B_Object *ward_file_open(stack *args, Scope *S) {
	if (args->count != 2)
		raiseError(ARGCOUNT_ERROR, "fopen requires 2 arguments: filename, openmode");
	B_Node *fname = (B_Node *) stack_pop(args);
	if (fname->type != B_NODE)
		raiseError(TYPE_ERROR, "filename must be a node");
	B_Byte *omode = (B_Byte *) stack_pop(args);
	if (omode->type != B_BYTE)
		raiseError(TYPE_ERROR, "openmode must be a byte");
	/* Get the filename */
	char sfname[129];
	int i = 0;
	B_Node *chr = fname;
	while (i <= 128) {
		if (chr->type == B_NIL)
			break;
		sfname[i++] = ((B_Byte *) Bnode_Get(chr, "$char"))->byte;
		chr = (B_Node *) Bnode_Get(chr, "$next");
	}
	sfname[i] = '\0';
	/* Create the file object */
	w_file *wf = new_file(sfname, omode->byte);
	if (!wf)
		return new_bnil();
	/* Create the node */
	dict *fdict = newdict();
	dict_update(fdict, ".file", (dict_Data *) wf); /* Set file pointer this way so becomes unreachable
							from ward.
							*/
	dict_update(fdict, "filename", (dict_Data *) fname);
	dict_update(fdict, "openmode", (dict_Data *) omode);
	return new_bnode(fdict);
}

B_Object *ward_file_put(stack *args, Scope *S) {
	if (args->count != 2)
		raiseError(ARGCOUNT_ERROR, "put requires 2 arguments: filenode, byte");
	B_Node *fnode = (B_Node *) stack_pop(args);
	if (fnode->type != B_NODE)
		raiseError(TYPE_ERROR, "filenode must be a node");
	B_Byte *byte = (B_Byte *) stack_pop(args);
	if (byte->type != B_BYTE)
		raiseError(TYPE_ERROR, "byte must be a byte");
	w_file *f = (w_file *) Bnode_Get(fnode, ".file");
	int r = fputc(byte->byte, f->file);
	if (r != EOF)
		return new_bbyte((char) r);
	else
		return new_bnil();
}

B_Object *ward_file_get(stack *args, Scope *S) {
	B_Node *fnode = (B_Node *) stack_pop(args);
	if (!fnode)
		raiseError(ARGCOUNT_ERROR, "get requires one argument: filenode");
	if (fnode->type != B_NODE)
		raiseError(TYPE_ERROR, "filenode must be a node");
	w_file *f = (w_file *) Bnode_Get(fnode, ".file");
	int r = fgetc(f->file);
	if (r != EOF)
		return new_bbyte((char) r);
	else
		return new_bnil();
}

B_Object *ward_file_puts(stack *args, Scope *S) {
	if (args->count != 2)
		raiseError(ARGCOUNT_ERROR, "puts requires 2 arguments: filenode, string");
	B_Node *fnode = (B_Node *) stack_pop(args);
	if (fnode->type != B_NODE)
		raiseError(TYPE_ERROR, "filenode must be a node");
	B_Node *string = (B_Node *) stack_pop(args);
	if (string->type != B_NODE)
		raiseError(TYPE_ERROR, "string must be a node");
	w_file *f = (w_file *) Bnode_Get(fnode, ".file");
	int r = fputc(byte->byte, f->file);
	if (r != EOF)
		return new_bbyte((char) r);
	else
		return new_bnil();
}

B_Object *ward_file_get(stack *args, Scope *S) {
	B_Node *fnode = (B_Node *) stack_pop(args);
	if (!fnode)
		raiseError(ARGCOUNT_ERROR, "get requires one argument: filenode");
	if (fnode->type != B_NODE)
		raiseError(TYPE_ERROR, "filenode must be a node");
	w_file *f = (w_file *) Bnode_Get(fnode, ".file");
	int r = fgetc(f->file);
	if (r != EOF)
		return new_bbyte((char) r);
	else
		return new_bnil();
}

B_Object *ward_file_close(stack *args, Scope *S) {



}


/* Module Loader */
B_Object *module_loader(stack *args, Scope *S) {
	while (S->upscope)
		S = S->upscope;
	Scope_Set(S, ".file-close",	(Scope_Object *) new_cfunction(ward_file_close));
	// creating the module node
	dict *d = newdict();
	dict_update(d, "file", (dict_Data *) new_cfunction(ward_file_open));
	return new_bnode(d);
}





