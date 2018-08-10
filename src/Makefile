
DESTDIR = ~/..
PREFIX = /usr
CC = cc

build: parser stack dict object eval scope ast error
	$(CC) *.o parser.tab.c lex.yy.c -o ward -lfl

install: ward
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $< $(DESTDIR)$(PREFIX)/bin/ward

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/ward

eval: evaluator.c evaluator.h
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

parser: lexer.l parser.y
	flex lexer.l
	bison -d parser.y

clean:
	rm *.o parser.tab.* lex.yy.c

