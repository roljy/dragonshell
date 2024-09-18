CC = gcc
CFLAGS = -Wall -std=c11
DBFLAGS = -O0 -g

dragonshell: dragonshell.o shellio.o handlers.o
	$(CC) $(CFLAGS) $^ -o dragonshell

# leverage make's implicit recipe for object files
compile: dragonshell.o shellio.o handlers.o

clean:
	rm -f *.o dragonshell dsh_debug

valgrind: dsh_debug
	valgrind --tool=memcheck --leak-check=yes ./dsh_debug

dsh_debug: db_dragonshell.o db_shellio.o db_handlers.o
	$(CC) $(CFLAGS) $(DBFLAGS) $^ -o $@

db_%.o: %.c
	$(CC) $(CFLAGS) $(DBFLAGS) -c $^ -o $@
