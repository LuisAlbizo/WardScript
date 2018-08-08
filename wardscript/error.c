/* Error Handling implementation file: error.c
 * Luis Albizo "07-25-18"
 */

#include "error.h"
#include <stdio.h>
#include <stdlib.h>

void raiseError(int errorcode, char *message) {
	char *errortype;
	switch (errorcode) {
		case MEMORY_ERROR:
			errortype = "Memory Error";
			break;
		case UNDECLARED_ERROR:
			errortype = "Undeclared Error";
			break;
		case OPERATOR_ERROR:
			errortype = "Operator Error";
			break;
		case SYNTAX_ERROR:
			errortype = "Syntax Error";
			break;
		case INVALID_BYTE_ERROR:
			errortype = "Invalid Byte Error";
			break;
		case UNCALLABLE_ERROR:
			errortype = "Uncallable Expression Error";
			break;
		case ARGCOUNT_ERROR:
			errortype = "Argument Count Error";
			break;
		case TYPE_ERROR:
			errortype = "Type Error";
			break;
		case LOOP_ERROR:
			errortype = "Loop Error";
			break;
		default:
			errortype = "Unknown Error";
			break;
	}
	fprintf(stderr, "%s: %s\n", errortype, message);
	exit(errorcode);
}

