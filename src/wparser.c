/* Parser file: wparser.c
 * Luis Albizo "08-16-20"
 */
#include "ast.h"
#include "object.h"
#include "stack.h"
#include "dict.h"
#include "ward.h"
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

union st_type {
	st_st *stat;
	st_block *block;
	stack *sstack;
};
typedef union st_type st;

enum yytokentype {
    EQ = 258,                      /* EQ */
    DOT = 259,                     /* DOT */
    COMMA = 260,                   /* COMMA */
    COLON = 261,                   /* COLON */
    SEMICOLON = 262,               /* SEMICOLON */
    LPARENT = 263,                 /* LPARENT */
    RPARENT = 264,                 /* RPARENT */
    LBRACKET = 265,                /* LBRACKET */
    RBRACKET = 266,                /* RBRACKET */
    LBRACE = 267,                  /* LBRACE */
    RBRACE = 268,                  /* RBRACE */
    IF = 269,                      /* IF */
    THEN = 270,                    /* THEN */
    ELSE = 271,                    /* ELSE */
    END = 272,                     /* END */
    LOOP = 273,                    /* LOOP */
    EXIT = 274,                    /* EXIT */
    FUNCTION = 275,                /* FUNCTION */
    NONLOCAL = 276,                /* NONLOCAL */
    NAME = 277,                    /* NAME */
    NUMBER = 278,                  /* NUMBER */
    STRING = 279                   /* STRING */
};
typedef enum yytokentype token_t;

/* Symbol kind.  */
enum symbol_kind_t {
    SYMBOL_EQ = 3,                         /* EQ  */
    SYMBOL_DOT = 4,                        /* DOT  */
    SYMBOL_COMMA = 5,                      /* COMMA  */
    SYMBOL_COLON = 6,                      /* COLON  */
    SYMBOL_SEMICOLON = 7,                  /* SEMICOLON  */
    SYMBOL_LPARENT = 8,                    /* LPARENT  */
    SYMBOL_RPARENT = 9,                    /* RPARENT  */
    SYMBOL_LBRACKET = 10,                  /* LBRACKET  */
    SYMBOL_RBRACKET = 11,                  /* RBRACKET  */
    SYMBOL_LBRACE = 12,                    /* LBRACE  */
    SYMBOL_RBRACE = 13,                    /* RBRACE  */
    SYMBOL_IF = 14,                        /* IF  */
    SYMBOL_THEN = 15,                      /* THEN  */
    SYMBOL_ELSE = 16,                      /* ELSE  */
    SYMBOL_END = 17,                       /* END  */
    SYMBOL_FOREVER = 18,                   /* FOREVER  */
    SYMBOL_EXIT = 19,                      /* EXIT  */
    SYMBOL_FUNCTION = 20,                  /* FUNCTION  */
    SYMBOL_NONLOCAL = 21,                  /* NONLOCAL  */
    SYMBOL_NAME = 22,                      /* NAME  */
    SYMBOL_NUMBER = 23,                    /* NUMBER  */
    SYMBOL_STRING = 24,                    /* STRING  */
    SYMBOL_25_A_ = 25,                     /* 'A'  */
    SYMBOL_26_O_ = 26,                     /* 'O'  */
    SYMBOL_27_g_ = 27,                     /* 'g'  */
    SYMBOL_28_G_ = 28,                     /* 'G'  */
    SYMBOL_29_l_ = 29,                     /* 'l'  */
    SYMBOL_30_L_ = 30,                     /* 'L'  */
    SYMBOL_31_e_ = 31,                     /* 'e'  */
    SYMBOL_32_n_ = 32,                     /* 'n'  */
    SYMBOL_33_ = 33,                       /* '+'  */
    SYMBOL_34_ = 34,                       /* '-'  */
    SYMBOL_35_ = 35,                       /* '<'  */
    SYMBOL_36_ = 36,                       /* '>'  */
    SYMBOL_37_ = 37,                       /* '!'  */
    SYMBOL_38_ = 38,                       /* '#'  */
    SYMBOL_program = 40,                   /* program  */
    SYMBOL_block = 41,                     /* block  */
    SYMBOL_statement = 42,                 /* statement  */
    SYMBOL_assignment = 43,                /* assignment  */
    SYMBOL_expression = 44,                /* expression  */
    SYMBOL_args = 45,                      /* args  */
    SYMBOL_names = 46                      /* names  */
};
typedef enum symbol_kind_t symbol_t;


