CC = gcc
CFLAGS = -std=c99 -Wall
PARSER = parser.tab.c wlexer.o
OBJECTS = stack.o dict.o object.o evaluator.o scope.o ast.o error.o
EXEC = ward

LIB = libward_socket.so libward_file.so
LIB_REQS = object.o error.o stack.o dict.o scope.o

all: $(PARSER) $(OBJECTS) $(EXEC) $(LIB)
	@printf '======================\n'
	@printf '  Ready to install.\n'
	@printf '======================\n'

$(EXEC): $(OBJECTS) $(PARSER)
	$(CC) $(PARSER) $(OBJECTS) -o $(EXEC)

$(LIB): $(OBJECTS) clib/ward*.c
	mkdir -p $(PREFIX)/include/ward
	cp object.h error.h scope.h dict.h stack.h $(PREFIX)/include/ward/
	@printf '======================\n'
	@printf 'Compiling Libraries...\n'
	@printf '======================\n'
	$(CC) -fPIC -c clib/ward*.c
	$(CC) -shared -o libward_socket.so wardsocket.o $(LIB_REQS)
	$(CC) -shared -o libward_file.so wardfile.o $(LIB_REQS)

install: $(EXEC) $(LIB)
	cp $(EXEC) $(PREFIX)/bin/$(EXEC)
	cp $(LIB) $(PREFIX)/lib/
	mkdir -p $(PREFIX)/lib/ward
	cp lib/*.ward $(PREFIX)/lib/ward/
	@printf '======================\n'
	@printf ' Instalation success.\n'
	@printf '======================\n'

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

$(PARSER): wlexer.c parser.y ward.h
	bison -d parser.y
	$(CC) -c wlexer.c

clean:
	rm -f parser.tab.* *.o *.so $(EXEC)

