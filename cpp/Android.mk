LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/../SDL_image

# Add your application source files here...
LOCAL_SRC_FILES := \
    sdl.hpp \
    Config.cpp Config.hpp \
    TextureID.hpp TextureManager.cpp TextureManager.hpp \
    SoundID.hpp SoundManager.cpp SoundManager.hpp \
    Label.hpp LabelTable.hpp LabelTable.cpp \
    DisplayConfig.cpp DisplayConfig.hpp \
    Coord.hpp \
    GameMap.cpp GameMap.hpp \
    GameWindow.hpp \
    Window.cpp Window.hpp \
    Sprite.cpp Sprite.hpp \
    GameEvent.hpp GameEvent.cpp \
    Game.cpp Game.hpp \
    \
    Controller.cpp Controller.hpp \
    GameRunner.cpp GameRunner.hpp \
    AcidDrop.hpp AcidDrop.cpp \
    Fred.cpp Fred.hpp \
    Rat.cpp Rat.hpp \
    Ghost.cpp Ghost.hpp \
    Chameleon.cpp Chameleon.hpp \
    Mummy.cpp Mummy.hpp \
    MultiDirEnemy.cpp MultiDirEnemy.hpp \
    Vampire.cpp Vampire.hpp \
    Skeleton.cpp Skeleton.hpp \
    Bullet.cpp Bullet.hpp \
    Smoke.cpp Smoke.hpp \
    Object.cpp Object.hpp \
    Tomb.cpp Tomb.hpp \
    Application.cpp Application.hpp \
    \
    main.cpp

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image

LOCAL_CPPFLAGS += -fexceptions -frtti -std=c++20
LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid

include $(BUILD_SHARED_LIBRARY)
