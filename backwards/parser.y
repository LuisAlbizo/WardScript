/* Bison Parser file: parser.y
 * Luis Albizo "07-25-18"
 */
%{
#include "ast.h"
#include "object.h"
#include "stack.h"
#include "dict.h"
#include "bward.h"
#include <string.h>
#include <stdio.h>

st_block *program;

%}

%union {
	st_st *stat;
	st_block *block;
	stack *sstack;
}

%token EQ
%token DOT COMMA COLON
%token LPARENT RPARENT LBRACKET RBRACKET LBRACE RBRACE
%token IF THEN ELSE END FOREVER EXIT FUNCTION RETURN
%token <stat> NAME NUMBER

%type <stat> expression assignment statement
%type <block> block program
%type <sstack> args names

/* Precedence Rules
*/
%right DOT EQ COLON
%right 'A' 'O' /* && || ... Logical Operators */
%right 'g' 'G' 'l' 'L' 'e' 'n' /* > >= < <= == =/ ... Comparission Operands */
%right '+' '-' /* + - ... Aritmethic Low Precedence */
%right '*' '/' '%' /* * / % ... Aritmethic High Precedence */
%right '^' /* Exponential Higher Precedence */
%nonassoc '!' /* Unary Operators non-associative */

%start program

%%

program: block { program = $1;};

block: { $$ = NULL; printf("null block\n"); }
     | statement
     {
	printf("stat block\n");
     	stack *sctk = newstack();
	stack_push(sctk, (stack_Data *) $1);
     	$$ = (st_block *) new_block(sctk);
     }
     | block statement
     {
     	printf("block\n");
     	$$ = $1;
	stack_push($$->block, (stack_Data *) $2);
     }
     ;

statement: expression { $$ = $1; printf("stat expr\n"); }
	 | END block THEN expression IF
	 {
	 	printf("if 1\n");
	 	$$ = new_if($4, $2, NULL);
	 }
	 | END block ELSE block THEN expression IF
	 {
	 	printf("if 2\n");
	 	$$ = new_if($6, $4, $2);
	 }
	 | END block FOREVER
	 {
	 	printf("forever\n");
	 	$$ = new_forever($2);
	 }
	 | assignment { $$ = $1; printf("assignment\n"); }
	 | expression EQ NAME DOT LBRACKET expression RBRACKET
	 {
	 	printf("member assignment\n");
	  	st_member *member = (st_member *) new_member($6, ((st_name *) $3)->name);
		free($3);
		$$ = new_member_assign(member, $1);
	 }
	 | EXIT
	 {
	 	printf("exit\n");
	 	$$ = new_st();
		$$->type = AST_EXIT;
	 }
	 ;

assignment: expression EQ NAME
	  {
	 	printf("assignment 1\n");
	  	char name[MAX_DICT_KEY];
		strncpy(name, ((st_name *) $3)->name, strlen(((st_name *) $3)->name));
		assign *a = new_assign(name, $1);
		free_st($3);
		stack *s = newstack();
		stack_push(s, (stack_Data *) a);
		$$ = new_assignment(s);
	  }
	  | assignment COMMA expression EQ NAME
	  {
	 	printf("assignment 2\n");
		$$ = $1;
	  	char name[MAX_DICT_KEY];
		strncpy(name, ((st_name *) $5)->name, strlen(((st_name *) $5)->name));
		assign *a = new_assign(name, $3);
		free_st($3);
		free_st($5);
		stack_push(((st_assignment *) $$)->assigns, (stack_Data *) a);
	  }
	  ;

expression: NUMBER { $$ = $1; printf("number\n"); }
	  | NAME { $$ = $1; printf("name\n"); }
	  | LPARENT args RPARENT LBRACKET expression RBRACKET
	  {
	  	printf("call\n");
	  	$$ = new_call($5, $2);
	  }
	  | NAME DOT LBRACKET expression RBRACKET
	  {
	 	printf("member\n");
	  	$$ = new_member($4, ((st_name *) $1)->name);
		free($1);
	  }
	  | LPARENT args RPARENT NAME COLON LBRACKET expression RBRACKET
	  {
	 	printf("methodcall\n");
	  	$$ = new_methodcall($7, ((st_name *) $4)->name, $2);
		free($4);
	  }
	  | END block COLON names FUNCTION NAME RETURN
	  {
	 	printf("function\n");
	  	$$ = new_object(new_bfunction(((st_name *) $6)->name, $4, ((st_block *) $2)->block));
		free($2);
		free($6);
	  }
	  | LBRACE assignment RBRACE
	  {
	 	printf("node\n");
	  	// Node
		dict *members = newdict(); // create the member's dict for the node
		assign *member = (assign *) stack_pop(((st_assignment *) $2)->assigns); // get an assignment
		while (member) { // while the stack still have assignments left
			dict_update(members, member->name, (dict_Data *) member->value);
			free(member); //  free the memory because is not needed
			member = (assign *) stack_pop(((st_assignment *) $2)->assigns); // get the next assignment
		}
		$$ = new_object(new_bnode(members));
	  }
	  ;

expression: expression '+' expression
	  {
	  	$$ = new_bop('+', $3, $1);
	  }
	  | expression '-' expression
	  {
	  	$$ = new_bop('-', $3, $1);
	  }
	  | expression '*' expression
	  {
	  	$$ = new_bop('*', $3, $1);
	  }
	  | expression '/' expression
	  {
	  	$$ = new_bop('/', $3, $1);
	  }
	  | expression '%' expression
	  {
	  	$$ = new_bop('%', $3, $1);
	  }
	  | expression '^' expression
	  {
	  	$$ = new_bop('^', $3, $1);
	  }
	  | expression 'g' expression
	  {
	  	$$ = new_bop('g', $3, $1);
	  }
	  | expression 'G' expression
	  {
	  	$$ = new_bop('G', $3, $1);
	  }
	  | expression 'l' expression
	  {
	  	$$ = new_bop('l', $3, $1);
	  }
	  | expression 'L' expression
	  {
	  	$$ = new_bop('L', $3, $1);
	  }
	  | expression 'e' expression
	  {
	  	$$ = new_bop('e', $3, $1);
	  }
	  | expression 'n' expression
	  {
	  	$$ = new_bop('n', $3, $1);
	  }
	  | expression '!' 
	  {
	  	$$ = new_uop('!', $1);
	  }
	  ;

args: { $$ = newstack(); printf("empty args\n"); }
    | expression
    {
	printf("args 1\n");
    	$$ = newstack();
	stack_push($$, (stack_Data *) $1);
    }
    | expression COMMA args
    {
	printf("args 2\n");
	$$ = $3;
	stack_push($$, (stack_Data *) $1);
    }
    ;

names: { $$ = newstack(); printf("empty names\n"); }
     | NAME
     {
	printf("names 1\n");
	$$ = newstack();
	stack_push($$, (stack_Data *) $1);
     }
     | NAME COMMA names
     {
	printf("names 2\n");
     	$$ = $3;
	stack_push($$, (stack_Data *) $1);
     }
     ;

%%


