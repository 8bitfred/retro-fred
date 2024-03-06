
Prerequisities for all build: SDL and SDL_image libraries:

    https://github.com/libsdl-org/SDL/releases/tag/release-2.30.0
    https://github.com/libsdl-org/SDL_image/releases


Linux
-----

Replace <SDL2_PREFIX> and <SDL2_IMAGE_PREFIX> with the installation prefix of the SDL and
SDL_image libraries, respectively (for example: /opt/sdl2/2.28.3 or /usr/local)

```
    git clone <repository> retro-fred
    cd retro-fred
    mkdir build
    cd build
    cmake '-DCMAKE_PREFIX_PATH=<SDL2_PREFIX>;<SDL2_IMAGE_PREFIX>' ..
    cmake --build .
```

To run, from the `retro-fred/build` directory:

```
    ./cpp/retro-fred
```

Note that the executable will expect to find subdirectories sprites and sounds in the
current working directory.

Use option --help to get a list of configuration options.


Android
-------

You need to add the following symbolic links in the android-project
subdirectory for building for Android:

  cd android-project/app/src/main
  ln -s ../../../../assets .
  cd ../../jni
  ln -s ../../../cpp .

As well as the links to SDL and SDL_image (in android-project/app/jni):

  ln -s /usr/local/src/miguel-src/sdl2/SDL2-2.29.3-aaudio-buffer-size SDL
  ln -s /usr/local/src/miguel-src/sdl2_image/SDL2_image-2.8.2 SDL_image

Note that SDL2 library must be patched with
patch-SDL2-2.29.3-aaudio-buffer-size.patch to ensure that the audio
buffer gets set correctly.

Android SDK versions (see File|Settings, Languages & Frameworks|Android SDK):

  SDK Tools:
    Android SDK Build-Tools 34
      34.0.0
      33.0.1
      30.0.3
    NDK
      25.1.8937393
    CMake
      3.22.1
      3.18.1
    Android Emulator 33.1.23
    Android SDK Platform-Tools 34.0.5

  SDK Platforms:
    Android 14.0
      Android SDK Platform 34
      Sources for Android 34
      Google APIs Intel x86_64 Atom System Image
    Android 13.0
      Android SDK Platform 33

To build the project:

  * In Android Studio: File|Open..., then open the android-project
    folder

  * Select Build | Make Project


Windows - Visual Studio
-----------------------

Tested with Visual Studio 2022, and CMake 3.28.3
(note that the cmake install is not the one that comes with Visual Studio)

Replace <SDL2_PREFIX> and <SDL2_IMAGE_PREFIX> with the installation prefix of the SDL and
SDL_image libraries, respectively (typically the directory where the .zip file of the
release has been unpacked)

```
    git clone <repository> retro-fred
    cd retro-fred
    mkdir build
    cd build
    mkdir win64
    cd win64
    cmake -G "Visual Studio 17 2022" -A x64 "-DCMAKE_PREFIX_PATH=<SDL2_PREFIX>;<SDL2_IMAGE_PREFIX>" ../..
    cmake --build . --config Release
    cpack -G ZIP --config .\CPackConfig.cmake
```

And for the 32 bit build:

```
    cd ..\..\build
    mkdir win32
    cd win32
    cmake -G "Visual Studio 17 2022" -A Win32 "-DCMAKE_PREFIX_PATH=<SDL2_PREFIX>;<SDL2_IMAGE_PREFIX>" ../..
    cmake --build . --config Release
    cpack -G ZIP --config .\CPackConfig.cmake
```

Windows - mingw-w64: cross compiling from Linux
-----------------------------------------------

Replace <SDL2_PREFIX> and <SDL2_IMAGE_PREFIX> with the installation
prefix of the SDL and SDL_image libraries for the x86_64 target. For
example: /opt/sdl2/2.30.0/x86_64-w64-mingw32 and
/opt/sdl2_image/2.8.2/x86_64-w64-mingw32. You can use the binary
distributions from the SDL2 and SDL2_image release page, respectively.

Replace <MINGW_TOOLCHAIN> with the path of the CMake toolchain file
for the mingw install. For example:
/opt/mingw/6.0.1/x86_64-w64-mingw32/lib/cmake/x86_64-w64-mingw32.cmake

Example toolchain file for mingw:

```
    set(CMAKE_SYSTEM_NAME Windows)
    set(TOOLCHAIN_PATH /opt/mingw/6.0.1)
    set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)
    set(CMAKE_SYSTEM_VERSION 1)
    set(CMAKE_SYSTEM_PROCESSOR x86_64)

    # cross compilers to use for C and C++
    set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-gcc)
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-g++)

    # target environment on the build host system
    set(CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PATH}/${TOOLCHAIN_PREFIX})

    # modify default behavior of FIND_XXX() commands
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```


```
    git clone <repository> retro-fred
    cd retro-fred
    mkdir -p build/mingw-w64
    cd build/mingw-w64
    cmake '-DCMAKE_PREFIX_PATH=<SDL2_PREFIX>;<SDL2_IMAGE_PREFIX>' -DCMAKE_TOOLCHAIN_FILE=<MINGW_TOOLCHAIN> -G Ninja ../..
    cmake --build .
```

TODO: you should be able to run cpack to create a zip file, but it
currently fails to archive all the required .dlls:

```
    cpack -G ZIP --config ./CPackConfig.cmake
```
