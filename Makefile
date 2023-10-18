CC = gcc

CFLAGS = `pkg-config --cflags gtk+-3.0 gtksourceview-3.0`

LIBS = `pkg-config --libs gtk+-3.0 gtksourceview-3.0`

# File names

SRC = sgnotes.c

OBJ = $(SRC:.c=.o)

EXE = sgnotes

# Build executable files

all: $(EXE)

sgnotes: sgnotes.o

	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

# Debug step

debug:

	$(CC) $(CFLAGS) -g $(SRC) -o debug $(LIBS)

# Test step

test:

	./sgnotes

# Clean object files and executables

clean:

	rm -f $(OBJ) $(EXE) debug
