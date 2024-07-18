DATE := $(shell git log -n 1 --format="%ad" --date=format-local:'%Y%m%d/%H:%M:%S')
CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0 gtksourceview-3.0` -Dmver=\"$(DATE)\"
LIBS = `pkg-config --libs gtk+-3.0 gtksourceview-3.0`
SRC = sgnotes.c
OBJ = $(SRC:.c=.o)
EXE = sgnotes

PREFIX = /usr
BIN_DIR = $(PREFIX)/bin
APP_DIR = $(PREFIX)/share/applications

all: $(EXE)
sgnotes: sgnotes.o
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)
debug:
	$(CC) $(CFLAGS) -g $(SRC) -o debug $(LIBS)
test:
	./$(EXE)
install:
	install -Dm755 $(EXE) $(BIN_DIR)/$(EXE)
	install -Dm644 $(EXE).desktop $(APP_DIR)/$(EXE).desktop
uninstall:
	rm $(BIN_DIR)/$(EXE)
	rm $(APP_DIR)/$(EXE).desktop
clean:
	rm -f $(OBJ) $(EXE) debug
