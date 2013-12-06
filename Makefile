CFLAGS = -Wall -O3
OBJ = morse.o gpio.o main.o
PROG = morse

default: $(PROG)

$(PROG): $(OBJ)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJ) -lrt

clean:
	rm -f $(OBJ) $(PROG)
