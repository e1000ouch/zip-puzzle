# ZIP - jeu de parcours logique en C
# Utilisation : make (ou mingw32-make sous MSYS2/MinGW)

CC      ?= gcc
CFLAGS  ?= -std=c11 -O2 -Wall -Wextra
CPPFLAGS = -Iinclude
TARGET   = zip.exe
SRC      = $(wildcard src/*.c)
OBJ      = $(SRC:src/%.c=build/%.o)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

build/%.o: src/%.c include/*.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build:
	mkdir build

run: $(TARGET)
	./$(TARGET)

clean:
	-rm -rf build $(TARGET) sauvegarde.txt
