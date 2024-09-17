CC = gcc
CFLAGS = -Wall

dragonshell: compile
	$(CC) $(CFLAGS) -o dragonshell dragonshell.o

# leverage make's implicit recipe for object files
compile: dragonshell.o

clean:
	rm -f *.o dragonshell dsh_debug

valgrind: dsh_debug
	valgrind --tool=memcheck --leak-check=yes ./dsh_debug

dsh_debug: db_dragonshell.o
	$(CC) $(CFLAGS) -O0 -g -o $@ $^

db_dragonshell.o: dragonshell.c
	$(CC) $(CFLAGS) -O0 -g -c -o $@ dragonshell.c
