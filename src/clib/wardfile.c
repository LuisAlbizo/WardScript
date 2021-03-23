/* File manipulation library file: wardfile.c
 * Luis Albizo "08-13-18"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ward/dict.h>
#include <ward/scope.h>
#include <ward/object.h>
#include <ward/stack.h>
#include <ward/error.h>

#define MAXBUFFER 255

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

w_file *new_prefile(FILE *f, int omode) {
	// Function to create w_file structs for stdin, stdout and stderr
	w_file *wf = malloc(sizeof(w_file));
	if (!wf)
		raiseError(MEMORY_ERROR, "can't create a new file");
	wf->file = f;
	wf->openmode = omode;
	return wf;
}

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
	if (!f)
		return NULL;
	wf->openmode = omode;
	wf->file = f;
	return wf;
}

/* File Methods */

void assign_file_methods(dict *d, Scope *S) {
	// Function to assing the file methods to a node
	dict_update(d, "get",  (dict_Data *) Scope_Get(S, ".file-get"));
	dict_update(d, "gets", (dict_Data *) Scope_Get(S, ".file-gets"));
	dict_update(d, "put",  (dict_Data *) Scope_Get(S, ".file-put"));
	dict_update(d, "puts", (dict_Data *) Scope_Get(S, ".file-puts"));
	dict_update(d, "close", (dict_Data *) Scope_Get(S, ".file-close"));
}

/* Open
 * Function to open new files: open(filename, openmode)
 * `filename` should be a string and `openmode` a byte.
 *
 *     file := import("file");
 *     f := file.open("file.txt", file.READ);
 *
 * returns a new node in case of no error that contains:
 *     {
 *          filename (string)
 *          openmode (byte)
 *          get      (method)
 *          gets     (method)
 *          put      (method)
 *          puts     (method)
 *          close    (method)
 *     }
 * in the case an error occurs when trying to open a file then returns nil.
 */
W_Object *ward_file_open(stack *args, Scope *S) {
	if (args->count != 2)
		raiseError(ARGCOUNT_ERROR, "open requires 2 arguments: filename, openmode");
	W_Node *fname = (W_Node *) stack_pop(args);
	if (fname->type != W_NODE)
		raiseError(TYPE_ERROR, "filename must be a node");
	W_Byte *omode = (W_Byte *) stack_pop(args);
	if (omode->type != W_BYTE)
		raiseError(TYPE_ERROR, "openmode must be a byte");
	/* Get the filename */
	char sfname[129];
	int i = 0;
	W_Node *chr = fname;
	while (i <= 128) {
		if (chr->type == W_NIL)
			break;
		sfname[i++] = ((W_Byte *) Wnode_Get(chr, "$char"))->byte;
		chr = (W_Node *) Wnode_Get(chr, "$next");
	}
	sfname[i] = '\0';
	/* Create the file object */
	w_file *wf = new_file(sfname, omode->byte);
	if (!wf)
		return new_wnil();
	/* Create the node */
	dict *fdict = newdict();
	// Set file pointer this way so it becomes unreachable from ward.
	dict_update(fdict, ".file", (dict_Data *) wf); 
	dict_update(fdict, "filename", (dict_Data *) fname);
	dict_update(fdict, "openmode", (dict_Data *) omode);
	assign_file_methods(fdict, S);
	return new_wnode(fdict);
}

/* Put
 * Method to write one byte to a file on the current position: put(file, byte)
 * `file` must be a node returned by the open function and `byte` a byte.
 *
 *     file := import("file");
 *     f := file.open("file.txt", files.APPEND);
 *     
 *     f:put(64); 1
 *     f.put(f, 10); 2
 * 
 * both ways 1 and 2 are correct, but 1 is more recommended, returns the byte
 * writed itself, in case an error occurs then returns nil.
 */
W_Object *ward_file_put(stack *args, Scope *S) {
	if (args->count != 2)
		raiseError(ARGCOUNT_ERROR, "put requires 2 arguments: file, byte");
	W_Node *fnode = (W_Node *) stack_pop(args);
	if (fnode->type != W_NODE)
		raiseError(TYPE_ERROR, "filenode must be a node");
	W_Byte *byte = (W_Byte *) stack_pop(args);
	if (byte->type != W_BYTE)
		raiseError(TYPE_ERROR, "byte must be a byte");
	w_file *f = (w_file *) Wnode_Get(fnode, ".file");
	int r = fputc(byte->byte, f->file);
	if (r != EOF)
		return new_wbyte((char) r);
	else
		return new_wnil();
}

/* Puts
 * Method to write one or more bytes to a file on the current position:
 * puts(file, str) 
 * `file` must be a node returned by open and `str` the string to write.
 * 
 *     file := import("file");
 *     f := file.open("file.txt", file.WRITE);
 *     
 *     f:puts("line of text\n\n");
 *
 * returns a byte with the number of bytes written or nil if an error occurs.
 *
 * NOTE: the max lenght of `str` is 255
 */
W_Object *ward_file_puts(stack *args, Scope *S) {
	if (args->count != 2)
		raiseError(ARGCOUNT_ERROR, "puts requires 2 arguments: file, string");
	W_Node *fnode = (W_Node *) stack_pop(args);
	if (fnode->type != W_NODE)
		raiseError(TYPE_ERROR, "filenode must be a node");
	W_Node *string = (W_Node *) stack_pop(args);
	if (string->type != W_NODE)
		raiseError(TYPE_ERROR, "string must be a node");
	w_file *f = (w_file *) Wnode_Get(fnode, ".file");
	char chunk[MAXBUFFER + 1];
	W_Node *chr = string;
	int i = 0;
	int r;
	while (i <= MAXBUFFER) {
		if (chr->type == W_NIL)
			break;
		chunk[i++] = ((W_Byte *) Wnode_Get(chr, "$char"))->byte;
		chr = (W_Node *) Wnode_Get(chr, "$next");
	}
	chunk[i] = '\0';
	if (chr->type != W_NIL)
		raiseError(MEMORY_ERROR, "string length > 255");
	r = fputs(chunk, f->file);
	if (r != EOF)
		return new_wbyte((char) r);
	else
		return new_wnil();
}

/* Get
 * Method to read one byte from a file from the current position: get(file)
 * `file` must be a node returned by the function open.
 *
 *     file := import("file");
 *     f := file.open("file.txt", file.READ);
 *     
 *     f:get();
 *
 * returns the byte readed, if an error occurs then returns nil.
 */
W_Object *ward_file_get(stack *args, Scope *S) {
	W_Node *fnode = (W_Node *) stack_pop(args);
	if (!fnode)
		raiseError(ARGCOUNT_ERROR, "get requires one argument: file");
	if (fnode->type != W_NODE)
		raiseError(TYPE_ERROR, "file must be a node");
	w_file *f = (w_file *) Wnode_Get(fnode, ".file");
	int r = fgetc(f->file);
	if (r != EOF)
		return new_wbyte((char) r);
	else
		return new_wnil();
}

/* Gets 
 * Method to read one or nore bytes from a file from the current position:
 * gets(file, length)
 * `file` must be a node returned by the function open and `length` a byte.
 *
 *     file := import("file");
 *     f := file.open("file.txt", file.WRITE);
 *     
 *     f:gets(32);
 *
 * returns a string of `length` bytes or until id finds a newline character,
 * if an error occurs then returns nil.
 */
W_Object *ward_file_gets(stack *args, Scope *S) {
	W_Node *fnode = (W_Node *) stack_pop(args);
	if (!fnode)
		raiseError(ARGCOUNT_ERROR, "gets requires one argument: file");
	if (fnode->type != W_NODE)
		raiseError(TYPE_ERROR, "file must be a node");
	W_Byte *buffsize = (W_Byte *) stack_pop(args);
	if (buffsize->type != W_BYTE)
		raiseError(TYPE_ERROR, "size must be a byte");
	w_file *f = (w_file *) Wnode_Get(fnode, ".file");
	char s[MAXBUFFER+1];
	char *r = fgets(s, buffsize->byte, f->file);
	if (r != NULL)
		return new_string(s);
	else
		return new_wnil();
}

/* Seek
 * Method to move to a specific position in a file:
 * seek(file, pos)
 * `file` must be a node returnes by open, `pos` is a list with a little endian
 * unsigned int:
 * 
 *     file := import("file");
 *     f := file.open("file.txt", file.READ_PLUS);
 *
 *     ; Set position to 1000 starting from the begining
 *     f:seek([232,3,0,0], file.SET);
 *
 * returns 
 *
 */


/* Tell
 * Method to know the current position in a file: tell(file)
 * `file` must be a node returned by the open function.
 *
 *     file := import("file");
 *     f := file.open("file.txt", file.READ);
 *     
 *     f:tell();
 *
 * returns the vyte 0, if an error occurs then returns 1.
 */
W_Object *ward_file_tell(stack *args, Scope *S) {
	W_Node *fnode = (W_Node *) stack_pop(args);
	if (!fnode)
		raiseError(ARGCOUNT_ERROR, "get requires one argument: file");
	if (fnode->type != W_NODE)
		raiseError(TYPE_ERROR, "file must be a node");
	w_file *f = (w_file *) Wnode_Get(fnode, ".file");
	if (fclose(f->file) == EOF)
		return new_wbyte(1);
	else
		return new_wbyte(0);
}

/* Close
 * Method to close a file: close(file)
 * `file` must be a node returned by the open function.
 *
 *     file := import("file");
 *     f := file.open("file.txt", file.READ);
 *     
 *     f:close();
 *
 * returns the byte 0, if an error occurs then returns 1.
 */
W_Object *ward_file_close(stack *args, Scope *S) {
	W_Node *fnode = (W_Node *) stack_pop(args);
	if (!fnode)
		raiseError(ARGCOUNT_ERROR, "get requires one argument: file");
	if (fnode->type != W_NODE)
		raiseError(TYPE_ERROR, "file must be a node");
	w_file *f = (w_file *) Wnode_Get(fnode, ".file");
	if (fclose(f->file) == EOF)
		return new_wbyte(1);
	else
		return new_wbyte(0);
}

/* Remove
 * Function to delete a file: remove(filename)
 * `filename` should be a string with the path of the file.
 *
 *     file := import("file");
 *
 *     file.remove("file.txt");
 *
 * returns the byte 0, if an error occurs then returns 1.
 */
W_Object *ward_file_remove(stack *args, Scope *S) {
	W_Node *fname = (W_Node *) stack_pop(args);
	if (fname->type != W_NODE)
		raiseError(TYPE_ERROR, "filename must be a node");
	/* Get the filename */
	char sfname[129];
	int i = 0;
	W_Node *chr = fname;
	while (i <= 128) {
		if (chr->type == W_NIL)
			break;
		sfname[i++] = ((W_Byte *) Wnode_Get(chr, "$char"))->byte;
		chr = (W_Node *) Wnode_Get(chr, "$next");
	}
	sfname[i] = '\0';
	return new_wbyte(remove(sfname) * -1);
}

/* Module Loader */
W_Object *module_loader(stack *args, Scope *S) {
	while (S->upscope)
		S = S->upscope;
	Scope_Set(S, ".file-get", (Scope_Object *) new_cfunction(ward_file_get));
	Scope_Set(S, ".file-gets", (Scope_Object *) new_cfunction(ward_file_gets));
	Scope_Set(S, ".file-put", (Scope_Object *) new_cfunction(ward_file_put));
	Scope_Set(S, ".file-puts", (Scope_Object *) new_cfunction(ward_file_puts));
	Scope_Set(S, ".file-close", (Scope_Object *) new_cfunction(ward_file_close));
	// Creating the module node
	dict *d = newdict();
	// STDIO Files
	dict *fdict = newdict();
	dict_update(fdict, ".file", (dict_Data *) new_prefile(stdin,0));//stdin 
	dict_update(fdict, "openmode", (dict_Data *) new_wbyte(0));
	dict_update(fdict, "filename", (dict_Data *) new_wnil());
	assign_file_methods(fdict, S);
	dict_update(d, "stdin", (dict_Data *) new_wnode(fdict));
	fdict = newdict();
	dict_update(fdict, ".file", (dict_Data *) new_prefile(stdout,0));//stdout
	dict_update(fdict, "openmode", (dict_Data *) new_wbyte(0));
	dict_update(fdict, "filename", (dict_Data *) new_wnil());
	assign_file_methods(fdict, S);
	dict_update(d, "stdout", (dict_Data *) new_wnode(fdict));
	fdict = newdict();
	dict_update(fdict, ".file", (dict_Data *) new_prefile(stderr,0));//stderr
	dict_update(fdict, "openmode", (dict_Data *) new_wbyte(0));
	dict_update(fdict, "filename", (dict_Data *) new_wnil());
	assign_file_methods(fdict, S);
	dict_update(d, "stderr", (dict_Data *) new_wnode(fdict));
	// Variables
	dict_update(d, "READ", (dict_Data *) new_wbyte(READ));
	dict_update(d, "WRITE", (dict_Data *) new_wbyte(WRITE));
	dict_update(d, "APPEND", (dict_Data *) new_wbyte(APPEND));
	dict_update(d, "READ_PLUS", (dict_Data *) new_wbyte(READ_PLUS));
	dict_update(d, "WRITE_PLUS", (dict_Data *) new_wbyte(WRITE_PLUS));
	dict_update(d, "APPEND_PLUS", (dict_Data *) new_wbyte(APPEND_PLUS));
	dict_update(d, "SET", (dict_Data *) new_wbyte(SET));
	dict_update(d, "CUR", (dict_Data *) new_wbyte(CUR));
	dict_update(d, "END", (dict_Data *) new_wbyte(END));
	// Functions
	dict_update(d, "open", (dict_Data *) new_cfunction(ward_file_open));
	dict_update(d, "remove", (dict_Data *) new_cfunction(ward_file_remove));
	return new_wnode(d);
}

