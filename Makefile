CFLAGS = -g -fno-exceptions -Wall -Werror -Wno-unused-variable -Wno-unused-function
LDFLAGS = -lm

CFLAGS  += $(shell pkg-config sdl3 --cflags)
LDFLAGS += $(shell pkg-config sdl3 --libs)

compile:
	mkdir -p ./build

#	Game platform code.
	$(CC) ./src/platform_sdl.cpp -c -o ./build/platform_release.o -O2 -DDEBUG=0 $(CFLAGS)
	$(CC) ./src/platform_sdl.cpp -c -o ./build/platform_debug.o   -O0 -DDEBUG=1 $(CFLAGS)

#	Server code.
	$(CC) -c ./src/server.cpp -o ./build/server_release.o -O2 -DDEBUG=0 $(CFLAGS)
	$(CC) -c ./src/server.cpp -o ./build/server_debug.o   -O0 -DDEBUG=1 $(CFLAGS)

#	Server entry point.
#	$(CC) ./src/main_server_sdl.cpp ./build/server_release.o -o ./build/beam_server_release -O2 -DDEBUAG=0 $(CFLAGS) $(LDFLAGS)
#	$(CC) ./src/main_server_sdl.cpp ./build/server_debug.o   -o ./build/beam_server_debug   -O0 -DDEBUG=1 $(CFLAGS) $(LDFLAGS)

#	Game code.
	$(CC) -c ./src/game.cpp -o ./build/game_release.o -O2 -DDEBUG=0 $(CFLAGS)
	$(CC) -c ./src/game.cpp -o ./build/game_debug.o   -O0 -DDEBUG=1 $(CFLAGS)

#	Game entry point.
	$(CC) ./src/main_game.cpp ./build/platform_release.o ./build/game_release.o -o ./build/beam_release -O2 -DDEBUG=0 $(CFLAGS) $(LDFLAGS)
	$(CC) ./src/main_game.cpp ./build/platform_debug.o   ./build/game_debug.o   -o ./build/beam_debug   -O0 -DDEBUG=1 $(CFLAGS) $(LDFLAGS)

pack:
	mkdir -p ./build

	$(CC) ./src/main_packer.cpp $(CFLAGS) -o ./build/packer
	./build/packer cube falcon

serve:
	$(DBG) ./build/beam_server_debug

debug:
	$(DBG) ./build/beam_debug

run:
	./build/beam_debug
