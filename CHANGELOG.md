# 0.4.2

* Added command line option to select window size on initialization.

* Fixed audio clipping: playing of some of the sound effects was getting interrupted at
  the beginning of the game loop.

* Refactored audio support: use SDL_mixer library to allow multiple sound effects to play
  simultaneously.

* Set configuration defaults to easier values. Store version number in configuration file.
  Store true or false for all supported options in configuration file.

* Separate the in-game menu between configuration options, and cheats (POKEs).
  Configuration changes from the in-game menu affect the global configuration (instead of
  just for the current game). Cheats get reset to their default values (which can only be
  set from te command line) at the beginning of every game. 

# 0.4.1

* Removed unnecessary "low latency audio" feature from Android manifest. Some users are
  getting an "incompatible device" error when they try to download the app from Google
  Play, and this may be the fix for it.

# 0.4.0

* Refactored "full-map" mode: moved the initialization of the SDL window to the
  DisplayConfig class; added support for scaling the window frame and scoreboard in
  full-map mode.

* Bugfix: add 200 points every time an enemy (ghost, mummy, vampire or skeleton) is hit by
  a bullet.

* Bugfix: intentionally mis-align the current score and the high score, as the original
  game did it. The high score numer is one pixel to the left of the current score.

# 0.3.1

* Use custom package name in Android app, instead of the default app.libsdl.org. In
  Android, check version names in build.gradle, in addition to AndroidManifest.xml.

* Bugfix: fix "seeping" of fonts by ensuring that there is at least 1 empty pixel on every
  side of every character in the font sprite sheet.

# 0.3.0

* Added minimap tracker (configurable): when enabled, it shows the position of fred in the
  minimap with a red dot.

* Added support for saving and restoring high scores. High scores get saved in the
  user-and-app specific path, as reported by SDL_GetPrefPath(). In Linux this is
  `~/.local/share/8bitfred/Retro-Fred/high_scores.tbl`.

* Added option menu for configuration. The new menu can be used by the user to set up game
  configuration flags and cheats, like infinite power or ammo, the minimap tracker, etc.

* Fixed bug in "manual kill" mode: when, with --debug-keys enabled, Shift+D is pressed
  Fred cycled through one color too many.

* Added version attributes to AndroidManifest.xml. Added checks to make sure that the
  Android version name matches the version specified in CMakeLists.txt. Added checks to
  make sure that the Android version code matches the number of versions in CHANGELOG.md.
  Run checks on every build that runs from Linux

* Added in-game menu. Menu contains some of the same options as the configuration menus,
  and it adds a few cheats. Enabling cheats prevents the score from being added to the
  high scores.

* Added option to delete the last letter when entering high scores. If the user selects
  symbol '<' the last letter gets deleted.

* Bugfix: fix reference to element of empty sprite list after Fred sprite has been destroyed,
  during the game over sequence. This was causing crashes in Windows, but not in other
  builds.

* Bugfix: when mummies get shot make them disappear and respawn in a different location,
  instead of killing them.

* Bugfix: increase power by 2 units when starting a new level, per the original game.

# 0.2.0

* Initial release

* Supports game in Linux, Windows and Android. It builds for MacOS X, but there seems to
  be some problem when trying to run the code in a different machine that the one where
  the code was built. This is probably due to the bundle not being signed
