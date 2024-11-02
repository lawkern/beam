CFLAGS = -g -Wno-c++11-compat-deprecated-writable-strings

compile:
	@mkdir -p build
	clang -O2 -DDEBUG=0 ./src/platform_clib.cpp -c -o ./build/platform_release.o $(CFLAGS)
	clang -O0 -DDEBUG=1 ./src/platform_clib.cpp -c -o ./build/platform_debug.o $(CFLAGS)

	clang -O2 -DDEBUG=0 ./src/main_sdl.cpp ./build/platform_release.o -o ./build/beam_release $(CFLAGS) `sdl2-config --cflags --libs`
	clang -O0 -DDEBUG=1 ./src/main_sdl.cpp ./build/platform_debug.o   -o ./build/beam_debug   $(CFLAGS) `sdl2-config --cflags --libs`

run:
	./build/beam_debug
