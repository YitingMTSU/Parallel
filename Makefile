
p4:	p4.c p4.c
	mpicc -c p4.c -lm
	mpicc -o p4 p4.o -lm
clean:
	rm -f *.o
	rm p4
