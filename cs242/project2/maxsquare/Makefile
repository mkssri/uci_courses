all: main.c
	#gcc -Wall -fopenmp -std=c99 -o main main.c transpose.c
	g++ -fopenmp -pthread -o main main.c transpose.c

clean:
	$(RM) main core.*
