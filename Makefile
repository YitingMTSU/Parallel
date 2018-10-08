p3: p3.c evalfunc.c
	gcc -o p3 $^ -lpthread

clean:
	rm -f *.o p3
