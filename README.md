## Windows setup instructions

1. Install MinGW-W64
   1. Download [here](https://sourceforge.net/projects/mingw-w64/files/). Choose the `x86_64-posix-sjlj` configuration.
   2. Unzip and copy `mingw64` into `C:`.
   3. Add `C:/mingw64/bin` to PATH.
2. Install GLEW
   1. Download [here](https://glew.sourceforge.net/index.html).
   2. Copy `include/*` to `C:/mingw64/include/`.
   3. Copy `lib/Release/x64/*` to `C:/mingw64/lib/`.
   4. Copy `bin/Release/x64/*` to `C:/Windows/System32/`.
3. Install GLFW
   1. Download [here](https://www.glfw.org/download).
   2. Copy `include/*` to `C:/mingw64/include/`.
   3.  Copy `lib-mingw-w64`  to `C:/mingw64/lib/`.

Instead of `make` use `mingw32-make`.