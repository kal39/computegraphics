## Windows setup instructions

`[mingw_root_path]` should be something like `C:\MinGW` or `C:\mingw64` etc.

1. Install MinGW-W64
   1. Download [here](https://sourceforge.net/projects/mingw-w64/files/). Choose the `x86_64-posix-sjlj` configuration.
   2. Unzip and copy `mingw64\*` into `[mingw_root_path]\*`.
   3. Add `[mingw_root_path]\bin` to PATH.
3. Install GLFW
   1. Download [here](https://www.glfw.org/download).
   2. Copy `include\*` to `C:\[mingw_root_path]\include\`.
   3.  Copy `lib-mingw-w64`  to `C:\[mingw_root_path]\lib\`.

Compile using:
```
mingw32-make -f makefile_win MINGW_PATH="[mingw_root_path]"
```
