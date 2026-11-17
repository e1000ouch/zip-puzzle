# ZIP - jeu de parcours logique en C
# Utilisation : make (ou mingw32-make sous MSYS2/MinGW)

CC      ?= gcc
CFLAGS  ?= -std=c11 -O2 -Wall -Wextra
CPPFLAGS = -Iinclude
TARGET   = zip.exe
SRC      = $(wildcard src/*.c)
OBJ      = $(SRC:src/%.c=build/%.o)

.PHONY: all run test clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

build/%.o: src/%.c include/*.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build:
	mkdir build

run: $(TARGET)
	./$(TARGET)

# Tests de la logique du jeu (sans la boucle console de main.c)
TEST_OBJ = $(filter-out build/main.o build/console.o,$(OBJ))

build/test_logique.exe: tests/test_logique.c $(TEST_OBJ) include/*.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ tests/test_logique.c $(TEST_OBJ)

test: build/test_logique.exe
	./build/test_logique.exe

clean:
	-rm -rf build $(TARGET) sauvegarde.txt
