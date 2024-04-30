DATE := $(shell git log -n 1 --format="%ad" --date=format-local:'%Y%m%d/%H:%M:%S')
DESTDIR = /usr/local/
CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0 gtksourceview-3.0` -Dmver=\"$(DATE)\"
LIBS = `pkg-config --libs gtk+-3.0 gtksourceview-3.0`
SRC = sgnotes.c
OBJ = $(SRC:.c=.o)
EXE = sgnotes

all: $(EXE)
sgnotes: sgnotes.o
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)
debug:
	$(CC) $(CFLAGS) -g $(SRC) -o debug $(LIBS)
test:
	./sgnotes
install:
	cp $(EXE) $(DESTDIR)/bin/$(EXE) && cp $(EXE).desktop $(DESTDIR)/share/$(EXE).desktop
uninstall:
	rm $(DESTDIR)/bin/$(EXE) $(DESTDIR)/share/$(EXE).desktop
clean:
	rm -f $(OBJ) $(EXE) debug
