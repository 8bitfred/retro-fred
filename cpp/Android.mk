LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/../SDL_image

# Add your application source files here...
LOCAL_SRC_FILES := AcidDrop.hpp Application.hpp Bullet.hpp Chameleon.hpp Config.hpp Coord.hpp Fred.hpp GameEvent.hpp Game.hpp GameMap.hpp Ghost.hpp MultiDirEnemy.hpp Mummy.hpp Object.hpp Rat.hpp sdl.hpp Signal.hpp Skeleton.hpp Smoke.hpp SoundID.hpp SoundManager.hpp Sprite.hpp TextureID.hpp TextureManager.hpp Tomb.hpp Vampire.hpp Window.hpp AcidDrop.cpp Application.cpp Bullet.cpp Chameleon.cpp Config.cpp Fred.cpp Game.cpp GameEvent.cpp GameMap.cpp Ghost.cpp main.cpp MultiDirEnemy.cpp Mummy.cpp Object.cpp Rat.cpp Skeleton.cpp Smoke.cpp SoundManager.cpp Sprite.cpp TextureManager.cpp Tomb.cpp Vampire.cpp Window.cpp DisplayConfig.hpp DisplayConfig.cpp Controller.hpp Controller.cpp

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image

LOCAL_CPPFLAGS += -fexceptions -frtti -std=c++20
LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid

include $(BUILD_SHARED_LIBRARY)
