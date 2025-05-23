CC=g++
CFLAGS=-Wall -Wextra -pedantic
DEBUG_FLAGS=-ggdb -fsanitize=address
RELEASE_FLAGS=-O3
SRC=src/*.cpp
OUT=-o out/c8i

pre:
	mkdir -p out


debug: pre
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(OUT) $(SRC)

release: pre
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(OUT) $(SRC)

runDebug: debug
	./out/c8i
