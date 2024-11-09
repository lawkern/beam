CC = clang
DBG = lldb

CFLAGS = -g -std=c++11 -Wall -Wpedantic -Werror -Wno-unused-variable -Wno-unused-function -Wno-writable-strings -Wno-missing-braces -Wno-nested-anon-types -Wno-gnu-anonymous-struct
LDFLAGS =
LDLIBS = -lm

SDL_CFLAGS = $(CFLAGS) $(shell sdl2-config --cflags)
SDL_LDFLAGS = $(LDFLAGS) $(shell sdl2-config --libs)
SDL_LDLIBS = $(LDLIBS) -lSDL2_net

platform: game server
	@mkdir -p ./build
	$(CC) ./src/main_game_sdl.cpp ./build/game_release.o -o ./build/beam_release -O2 -DDEBUG=0 $(SDL_CFLAGS) $(SDL_LDFLAGS) $(SDL_LDLIBS)
	$(CC) ./src/main_game_sdl.cpp ./build/game_debug.o   -o ./build/beam_debug   -O0 -DDEBUG=1 $(SDL_CFLAGS) $(SDL_LDFLAGS) $(SDL_LDLIBS)

game:
	@mkdir -p ./build
	$(CC) ./src/game.cpp -o ./build/game_release.o -c -O2 -DDEBUG=0 $(CFLAGS)
	$(CC) ./src/game.cpp -o ./build/game_debug.o   -c -O0 -DDEBUG=1 $(CFLAGS)

server:
	@mkdir -p ./build
	$(CC) ./src/main_server_sdl.cpp -o ./build/server_release -O2 -DDEBUG=0 $(SDL_CFLAGS) $(SDL_LDFLAGS) $(SDL_LDLIBS)
	$(CC) ./src/main_server_sdl.cpp -o ./build/server_debug   -O0 -DDEBUG=1 $(SDL_CFLAGS) $(SDL_LDFLAGS) $(SDL_LDLIBS)

pack:
	@mkdir -p ./build
	$(CC) ./src/main_packer.cpp -o ./build/packer
	./build/pack cube falcon

debug:
	$(DBG) ./build/beam_debug

run:
	./build/beam_debug
