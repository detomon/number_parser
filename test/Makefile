CC      = clang
PROG    = test
CFLAGS  = -Wall -O2 -I../src
OBJ     = test.c ../src/number_parser.c

.PHONY: run

prog: $(OBJ)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJ)

clean:
	rm -rf $(OBJS) $(PROG)

run: prog
	./test
