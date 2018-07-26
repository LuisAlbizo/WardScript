/* Error Handling implementation file: error.c
 * Luis Albizo "07-25-18"
 */

#include "error.h"
#include <stdio.h>
#include <stdlib.h>

void raiseError(int errorcode, char *message, void(*callback)()) {
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
		default:
			errortype = "Unknown Error";
			break;
	}
	fprintf(stderr, "%s: %s\n", errortype, message);
	if (callback)
		(*callback)();
	exit(errorcode);
}

