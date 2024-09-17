CC = gcc
CFLAGS = -Wall

dragonshell: compile
	$(CC) $(CFLAGS) -o dragonshell *.o

# leverage make's implicit recipe for object files
compile: dragonshell.o

clean:
	rm -f *.o dragonshell dsh_debug

valgrind:
	make dsh_debug
	valgrind --tool=memcheck --leak-check=yes ./dsh_debug

dsh_debug:
	$(CC) $(CFLAGS) -O0 -g -o dsh_debug *.c
