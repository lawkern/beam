beam is an in-development 3D racing game.

The current codebase depends on SDL3 for basic platform functionality and
SDL_net for UDP networking, so be sure to install those first. Likewise, clang
and make are required for compilation.

The top-level Makefile handles compiling and running the game. Some targets you
might care about include:

   make compile  # Build all targets from scratch (default)
   make pack     # Build and run asset packer
   make run      # Run the game debug build from the current shell
   make debug    # Run the game debug build under a debugger
   make serve    # Run the server debug build under a debugger
