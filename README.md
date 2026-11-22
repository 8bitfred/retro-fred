
Prerequisites for all builds: CMake 3.16 or newer, and the SDL3, SDL3_image and SDL3_mixer
libraries. The continuous integration build uses SDL 3.4.20, SDL3_image 3.4.8 and
SDL3_mixer 3.2.4, which are the latest versions at the time of writing:

    https://github.com/libsdl-org/SDL/releases/tag/release-3.4.20
    https://github.com/libsdl-org/SDL_image/releases/tag/release-3.4.8
    https://github.com/libsdl-org/SDL_mixer/releases/tag/release-3.2.4


## Linux

If your distribution does not provide recent versions of SDL3, SDL3_image and SDL3_mixer
you can build them from source. Replace <SDL3_SOURCE>, <SDL3_IMAGE_SOURCE> and
<SDL3_MIXER_SOURCE> with the directories of the source distributions of the libraries, and
<SDL3_PREFIX> with the directory where you want to install them (they can all be installed
in the same directory). SDL3 needs some development packages, listed in docs/README-linux.md
in the SDL3 source code. In Ubuntu or Debian:

```
    sudo apt-get install build-essential git pkg-config cmake ninja-build \
        libasound2-dev libpulse-dev libaudio-dev libfribidi-dev libjack-dev libsndio-dev \
        libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev \
        libxss-dev libxtst-dev libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev \
        libgles2-mesa-dev libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev \
        libthai-dev libusb-1.0-0-dev libpipewire-0.3-dev libwayland-dev libdecor-0-dev \
        liburing-dev
```

Then build and install the libraries (SDL3 first, since the others need it):

```
    cmake -S <SDL3_SOURCE> -B build-sdl3 -G Ninja -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=<SDL3_PREFIX> -DSDL_TESTS=OFF -DSDL_EXAMPLES=OFF
    cmake --build build-sdl3
    cmake --install build-sdl3

    cmake -S <SDL3_IMAGE_SOURCE> -B build-sdl3_image -G Ninja -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=<SDL3_PREFIX> -DCMAKE_PREFIX_PATH=<SDL3_PREFIX> \
        -DSDLIMAGE_SAMPLES=OFF -DSDLIMAGE_TESTS=OFF
    cmake --build build-sdl3_image
    cmake --install build-sdl3_image

    cmake -S <SDL3_MIXER_SOURCE> -B build-sdl3_mixer -G Ninja -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=<SDL3_PREFIX> -DCMAKE_PREFIX_PATH=<SDL3_PREFIX> \
        -DSDLMIXER_EXAMPLES=OFF -DSDLMIXER_TESTS=OFF
    cmake --build build-sdl3_mixer
    cmake --install build-sdl3_mixer
```

To build the game, replace <SDL3_PREFIX>, <SDL3_IMAGE_PREFIX> and <SDL3_MIXER_PREFIX> with the
installation prefix of the SDL3, SDL3_image and SDL3_mixer libraries, respectively (for
example: /opt/sdl/sdl3-3.4.20 or /usr/local):

```
    git clone <repository> retro-fred
    cd retro-fred
    mkdir build
    cd build
    cmake '-DCMAKE_PREFIX_PATH=<SDL3_PREFIX>;<SDL3_IMAGE_PREFIX>;<SDL3_MIXER_PREFIX>' ..
    cmake --build .
```

To run, from the `retro-fred/build` directory:

```
    ./cpp/retro-fred
```

Note that the executable will expect to find subdirectories sprites and sounds in the
current working directory.

Use option --help to get a list of configuration options.

### CMake presets / IDE setup (VSCode, CLion, etc.)

The repository ships a `CMakePresets.json` with a `linux` configure preset (generates
`compile_commands.json`, which is what most editors use for C++ IntelliSense/indexing). It
does not hardcode `CMAKE_PREFIX_PATH`, since that is specific to where you installed SDL on
your machine. Instead, create a `CMakeUserPresets.json` next to it (already gitignored, not
meant to be committed) with your own prefix paths:

```json
{
    "version": 3,
    "configurePresets": [
        {
            "name": "linux-local",
            "inherits": "linux",
            "cacheVariables": {
                "CMAKE_PREFIX_PATH": "<SDL3_PREFIX>;<SDL3_IMAGE_PREFIX>;<SDL3_MIXER_PREFIX>"
            }
        }
    ]
}
```

Then configure and build with:

```
    cmake --preset linux-local
    cmake --build --preset linux
```

In VSCode with the CMake Tools extension installed, the presets above show up directly in
the CMake preset picker (status bar, or Command Palette → "CMake: Select Configure Preset"),
and compile_commands.json-backed IntelliSense will work without any further configuration.
`windows-x64`/`windows-x86` (Visual Studio 2026, which needs CMake 4.2 or newer),
`windows-vs2022-x64`/`windows-vs2022-x86` (Visual Studio 2022) and `macos` presets are also
provided, following the same `CMakeUserPresets.json` pattern for `CMAKE_PREFIX_PATH` where
needed.


## Android

The Android build needs the source distributions of SDL3, SDL3_image and SDL3_mixer
(SDL 3.4.20, SDL3_image 3.4.8 and SDL3_mixer 3.2.4 were tested), which are compiled from
source as part of the Gradle build. The SDL3 sources include the Java glue code
(`android/app/src/main/java/org/libsdl/app`), which must come from the same SDL3 release as
the native library; when upgrading SDL3, copy the files from
`<SDL3_SOURCE>/android-project/app/src/main/java/org/libsdl/app` and
`<SDL3_SOURCE>/android-project/app/proguard-rules.pro`.

The AAudio buffer size patch that was required with SDL2 does not apply to SDL3, which sizes
the audio buffer itself. Audio on Android has not been verified yet; if there is crackling or
lag, try the SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES and SDL_HINT_ANDROID_LOW_LATENCY_AUDIO hints.

Set up the links to the source code of the SDL3 libraries in the android directory,
and an internal link to the assets directory from the android/app/src/main directory
(<SDL3_SOURCE>, <SDL3_IMAGE_SOURCE> and <SDL3_MIXER_SOURCE> are the directories of the
source distributions of the SDL3, SDL3_image and SDL3_mixer libraries):

```
    ./android/configure --sdl=<SDL3_SOURCE> \
                        --sdl-image=<SDL3_IMAGE_SOURCE> \
                        --sdl-mixer=<SDL3_MIXER_SOURCE>
```

Now the build can be started using the gradle tool included in the android directory.
JAVA_HOME must point to a JDK 17 or newer that includes the compiler (the JBR bundled with
Android Studio works),
and ANDROID_HOME to the Android SDK directory:

```
    JAVA_HOME=<JAVA_HOME> ANDROID_HOME=<ANDROID_HOME> ./android/gradlew build
```

The APKs with the packages will be available in the android/app/build/outputs/apk
directory. The wrapper downloads Gradle 9.9.2, and the build uses version 9.5.2 of the
Android Gradle plugin.

Android SDK versions (see File|Settings, Languages & Frameworks|Android SDK from Android
studio):

  SDK Tools:
    Android SDK Build-Tools 36
      36.0.0
    NDK
      28.2.13676358
    CMake
      3.22.1
    Android Emulator 37.3.10
    Android SDK Platform-Tools 37.0.3

  SDK Platforms:
    Android 17.0
      Android SDK Platform 37.0
      Google APIs Intel x86_64 Atom System Image

To build the project using Android Studio:

  * In Android Studio: File|Open..., then open the android-project
    folder

  * Select Build | Make Project


## Windows - Visual Studio

The continuous integration build uses Visual Studio 2026 and CMake 4.x. CMake 4.2 or newer
is required for the "Visual Studio 18 2026" generator; for Visual Studio 2022 use
"Visual Studio 17 2022" instead.

Download the SDL3-devel-<version>-VC.zip, SDL3_image-devel-<version>-VC.zip and
SDL3_mixer-devel-<version>-VC.zip packages from the release pages of the libraries and unpack
them. Replace <SDL3_PREFIX>, <SDL3_IMAGE_PREFIX> and <SDL3_MIXER_PREFIX> with the directories
created by unpacking the .zip files, respectively (for example C:\SDL3\SDL3-3.4.20)

```
    git clone <repository> retro-fred
    cd retro-fred
    mkdir build
    cd build
    mkdir win64
    cd win64
    cmake -G "Visual Studio 18 2026" -A x64 \
        "-DCMAKE_PREFIX_PATH=<SDL3_PREFIX>;<SDL3_IMAGE_PREFIX>;<SDL3_MIXER_PREFIX>" ../..
    cmake --build . --config Release
    cpack -G ZIP --config .\CPackConfig.cmake
```

And for the 32 bit build:

```
    cd ..\..\build
    mkdir win32
    cd win32
    cmake -G "Visual Studio 18 2026" -A Win32 \
        "-DCMAKE_PREFIX_PATH=<SDL3_PREFIX>;<SDL3_IMAGE_PREFIX>;<SDL3_MIXER_PREFIX>" ../..
    cmake --build . --config Release
    cpack -G ZIP --config .\CPackConfig.cmake
```

## Windows - mingw-w64: cross compiling from Linux

Replace <SDL3_PREFIX>, <SDL3_IMAGE_PREFIX> and <SDL3_MIXER_PREFIX> with the installation
prefix of the SDL3, SDL3_image and SDL3_mixer libraries for the x86_64 target. For example:
/opt/sdl3/3.4.20/x86_64-w64-mingw32 and /opt/sdl3_image/3.4.8/x86_64-w64-mingw32. You can
use the SDL3-devel-<version>-mingw.tar.gz, SDL3_image-devel-<version>-mingw.tar.gz and
SDL3_mixer-devel-<version>-mingw.tar.gz binary distributions from the release pages.

Replace <MINGW_TOOLCHAIN> with the path of the CMake toolchain file
for the mingw install. For example:
/opt/mingw/14.0.0/x86_64-w64-mingw32/lib/cmake/x86_64-w64-mingw32.cmake

Example toolchain file for mingw:

```
    set(CMAKE_SYSTEM_NAME Windows)
    set(TOOLCHAIN_PATH /opt/mingw/14.0.0)
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
    cmake '-DCMAKE_PREFIX_PATH=<SDL3_PREFIX>;<SDL3_IMAGE_PREFIX>;<SDL3_MIXER_PREFIX>' \
        -DCMAKE_TOOLCHAIN_FILE=<MINGW_TOOLCHAIN> -G Ninja ../..
    cmake --build .
```

TODO: you should be able to run cpack to create a zip file, but it
currently fails to archive all the required .dlls:

```
    cpack -G ZIP --config ./CPackConfig.cmake
```

## MacOS X

The game can be deployed to macOS 10.13 and newer, which is the minimum version supported by
SDL3. Building SDL3 requires Xcode 12.2 or newer.

First you need to build and install the SDL3, SDL3_image and SDL3_mixer libraries from their
source distributions. Replace <SDL3_SOURCE>, <SDL3_IMAGE_SOURCE> and <SDL3_MIXER_SOURCE> with the
directories of the source distributions, and <SDL3_PREFIX> with the directory where you want to
install the libraries (for example /usr/local/sdl, or a directory inside your home directory
if you do not want to use sudo):

```
    cmake -S <SDL3_SOURCE> -B build-sdl3 -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13 -DCMAKE_INSTALL_PREFIX=<SDL3_PREFIX>
    cmake --build build-sdl3
    cmake --install build-sdl3

    cmake -S <SDL3_IMAGE_SOURCE> -B build-sdl3_image -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13 -DCMAKE_INSTALL_PREFIX=<SDL3_PREFIX> \
        -DCMAKE_PREFIX_PATH=<SDL3_PREFIX> -DSDLIMAGE_SAMPLES=OFF -DSDLIMAGE_TESTS=OFF
    cmake --build build-sdl3_image
    cmake --install build-sdl3_image

    cmake -S <SDL3_MIXER_SOURCE> -B build-sdl3_mixer -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13 -DCMAKE_INSTALL_PREFIX=<SDL3_PREFIX> \
        -DCMAKE_PREFIX_PATH=<SDL3_PREFIX> -DSDLMIXER_EXAMPLES=OFF -DSDLMIXER_TESTS=OFF
    cmake --build build-sdl3_mixer
    cmake --install build-sdl3_mixer
```

The binaries are built for the architecture of the machine. To build universal binaries (Intel
and Apple silicon), add `"-DCMAKE_OSX_ARCHITECTURES=arm64;x86_64"` to all the cmake
invocations (including the ones to build the game).

Then build the game, using the same prefix for the libraries:

```
    git clone <repository> retro-fred
    cd retro-fred
    mkdir build
    cd build
    cmake -DCMAKE_PREFIX_PATH=<SDL3_PREFIX> ..
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
    cmake -DCMAKE_PREFIX_PATH=<SDL3_PREFIX> -G Xcode ..
    cmake --build . --config Release
    cpack -G Bundle --config ./CPackConfig.cmake
```

Note that the generated .dmg file and the .app bundle inside are not signed, so they will
not run if you transfer them to a different machine. To be able to run in a different
machine you need to copy the .app directory to a separate directory, then use `xattr -c
Retro-Fred.app` to clear the attributes, which should allow the executable to run.

## iOS

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
