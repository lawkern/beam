beam is an in-development 3D racing game.

The current codebase depends on SDL2 for basic platform functionality, so be
sure to install that first. Likewise, clang and make are required for
compilation.

The top-level Makefile handles compiling and running the game:

   make compile # Build all targets from scratch (default)
   make run     # Run the debug build of the executable
