 /* Bison Parser */
%{
#include "ast.h"
%}

%union {
	st_st *stat;
}

%token EQ BOP UOP
%token DOT COMMA COLON SEMICOLON
%token LPARENT RPARENT LBRACKET RBRACKET LBRACE RBRACE
%token IF THEN ELSE END FUNCTION FOREVER EXIT RETURN
%token <stat> STRING NAME NUMBER

%type <stat> expression assignment statement block args names list dict

/* Precedence Rules 
*/
%right 'A' 'O' /* && || ... Logical Operators */
%right 'g' 'G' 'l' 'L' 'e' 'n' /* > >= < <= == =/ ... Comparission Operands */
%right '+' '-' /* + - Aritmethic */
%right '*' '/' '%' /* * / % */
%right '^' /* Exponential */
%nonassoc 'm' '#' '!' /* Unary Operators non-associstive */

%start program

%%

program: block;

block: { $$; }
     | statement
     | block statement
     ;

/* Expressions */
expression: NUMBER
	  | NAME
	  | STRING
	  | UOP expression
	  | LBRACKET expression RBRACKET expression
	  | LPARENT args RPARENT expression
	  ;

expression: expression '+' expression
	  | expression '-' expression
	  | expression '*' expression
	  | expression '/' expression
	  | expression '%' expression
	  | expression '^' expression
	  | expression 'g' expression
	  | expression 'G' expression
	  | expression 'l' expression
	  | expression 'L' expression
	  | expression 'e' expression
	  | expression 'n' expression
	  ;

args: 
    | expression
    | expression COMMA args
    ;


%%

int main(int argc, char **argv) {
	yyparse();
}

int yyerror(char *s) {
	return 0;
}

