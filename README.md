
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

Note that the generated .dmg file and the .app bundle inside are not signed, so they will
not run if you transfer them to a different machine. To be able to run in a different
machine you need to copy the .app directory to a separate directory, then use `xattr -c
Retro-Fred.app` to clear the attributes, which should allow the executable to run.

iOS
---

For iOS we mostly followed the instructions from this post:

  https://discourse.libsdl.org/t/latest-sdl2-2-0-26-ios-dev-anyone-having-any-success-solved/41524

The LazyFoo tutorial (https://lazyfoo.net/tutorials/SDL/52_hello_mobile/ios_mac/index.php)
was also pretty useful, as well as the documentation in the SDL library itself
(https://github.com/libsdl-org/SDL/blob/release-2.30.x/docs/README-ios.md).

The steps are roughly the following:

  1) Create a work directory, and unpack the SDL libraries, and clone the repository in
     it:

```
    cd <work_directory>
    tar xf SDL2-<version>
    tar xf SDL2_image-<version>
    tar xf SDL2_mixer-<version>
    git clone <repository> retro-fred
````

  2) Build SDL2:

      * Open the Xcode project Xcode/SDL/SDL.xcodeproj inside the SDL source code
        directory.
      * Set Framework-iOS and Build->Any iOS device (arm64) in the status bar up top (see
        the instructions in the libsdl discussion forum)
      * Go to Product->Archive. This will build SDL2 in Release mode.
      * The Organizer window will pop up and you will see the archive. Right click on it
        and select "Show in Finder"
      * In the Finder window right click on the archive and pick "Show Package Contents".
        Go to Product/Library/Frameworks.
      * Copy SDL2.framework to the retro-fred/ios directory in the work directory.

  3) Build SDL2_image and SDL2_mixer: repeat the previous steps for SDL2_image and
     SDL2_mixer. You may need to add the SDL2 framework to each of them so that they can
     find the SDL library.

  4) Open the retro-fred/ios/Retro-Fred.xcodeproj project with Xcode

  5) Select Retro-Fred on the Project Navigator, select General, and ensure that the SDL2
     frameworks in "Frameworks, Libraries and Embedded Content" point to the frameworks
     that you just built.

  6) In Build Settings ensure that the Header Search Paths point to the include
     directories inside the source packages of the SDL libraries. Note that we do not want
     to point to the include headers in the frameworks, since the headers in the
     frameworks sit in the SDL2 directory instead of the top level directory.

  7) Select Product->Archive to build
