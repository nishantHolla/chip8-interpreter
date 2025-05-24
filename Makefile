CC=g++
CFLAGS=-Wall -Wextra -pedantic -Isrc/include -Lsrc/lib
DEBUG_FLAGS=-ggdb -fsanitize=address -D__C8I_MEM_BOUND_CHECK__
RELEASE_FLAGS=-O3
SRC=src/*.cpp
OUT=-o out/c8i
LINK_LIB=-lC8I_SDL3
ARGS=

pre:
	mkdir -p out


debug: pre
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(OUT) $(SRC) $(LINK_LIB)

release: pre
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(OUT) $(SRC) $(LINK_LIB)

runDebug: debug
	cd out && ./c8i $(ARGS)
