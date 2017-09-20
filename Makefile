CC=gcc
CFLAGS=-I.
DEPS = parse.h y.tab.h
OBJ = y.tab.o lex.yy.o parse.o example.o
FLAGS = -g -Wall
OBJ2 = y.tab.o lex.yy.o parse.o echo_server.o

default:all

all: example server client

lex.yy.c: lexer.l
	flex $^

y.tab.c: parser.y
	yacc -d $^

%.o: %.c $(DEPS)
	$(CC) $(FLAGS) -c -o $@ $< $(CFLAGS)

example: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

server: $(OBJ2)
	$(CC) -o $@ $^ $(CFLAGS)

client:
	@gcc echo_client.c -o client -Wall -Werror
	@echo "############### Build Successful ############### \n"

clean:
	rm -f *~ *.o example lex.yy.c y.tab.c y.tab.h
	rm -f *~ client server log.txt
	@echo "############### Make Clean Success ############## \n"

