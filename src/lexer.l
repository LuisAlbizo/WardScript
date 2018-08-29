%{
#include "ast.h"
#include "object.h"
#include "parser.tab.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int lineno = 0;

%}

%%

:= 	return EQ;
\.	return DOT;
,	return COMMA;
:	return COLON;
;	return SEMICOLON;
\?[^?]*\?	;
\".*\"	{
	char *str = calloc(strlen(yytext) - 1, sizeof(char));
	if (!str)
		raiseError(MEMORY_ERROR, "can't create new string");
	int i;
	for (i = 1; yytext[i] != '"'; i++) {
		str[i-1] = yytext[i];
	}
	str[i] = '\0';
	yylval.stat = new_object(new_string(str));
	return STRING;
}

"+"	return '+';
"-"	return '-';
"<<"	return '<';
">>"	return '>';
"&&"	return 'A';
"||"	return 'O';
"=="	return 'e';
"/="	return 'n';
"<"	return 'l';
"<="	return 'L';
">"	return 'g';
">="	return 'G';
"!"	return '!';
"#"	return '#';

\(	return LPARENT;
\)	return RPARENT;
"["	return LBRACKET;
\]	return RBRACKET;
\{	return LBRACE;
\}	return RBRACE;

[0-9]+	{
	unsigned int n = atoi(yytext);
	if (n >= 256)
		raiseError(INVALID_BYTE_ERROR, "byte value greater than 255");
	yylval.stat = new_object(new_bbyte(atoi(yytext)));
	return NUMBER;
}

['$a-zA-Z_]['$a-zA-Z_0-9]*	{
	if (strcmp(yytext, "if") == 0) return IF;
	else if (strcmp(yytext, "else") == 0) return ELSE;
	else if (strcmp(yytext, "then") == 0) return THEN;
	else if (strcmp(yytext, "nonlocal") == 0) return NONLOCAL;
	else if (strcmp(yytext, "end") == 0) return END;
	else if (strcmp(yytext, "func") == 0) return FUNCTION;
	else if (strcmp(yytext, "loop") == 0) return FOREVER;
	else if (strcmp(yytext, "exit") == 0) return EXIT;
	else {
		yylval.stat = new_name(yytext);
		return NAME;
	}
}

"\n"	lineno++;
[ \t]+	;
. raiseError(SYNTAX_ERROR, "Invalid Token");

%%





