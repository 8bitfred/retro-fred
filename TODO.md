## 2024-05-28

* In Android: ignore "Back" event if there is a touch event on the virtual controller. We
  are trying to avoid the current situation in which the "Back" event gets accidentally
  triggered when the user is moving the finger over the touch controller.

## 2024-05-27

* Add menu for configuration (including cheats), instead of relying on command line flags

* Real full screen mode: support using the whole screen in mobile devices
