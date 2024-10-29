VERSION := $(shell cat VERSION)
CC = gcc

SOURCEVIEW =
SOURCEVIEWARG =

ifeq ($(WITHSOURCEVIEW),1)
	SOURCEVIEW = gtksourceview-3.0
	SOURCEVIEWARG = -DWITHSOURCEVIEW
endif

CFLAGS = `pkg-config --cflags gtk+-3.0 $(SOURCEVIEW)` -Dmver=\"$(VERSION)\" $(SOURCEVIEWARG)
LIBS = `pkg-config --libs gtk+-3.0 $(SOURCEVIEW)`
SRC = sgnotes.c
OBJ = $(SRC:.c=.o)
EXE = sgnotes

PREFIX = /usr
BIN_DIR = $(PREFIX)/bin
APP_DIR = $(PREFIX)/share/applications
ICON_DIR = $(PREFIX)/share/icons/hicolor/64x64/apps/

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
	install -Dm644 icons/$(EXE).svg $(ICON_DIR)/$(EXE).svg
uninstall:
	rm $(BIN_DIR)/$(EXE)
	rm $(APP_DIR)/$(EXE).desktop
	rm $(ICON_DIR)/$(EXE).svg
clean:
	rm -f $(OBJ) $(EXE) debug