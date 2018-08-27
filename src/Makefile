
DESTDIR = ~/..
PREFIX = /usr
CC = gcc
CFLAGS = -std=c99
PARSER = parser.tab.c lex.yy.c
OBJECTS = stack.o dict.o object.o evaluator.o scope.o ast.o error.o
EXEC = ward

LIB = libward.a

all: $(PARSER) $(OBJECTS) $(EXEC) $(LIB)
	@printf '=================\n'
	@printf 'Ready to install.\n'
	@printf '=================\n'

$(EXEC): $(OBJECTS) $(PARSER)
	$(CC) $(PARSER) $(OBJECTS) -o $(EXEC) -lfl

$(LIB): $(OBJECTS)
	ar rc $(LIB) $(OBJECTS)

install: $(EXEC) $(LIB)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $< $(DESTDIR)$(PREFIX)/bin/$(EXEC)
	mv $(LIB) stdlib

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(EXEC)

evaluator: evaluator.c evaluator.h
	$(CC) -c evaluator.c

scope: scope.c scope.h
	$(CC) -c scope.c

ast: ast.c ast.h
	$(CC) -c ast.c

object: object.c object.h
	$(CC) -c object.c

stack: stack.c stack.h
	$(CC) -c stack.c

dict: dict.c dict.h
	$(CC) -c dict.c

error: error.c error.h
	$(CC) -c error.c

$(PARSER): lexer.l parser.y
	flex lexer.l
	bison -d parser.y

clean:
	rm -f parser.tab.* lex.yy.c *.o ward.a

