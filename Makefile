CFLAGS = -g -std=c++11 -fno-exceptions -Wall -Werror -Wno-unused-variable -Wno-unused-function -Wno-write-strings -Wno-writable-strings -Wno-missing-braces -Wno-nested-anon-types -Wno-gnu-anonymous-struct
LDFLAGS = -lm

SDL_CFLAGS = $(CFLAGS) $(shell pkg-config sdl3 --cflags)
SDL_LDFLAGS = $(LDFLAGS) $(shell pkg-config sdl3 --libs)

platform: game server
	@mkdir -p ./build
	$(CC) ./src/main_game_sdl.cpp ./build/game_release.o -o ./build/beam_release -O2 -DDEBUG=0 $(SDL_CFLAGS) $(SDL_LDFLAGS)
	$(CC) ./src/main_game_sdl.cpp ./build/game_debug.o   -o ./build/beam_debug   -O0 -DDEBUG=1 $(SDL_CFLAGS) $(SDL_LDFLAGS)

#	$(CC) ./src/main_server_sdl.cpp ./build/server_release.o -o ./build/beam_server_release -O2 -DDEBUG=0 $(SDL_CFLAGS) $(SDL_LDFLAGS)
#	$(CC) ./src/main_server_sdl.cpp ./build/server_debug.o   -o ./build/beam_server_debug   -O0 -DDEBUG=1 $(SDL_CFLAGS) $(SDL_LDFLAGS)

game:
	@mkdir -p ./build
	$(CC) ./src/game.cpp -o ./build/game_release.o -c -O2 -DDEBUG=0 $(CFLAGS)
	$(CC) ./src/game.cpp -o ./build/game_debug.o   -c -O0 -DDEBUG=1 $(CFLAGS)

server:
	@mkdir -p ./build
	$(CC) ./src/server.cpp -o ./build/server_release.o -c -O2 -DDEBUG=0 $(CFLAGS)
	$(CC) ./src/server.cpp -o ./build/server_debug.o   -c -O0 -DDEBUG=1 $(CFLAGS)

pack:
	@mkdir -p ./build
	$(CC) ./src/main_packer.cpp $(CFLAGS) -o ./build/packer
	./build/packer cube falcon

serve:
	$(DBG) ./build/beam_server_debug

debug:
	$(DBG) ./build/beam_debug

run:
	./build/beam_debug
