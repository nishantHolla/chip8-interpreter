CC=g++
CFLAGS=-Wall -Wextra -pedantic -Isrc/include
DEBUG_FLAGS=-ggdb -fsanitize=address -D__C8I_MEM_BOUND_CHECK__
RELEASE_FLAGS=-O3
SRC=src/*.cpp
OUT=-o out/c8i
LINK_LIB=-lSDL3
ARGS=

pre:
	mkdir -p out

debug: pre
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(OUT) $(SRC) $(LINK_LIB)

debugState: pre
	$(CC) $(CFLAGS) -D__C8I_DEBUG_STATE__ $(DEBUG_FLAGS) $(OUT) $(SRC) $(LINK_LIB)

release: pre
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(OUT) $(SRC) $(LINK_LIB)

runDebug: debug
	cd out && ./c8i $(ARGS)

runDebugState: debugState
	cd out && ./c8i $(ARGS)
