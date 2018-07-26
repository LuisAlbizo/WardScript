/* Bison Parser file: parser.y 
 * Luis Albizo "07-25-18"
 */
%{
#include "ast.h"
#include "stack.h"
#include <string.h>

%}

%union {
	st_st *stat;
	st_block *block;
	stack *sstack;
}

%token EQ
%token DOT COMMA COLON SEMICOLON
%token LPARENT RPARENT LBRACKET RBRACKET LBRACE RBRACE
%token IF THEN ELSE END FUNCTION FOREVER EXIT RETURN
%token <stat> NAME NUMBER

%type <stat> expression assignment statement
%type <block> block program
%type <sstack> args names

/* Precedence Rules
*/
%right 'A' 'O' /* && || ... Logical Operators */
%right 'g' 'G' 'l' 'L' 'e' 'n' /* > >= < <= == =/ ... Comparission Operands */
%right '+' '-' /* + - ... Aritmethic Low Precedence */
%right '*' '/' '%' /* * / % ... Aritmethic High Precedence */
%right '^' /* Exponential Higher Precedence */
%nonassoc UMINUS '!' /* Unary Operators non-associative */

%start program

%%

program: block {$$ = $1;};

block: { $$ = NULL; }
     | statement
     {
     	stack *sctk = new_stack();
	stack_push(sctk, (stack_Data *) $1);
     	$$ = new_block(sctk);
     }
     | block statement
     {
     	$$ = $1;
	stack_push($$->block, (stack_Data *) $2);
     }
     ;

statement: expression { $$ = $1; }
	 | SEMICOLON { $$ = NULL; }
	 | END block THEN expression IF
	 {
	 	$$ = new_if($4, $2, NULL);
	 }
	 | END block ELSE block THEN expression IF
	 {
	 	$$ = new_if($6, $4, $2);
	 }
	 | END block FOREVER
	 {
	 	$$ = new_forever($2);
	 }
	 | assignment { $$ = $1; }
	 | EXIT
	 {
	 	$$ = new_st();
		$$->type = AST_EXIT;
	 }
	 ;

assignment: expression EQ NAME
	  {
	  	char *name;
		strncpy(name, $3->name, strlen($3->name));
		assign *a = new_assign(name, $1);
		free_st($3);
		stack *s = new_stack();
		stack_push(s, a);
		$$ = new_assignment(s);
	  }
	  | assignment COMMA expression EQ NAME
	  {
		$$ = $1;
	  	char *name;
		strncpy(name, $5->name, strlen($5->name));
		assign *a = new_assign(name, $3);
		free_st($3);
		stack_push($$->assigns, a);
	  }
	  ;

expression: NUMBER
	  | NAME
	  | LPARENT args RPARENT expression
	  {
	  	$$ = new_call($4, $1);
	  }
	  | NAME DOT LBRACKET expr RBRACKET
	  | LPARENT args RPARENT NAME COLON LBRACKET expr RBRACKET
	  | END block COLON names FUNCTION NAME RETURN
	  | node
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
	  | expression '-' %prec UMINUS
	  {
	  	$$ = new_uop('-', $1);
	  }
	  | expression '!' 
	  {
	  	$$ = new_uop('!', $1);
	  }
	  ;

args: { $$ = NULL; }
    | expression
    {
    	stack *stck = new_stack();
	$$ = stck;
	stack_push($$, (stack_Data *) $1);
    }
    | expression COMMA args
    {
	$$ = $3;
	stack_push($$, (stack_Data *) $1);
    }
    ;

names: {  }
     | NAME
     {
     }
     | NAME COMMA names
     {
     }
     ;

node: LBRACE assignment RBRACE
    {
    };

%%




