COMPILER = gcc
CFLAGS = -Wall
PROGRAM = detect
$(PROGRAM): detect.c
	$(COMPILER) $(CFLAGS) -o $(PROGRAM) detect.c functions.c -std=c99

clean:
	-rm -f $(PROGRAM) *.o
