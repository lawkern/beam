CFLAGS = -g -std=c++11 -Wall -Wpedantic -Werror
CFLAGS += -Wno-unused-variable
CFLAGS += -Wno-unused-function
CFLAGS += -Wno-writable-strings
CFLAGS += -Wno-missing-braces
CFLAGS += -Wno-nested-anon-types
CFLAGS += -Wno-gnu-anonymous-struct

LDFLAGS = -lm

compile:
	@mkdir -p build
	clang ./src/game.cpp -O2 -DDEBUG=0 -c -o ./build/game_release.o $(CFLAGS)
	clang ./src/game.cpp -O0 -DDEBUG=1 -c -o ./build/game_debug.o   $(CFLAGS)

	clang ./src/main_sdl.cpp -O2 -DDEBUG=0 ./build/game_release.o -o ./build/beam_release $(CFLAGS) $(LDFLAGS) `sdl2-config --cflags --libs`
	clang ./src/main_sdl.cpp -O0 -DDEBUG=1 ./build/game_debug.o   -o ./build/beam_debug   $(CFLAGS) $(LDFLAGS) `sdl2-config --cflags --libs`

pack:
	clang ./src/main_pack.cpp -o ./build/pack
	./build/pack cube.obj

run:
	./build/beam_debug
