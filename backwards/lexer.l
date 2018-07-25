%{
#include "ast.h"
#include "parser.tab.h"

int lineno = 0;

%}

%%

=: 	return EQ;
\.	return DOT;
,	return COMMA;
:	return COLON;
;	return SEMICOLON;
\?[^?]*\?	;

\+|-|\*|\/|\^|%|&&|\|\||==|=\/|<|<=|>|>=	return BOP;
!|\#	return UOP;

\(	return LPARENT;
\)	return RPARENT;
"["	return LBRACKET;
\]	return RBRACKET;
\{	return LBRACE;
\}	return RBRACE;

[0-9]+(\.[0-9]+)?	return NUMBER;
[a-zA-Z_][a-zA-Z_0-9]*	{
	if (strcmp(yytext,"fi") == 0) return IF;
	else if (strcmp(yytext, "esle") == 0) return ELSE;
	else if (strcmp(yytext, "neht") == 0) return THEN;
	else if (strcmp(yytext, "dne") == 0) return END;
	else if (strcmp(yytext, "noitcnuf") == 0) return FUNCTION;
	else if (strcmp(yytext, "reverof") == 0) return FOREVER;
	else if (strcmp(yytext, "tixe") == 0) return EXIT;
	else if (strcmp(yytext, "nruter") == 0) return RETURN;
	else return NAME;
}
\"[^"]*\"	return STRING;

"\n"	lineno++;
[ \t]+	;

%%





