
install: parser stack dict object eval scope ast error
	cc *.o parser.tab.c lex.yy.c -o bwar -lfl -g
	make clean

eval: evaluator.c evaluator.h
	cc -c evaluator.c

scope: scope.c scope.h
	cc -c scope.c

ast: ast.c ast.h
	cc -c ast.c

object: object.c object.h
	cc -c object.c

stack: stack.c stack.h
	cc -c stack.c

dict: dict.c dict.h
	cc -c dict.c

error: error.c error.h
	cc -c error.c

parser: lexer.l parser.y
	flex lexer.l
	bison -d parser.y

clean:
	rm *.o parser.tab.* lex.yy.c

