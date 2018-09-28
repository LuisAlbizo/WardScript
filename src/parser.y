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
#include <stdlib.h>

FILE *yyin = NULL;
extern int yylex();
int lineno = 0;

st_block *program;

void yyerror (char const *s) {
	fprintf(stderr, "error:- %s\n", s);
}

%}

%union {
	st_st *stat;
	st_block *block;
	stack *sstack;
}

%token EQ
%token DOT COMMA COLON SEMICOLON
%token LPARENT RPARENT LBRACKET RBRACKET LBRACE RBRACE
%token IF THEN ELSE END FOREVER EXIT FUNCTION NONLOCAL
%token <stat> NAME NUMBER STRING

%type <stat> expression assignment statement
%type <block> block program
%type <sstack> args names

/* Precedence Rules
*/
%left 'A' 'O' /* && || ... Logical Operators */
%left 'g' 'G' 'l' 'L' 'e' 'n' /* > >= < <= == /= ... Comparission Operands */
%left '+' '-' /* + - ... Aritmethic High Precedence */
%left '<' '>' /* << >> ... Aritmethic Low Precedence */
%nonassoc '!' '#' /* Unary Operators non-associative */
%nonassoc LPARENT DOT COLON

%start program

%%

program: block { program = $1;};

block: { $$ = (st_block *) new_block(newstack()); }
     | statement block
     {
     	$$ = $2;
	stack_push($$->block, (stack_Data *) $1);
     }
     ;

statement: SEMICOLON
	 { 
	 	$$ = new_st();
		$$->type = SEMICOLON;
	 }
	 | expression SEMICOLON { $$ = $1; }
	 | IF expression THEN block END
	 {
	 	$$ = new_if($2, $4, (st_block *) new_block(newstack()));
	 }
	 | IF expression THEN block ELSE block END
	 {
	 	$$ = new_if($2, $4, $6);
	 }
	 | IF expression THEN block ELSE '-' statement
	 {
		st_block *block = (st_block *) new_block(newstack());
		stack_push(block->block, (stack_Data *) $7);
		$$ = new_if($2, $4, block);
	 }
	 | FOREVER block END
	 {
	 	$$ = new_forever($2);
	 }
	 | assignment SEMICOLON { $$ = $1; }
	 | NONLOCAL assignment SEMICOLON
	 {
	 	$$ = $2;
		$$->type = AST_NL_ASSIGN;
	 }
	 | expression DOT NAME EQ expression SEMICOLON
	 {
	  	st_member *member = (st_member *) new_member($1, ((st_name *) $3)->name);
		free($3);
		$$ = new_member_assign(member, $5);
	 }
	 | EXIT SEMICOLON
	 {
	 	$$ = new_st();
		$$->type = AST_EXIT;
	 }
	 ;

assignment: NAME EQ expression
	  {
	  	char name[MAX_DICT_KEY];
		strncpy(name, ((st_name *) $1)->name, MAX_DICT_KEY);
		assign *a = new_assign(name, $3);
		free_st($1);
		stack *s = newstack();
		stack_push(s, (stack_Data *) a);
		$$ = new_assignment(s);
	  }
	  | NAME EQ expression COMMA assignment
	  {
		$$ = $5;
	  	char name[MAX_DICT_KEY];
		strncpy(name, ((st_name *) $1)->name, MAX_DICT_KEY);
		assign *a = new_assign(name, $3);
		stack_push(((st_assignment *) $$)->assigns, (stack_Data *) a);
	  }
	  ;

expression: NUMBER { $$ = $1; }
	  | NAME { $$ = $1; }
	  | NONLOCAL NAME
	  {
	  	$$ = $2;
		$$->type = AST_NL_NAME;
	  }
	  | STRING { $$ = $1; }
	  | LPARENT expression RPARENT { $$ = $2; }
	  | expression LPARENT args RPARENT
	  {
	  	$$ = new_call($1, $3);
	  }
	  | expression DOT NAME
	  {
	  	$$ = new_member($1, ((st_name *) $3)->name);
		free($3);
	  }
	  | expression COLON NAME LPARENT args RPARENT
	  {
	  	$$ = new_methodcall($1, ((st_name *) $3)->name, $5);
		free($3);
	  }
	  | FUNCTION names COLON NAME COLON block END
	  {
	  	$$ = new_function_construct(((st_name *) $4)->name, $2, ((st_block *) $6)->block);
	  	free($4);
	  }
	  | FUNCTION LBRACE names COLON expression RBRACE
	  {
	  	/* lambda-like syntactic sugar */
		stack *block = newstack();
		assign *assi = new_assign(".return", $5);
		stack *assis = newstack();
		stack_push(assis, (stack_Data *) assi);
		st_st *aux = new_assignment(assis);
		stack_push(block, (stack_Data *) aux);
	  	$$ = new_function_construct(".return", $3, block);
	  }
	  | LBRACE assignment RBRACE
	  {
	  	// Node
		$$ = new_node_construct((st_assignment *) $2);
	  }
	  | LBRACKET args RBRACKET
	  {
	  	// List
		$$ = new_list_construct($2);
	  }
	  ;

expression: expression '+' expression { $$ = new_bop('+', $1, $3); }
	  | expression '-' expression { $$ = new_bop('-', $1, $3); }
	  | expression '<' expression { $$ = new_bop('<', $1, $3); }
	  | expression '>' expression { $$ = new_bop('>', $1, $3); }
	  | expression 'g' expression { $$ = new_bop('g', $1, $3); }
	  | expression 'G' expression { $$ = new_bop('G', $1, $3); }
	  | expression 'l' expression { $$ = new_bop('l', $1, $3); }
	  | expression 'L' expression { $$ = new_bop('L', $1, $3); }
	  | expression 'e' expression { $$ = new_bop('e', $1, $3); }
	  | expression 'n' expression { $$ = new_bop('n', $1, $3); }
	  | expression 'A' expression { $$ = new_bop('A', $1, $3); }
	  | expression 'O' expression { $$ = new_bop('O', $1, $3); }
	  | '!' expression { $$ = new_uop('!', $2); }
	  | '#' expression { $$ = new_uop('#', $2); }
	  ;

args: { $$ = newstack(); }
    | expression
    {
    	$$ = newstack();
	stack_push($$, (stack_Data *) $1);
    }
    | expression COMMA args
    {
	$$ = $3;
	stack_push($$, (stack_Data *) $1);
    }
    ;

names: { $$ = newstack(); }
     | NAME
     {
	$$ = newstack();
	stack_push($$, (stack_Data *) $1);
     }
     | NAME COMMA names
     {
     	$$ = $3;
	stack_push($$, (stack_Data *) $1);
     }
     ;

%%

