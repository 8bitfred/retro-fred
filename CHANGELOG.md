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

* Add version attributes to AndroidManifest.xml. Add checks to make sure that the Android
  version name matches the version specified in CMakeLists.txt. Add checks to make sure
  that the Android version code matches the number of versions in CHANGELOG.md. Run checks
  on every build that runs from Linux


# 0.2.0

* Initial release

* Supports game in Linux, Windows and Android. It builds for MacOS X, but there seems to
  be some problem when trying to run the code in a different machine that the one where
  the code was built. This is probably due to the bundle not being signed
