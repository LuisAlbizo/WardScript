/* Lexical Analyser for WardScript
 * Luis Albizo 27-09-18
 */

#include "ast.h"
#include "object.h"
#include "parser.tab.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//#include <math.h>

#define valid_name_chars "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_'$\x00"
#define valid_number_chars "0123456789\x00"
#define valid_hex_chars "abcdefABCDEF\x00"

extern FILE *yyin;
extern int lineno;
extern union YYSTYPE yylval;

int hex_to_dec(const char *hex) {
	unsigned int dec = 0, p = 0, aux;
	for (int i = strlen(hex) - 1; i >= 0; i--) {
		switch (hex[i]) {
			case 'f':
			case 'F':
				dec = dec + (15 * _pow(16, p));
				break;
			case 'e':
			case 'E':
				dec = dec + (14 * _pow(16, p));
				break;
			case 'd':
			case 'D':
				dec = dec + (13 * _pow(16, p));
				break;
			case 'c':
			case 'C':
				dec = dec + (12 * _pow(16, p));
				break;
			case 'b':
			case 'B':
				dec = dec + (11 * _pow(16, p));
				break;
			case 'a':
			case 'A':
				dec = dec + (10 * _pow(16, p));
				break;
			case '0':
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				aux = hex[i] - '0';
				dec = dec + (aux * _pow(16, p));
				break;
			default:
				raiseError(SYNTAX_ERROR, "Invalid hex digit on escape");
				break;
		}
		p++;
	}
	return dec;
}

// Valid Character Functions

int is_nameinit(int c) {
	for (int i = 0; valid_name_chars[i]; i++)
		if (c == valid_name_chars[i])
			return 1;
	return 0;
}

int is_valid_number(int c) {
	for (int i = 0; valid_number_chars[i]; i++)
		if (c == valid_number_chars[i])
			return 1;
	return 0;
}

int is_valid_name(int c) {
	for (int i = 0; valid_name_chars[i]; i++)
		if (c == valid_name_chars[i])
			return 1;
	return is_valid_number(c);
}

int is_valid_hex(int c) {
	for (int i = 0; valid_hex_chars[i]; i++)
		if (c == valid_hex_chars[i])
			return 1;
	return 0;
}

// Scan Functions

void wscan_comment(FILE *input) {
	int t = fgetc(input);
	while (1) {
		if (t == '?')
			break;
		else if (t == EOF)
			raiseError(SYNTAX_ERROR, "EOF while scanning a comment");
		else
			t = fgetc(input);
	}
}

int wscan_escape(FILE *input) {
	int t = fgetc(input);
	char seq[4] = "";
	switch (t) {
		case 'n':
			return '\n';
		case 't':
			return '\t';
		case 'v':
			return '\v';
		case 'b':
			return '\b';
		case 'r':
			return '\r';
		case 'a':
			return '\a';
		case 'f':
			return '\f';
		case '"':
			return '"';
		case '\\':
			return '\\';
		case 'x':
			// Hexadecimal Char
			fread(seq, 1, 2, input);
			if (strlen(seq) < 2)
				raiseError(SYNTAX_ERROR,"EOF while scanning hexadecimal escape");
			else
				return hex_to_dec(seq);
		case '0':
		case '1':
		case '2':
			// Decimal Char
			fseek(input, -1, SEEK_CUR);
			fread(seq, 1, 3, input);
			if (strlen(seq) < 3)
				raiseError(SYNTAX_ERROR,"EOF while scanning decimal escape");
			else
				return atoi(seq);
		case '\n':
			lineno++;
			return -2;
		case '\r':
			lineno++;
			return -2;
		default:
			raiseError(SYNTAX_ERROR, "Invalid Escape Character");
			break;
	}
	return -1;
}

void wscan_string(FILE *input) {
	//t is the next character
	//len is the lenght of the string scanned, it is useful for
	//adding a character (\0) meaning the end of the string
	int t = fgetc(input);
	unsigned int len = 0, i = -1;
	char *str = malloc(129);
	while (1) {
		if (i == 128) {
			i = 0;
			str = realloc(str, len + 128);
		}
		if (t == '"') break;
		else if (t == '\\') {
			t = wscan_escape(input);
			if (t == -2)
				i--, len--; 
			else
				str[len] = t;
		}
		else if (t == EOF)
			raiseError(SYNTAX_ERROR, "EOF while scanning a string");
		else if (t == '\n')
			raiseError(SYNTAX_ERROR, "EOL while scanning a string");
		else
			str[len] = t;
		t = fgetc(input);
		i++; len++;
	}
	str[len] = '\0';
	yylval.stat = new_object(new_string(str));
	free(str);
}

int wscan_name(FILE *input, char init) {
	char name[MAX_DICT_KEY] = "";
	int i = 1, t = fgetc(yyin);
	name[0] = init;
	while (1) {
		if (i == MAX_DICT_KEY)
			raiseError(SYNTAX_ERROR, "Name too long");
		if (is_valid_name(t))
			name[i] = t;
		else
			break;
		i++;
		t = fgetc(input);
	}
	fseek(input, -1, SEEK_CUR);
	if (strcmp(name, "if") == 0) return IF;
	else if (strcmp(name, "else") == 0) return ELSE;
	else if (strcmp(name, "then") == 0) return THEN;
	else if (strcmp(name, "nonlocal") == 0) return NONLOCAL;
	else if (strcmp(name, "end") == 0) return END;
	else if (strcmp(name, "func") == 0) return FUNCTION;
	else if (strcmp(name, "loop") == 0) return FOREVER;
	else if (strcmp(name, "exit") == 0) return EXIT;
	else {
		yylval.stat = new_name(name);
		return NAME;
	}
}

void wscan_number(FILE *input) {
	fseek(input, -1, SEEK_CUR);
	int result;
	fscanf(input, "%3d", &result);
	if (result > 255)
		raiseError(SYNTAX_ERROR, "Number greater than 255");
	yylval.stat = new_object(new_wbyte(result));
}

int yylex(void) {
	int t, lt;
	SCANTOKEN:do {
	t = fgetc(yyin);
	switch (t) {
		// Ignore blank spaces
		case ' ':
		case '\t':
		case '\n':
		case '\v':
		case '\r':
			if (t == '\n' || t == '\v') lineno++;
			goto SCANTOKEN;
		// Comments
		case '?':
			wscan_comment(yyin);
			goto SCANTOKEN;
		// Strings
		case '"':
			wscan_string(yyin);
			return STRING;
		// Special tokens
		case '.':
			return DOT;
		case ',':
			return COMMA;
		case ':':
			t = fgetc(yyin);
			if (t == '=') return EQ;
			fseek(yyin, -1, SEEK_CUR);
			return COLON;
		case ';':
			do {
				t = fgetc(yyin);
			} while (t != '\n');
			lineno++;
			return SEMICOLON;
		// Operators
		case '<':
			t = fgetc(yyin);
			switch (t) {
				case '<':
					return '<';
				case '=':
					return 'L';
				default:
					fseek(yyin, -1, SEEK_CUR);
					return 'l';
			}
		case '>':
			t = fgetc(yyin);
			switch (t) {
				case '>':
					return '>';
				case '=':
					return 'G';
				default:
					fseek(yyin, -1, SEEK_CUR);
					return 'g';
			}
		case '/':
			t = fgetc(yyin);
			if (t == '=') return 'n';
			raiseError(SYNTAX_ERROR, "Invalid Token");
			return 0;
		case '=':
		case '&':
		case '|':
			lt = t;
			t = fgetc(yyin);
			if (t == lt) {
				if (t == '=') return 'e';
				else if (t == '&') return 'A';
				else if (t == '|') return 'O';
			}
			raiseError(SYNTAX_ERROR, "Invalid Token");
			return 0;
		case '!':
		case '#':
		case '+':
		case '-':
			return t;
		// Group tokens
		case '(':
			return LPARENT;
		case ')':
			return RPARENT;
		case '[':
			return LBRACKET;
		case ']':
			return RBRACKET;
		case '{':
			return LBRACE;
		case '}':
			return RBRACE;
		case EOF:
			return 0;
		default:
			if (is_nameinit(t)) {
				// Names
				return wscan_name(yyin, t);
			} else if (is_valid_number(t)) {
				// Numbers
				wscan_number(yyin);
				return NUMBER;
			} else {
				raiseError(SYNTAX_ERROR, "Invalid Token");
				return 0;
			}
	}
	} while (0);
}

