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
	clang ./src/game.cpp -o ./build/game_release.o -c -O2 -DDEBUG=0 $(CFLAGS)
	clang ./src/game.cpp -o ./build/game_debug.o   -c -O0 -DDEBUG=1 $(CFLAGS)

	clang ./src/main_sdl.cpp ./build/game_release.o -o ./build/beam_release -O2 -DDEBUG=0 $(CFLAGS) $(LDFLAGS) `sdl2-config --cflags --libs`
	clang ./src/main_sdl.cpp ./build/game_debug.o   -o ./build/beam_debug   -O0 -DDEBUG=1 $(CFLAGS) $(LDFLAGS) `sdl2-config --cflags --libs`

pack:
	clang ./src/main_pack.cpp -o ./build/pack
	./build/pack cube falcon

debug:
	lldb ./build/beam_debug

run:
	./build/beam_debug
