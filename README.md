
Prerequisities for all build: SDL, SDL_image and SDL_mixer libraries:

    https://github.com/libsdl-org/SDL/releases/tag/release-2.30.0
    https://github.com/libsdl-org/SDL_image/releases
    https://github.com/libsdl-org/SDL_mixer/releases


Linux
-----

Replace <SDL2_PREFIX>, <SDL2_IMAGE_PREFIX> and <SDL2_MIXER_PREFIX> with the installation
prefix of the SDL, SDL_image and SDL_mixer libraries, respectively (for example:
/opt/sdl2/2.28.3 or /usr/local)

```
    git clone <repository> retro-fred
    cd retro-fred
    mkdir build
    cd build
    cmake '-DCMAKE_PREFIX_PATH=<SDL2_PREFIX>;<SDL2_IMAGE_PREFIX>;<SDL2_MIXER_PREFIX>' ..
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

The Android build needs the source distributions of SDL, SDL_image and SDL_mixer. The SDL
source code needs to be patched with patch-SDL2-2.29.3-aaudio-buffer-size.patch to ensure
that the audio buffer gets set correctly.

First, apply the patch (<SDL2_SOURCE>, <SDL2_IMAGE_SOURCE> and <SDL2_MIXER_SOURCE> are the
directories of the source distributions of the SDL, SDL_image and SDL_mixer libraries):

```
    RETROFRED_ROOT=$(pwd); (cd <SDL2_SOURCE>; patch -p1 <$RETROFRED_ROOT/android/patch-SDL2-2.29.3-aaudio-buffer-size.patch)
```

Then set up the links to the source code of the SDL2 libraries in the android directory,
and an internal link to the assets directory from the android/app/src/main directory:

```
    ./android/configure --sdl=<SDL2_SOURCE> \
                        --sdl-image=<SDL2_IMAGE_SOURCE> \
                        --sdl-mixer=<SDL_MIXER_SOURCE>
```

Now the build can be started using the gradle tool included in the android directory.
JAVA_HOME must point to the JDK directory, and ANDROID_HOME to the Android SDK directory:

```
    JAVA_HOME=<JAVA_HOME> ANDROID_HOME=<ANDROID_HOME> ./android/gradlew build
```

The APKs with the packages will be available in the android/app/build/outputs/apk
directory.

Android SDK versions (see File|Settings, Languages & Frameworks|Android SDK from Android
studio):

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

To build the project using Android Studio:

  * In Android Studio: File|Open..., then open the android-project
    folder

  * Select Build | Make Project


Windows - Visual Studio
-----------------------

Tested with Visual Studio 2022, and CMake 3.28.3
(note that the cmake install is not the one that comes with Visual Studio)

Replace <SDL2_PREFIX>, <SDL2_IMAGE_PREFIX> and <SDL2_MIXER_PREFIX> with the installation
prefix of the SDL, SDL_image and SDL_mixer libraries, respectively (typically the
directory where the .zip file of the release has been unpacked)

```
    git clone <repository> retro-fred
    cd retro-fred
    mkdir build
    cd build
    mkdir win64
    cd win64
    cmake -G "Visual Studio 17 2022" -A x64 \
        "-DCMAKE_PREFIX_PATH=<SDL2_PREFIX>;<SDL2_IMAGE_PREFIX>;<SDL2_MIXER_REFIX>" ../..
    cmake --build . --config Release
    cpack -G ZIP --config .\CPackConfig.cmake
```

And for the 32 bit build:

```
    cd ..\..\build
    mkdir win32
    cd win32
    cmake -G "Visual Studio 17 2022" -A Win32 \
        "-DCMAKE_PREFIX_PATH=<SDL2_PREFIX>;<SDL2_IMAGE_PREFIX>;<SDL2_MIXER_PREFIX>" ../..
    cmake --build . --config Release
    cpack -G ZIP --config .\CPackConfig.cmake
```

Windows - mingw-w64: cross compiling from Linux
-----------------------------------------------

Replace <SDL2_PREFIX>, <SDL2_IMAGE_PREFIX> and <SDL2_MIXER_PREFIX> with the installation
prefix of the SDL, SDL_image and SDL_mixer libraries for the x86_64 target. For example:
/opt/sdl2/2.30.0/x86_64-w64-mingw32 and /opt/sdl2_image/2.8.2/x86_64-w64-mingw32. You can
use the binary distributions from the SDL2, SDL2_image and SDL2_mixer release pages.

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
    cmake '-DCMAKE_PREFIX_PATH=<SDL2_PREFIX>;<SDL2_IMAGE_PREFIX>;<SDL2_MIXER_PREFIX>' \
        -DCMAKE_TOOLCHAIN_FILE=<MINGW_TOOLCHAIN> -G Ninja ../..
    cmake --build .
```

TODO: you should be able to run cpack to create a zip file, but it
currently fails to archive all the required .dlls:

```
    cpack -G ZIP --config ./CPackConfig.cmake
```

MacOS X
-------

First you need to install SDL2, SDL2_image and SDL2_mixer. The easiest way is to download
the .dmg packages from the SDL sites. Then copy the SDL2.framework, SDL2_image.framework
and SDL2_mixer.framework directories to the system's /Library/Frameworks directory, or the
user's Library/Frameworks directory inside the home directory. You can also install it in
a different directory. In that case you will need to add option
"-DCMAKE_PREFIX_PATH=<SDL_FRAMEWORK_DIR>;<SDL_IMAGE_FRAMEWORK_DIR>;<SDL_MIXER_FRAMEWORK_DIR>"
to the cmake invocation to specify the location of the libraries.

```
    git clone <repository> retro-fred
    cd retro-fred
    mkdir build
    cd build
    cmake ..
    cmake --build . --config Release
    cpack -G Bundle --config ./CPackConfig.cmake
```

This will generate a Retro-Fred-<version>.dmg file in the build directory.

You can also build using the Xcode generator:

```
    git clone <repository> retro-fred
    cd retro-fred
    mkdir build
    cd build
    cmake -G Xcode ..
    cmake --build . --config Release
    cpack -G Bundle --config ./CPackConfig.cmake
```
