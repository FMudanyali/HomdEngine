# Setting Up Environment for Windows

- Install MSYS2 from [here](https://www.msys2.org/) to `C:\msys64`
- Once installed, open MSYS2 CLANG64 and run the following command:

```
pacman -S mingw-w64-clang-x86_64-toolchain mingw-w64-clang-x86_64-cmake mingw-w64-clang-x86_64-SDL2 mingw-w64-clang-x86_64-glew
```

- Add `C:\msys64\clang64\bin` to your `Path` from Environment Variables.
- Run the following steps:

```
mkdir build
cd build
cmake ..
ninja
```
