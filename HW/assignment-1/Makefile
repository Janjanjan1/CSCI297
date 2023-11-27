all: main


lib_myarray.a: myarray.o
	ar rcs lib_myarray.a myarray.o

main.o:main.c
	gcc -c main.c

myarray.o:myarray.c
	gcc -c myarray.c

main: main.o lib_myarray.a
	gcc main.o -L. -l_myarray -o main

clean: 
	rm -f *.o *.a main

.PHONY: all clean