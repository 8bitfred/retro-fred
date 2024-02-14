

Linux
-----

Prerequisities: SDL and SDL_image libraries:

    https://github.com/libsdl-org/SDL/releases/tag/release-2.30.0
    https://github.com/libsdl-org/SDL_image/releases

Build instructions:

Replace <SDL2_PREFIX> and <SDL2_IMAGE_PREFIX> with the installation prefix of the SDL and
SDL_image libraries, respectively (for example: /opt/sdl2/2.28.3 or /usr/local)

```
    git clone <repository> fred
    cd fred
    mkdir build
    cd build
    CMAKE_PREFIX_PATH=<SDL2_PREFIX>:<SDL2_IMAGE_PREFIX> cmake ..
    cmake --build .
```

To run, from the `fred/build` directory:

```
    fred
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

