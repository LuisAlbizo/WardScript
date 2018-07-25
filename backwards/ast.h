/* Syntax Tree Data Structure definition file: ast.h
 * Luis Albizo "07-20-18"
 */
#ifndef ast_h
#define ast_h

#include <stdlib.h>
#include "ast.h"

	/* AST Nodetypes */

#define AST_BLOCK	1
#define AST_ASSIGNMENT	2
#define AST_BOP		3
#define AST_UOP		4
#define AST_CALL	5
#define AST_METHODCALL	6
#define AST_IF		7
#define AST_FOREVER	8
#define AST_COMMENT	9
#define AST_ARGS	10
#define AST_ATTR	11
#define AST_INDEX	12
#define AST_NAMES	13
#define AST_EXPRESSION	14

	/* Datatypes */

#define AST_NUMBER	20
#define AST_STRING	21
#define AST_LIST	22
#define AST_DICT	23
#define AST_FUNCTION	24

/*
** AST struct definition for each type of statement
*/

struct st {
	unsigned int nodetype;
};

struct assign {
	char *name;
	struct expression *value;
}:

struct assignment {
	unsigned int nodetype;

};

struct bop {
	unsigned int nodetype;
	char op;
	struct expression *left;
	struct expression *right;
};

struct uop {
	unsigned int nodetype;
	char op;
	struct expression *right;
};

typedef struct st st_st;
typedef struct assignment st_assignment;
typedef struct bop st_bop;
typedef struct uop st_uop;







#endif
