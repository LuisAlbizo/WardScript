/* Error Handling definition file: error.h
 * Luis Albizo "07-25-18"
 */
#ifndef error_h
#define error_h

#define MEMORY_ERROR		1
#define UNDECLARED_ERROR	2
#define OPERATOR_ERROR		3
#define SYNTAX_ERROR		4
#define UNCALLABLE_ERROR	5
#define ARGCOUNT_ERROR		6

void raiseError(int, char *); /* int is the error code, is what the program returns to the console
					    at the exit

					    char * is the error message to show in screen

					    void(*)() is a function callback todo before the program ends
					    */

#endif
