
# default values
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(build_config.hpp.in build_config.hpp @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_subdirectory(platform)
add_subdirectory(fredcore)
add_subdirectory(fredgame)
add_subdirectory(sdlbackend)

include(InstallRequiredSystemLibraries)

if(LINUX)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()

if(ANDROID)
    add_library(main main.cpp)
    target_link_libraries(main fredgame fredcore sdlbackend)
else()
    add_executable(retro-fred main.cpp)
    target_link_libraries(retro-fred fredgame fredcore sdlbackend)

    add_executable(autofred autofred.cpp)
    target_link_libraries(autofred fredgame fredcore sdlbackend)

    add_executable(sandbox
                   sandbox.cpp
                   Signal.hpp)
    target_link_libraries(sandbox fredcore sdlbackend)

    install(TARGETS
            retro-fred
            RUNTIME DESTINATION .)
endif()

if(WIN32)
    # Configure retro-fred as a win32 executable (not a console application)
    set_property(TARGET retro-fred PROPERTY WIN32_EXECUTABLE True)

    # Add resources with application icon
    target_sources(fredcore PUBLIC resources/retro-fred.rc resources/resource.h)
    target_sources(sandbox PRIVATE resources/retro-fred.rc resources/resource.h)

    # Create a console version of the fred app, to allow command-line interaction
    add_executable(retro-fred-console main.cpp)
    target_link_libraries(retro-fred-console
                          fredgame fredcore sdlbackend)

    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
    install(TARGETS
            retro-fred-console
            RUNTIME DESTINATION .)
    install(FILES
            $<TARGET_PROPERTY:SDL3::SDL3,IMPORTED_LOCATION>
            $<TARGET_PROPERTY:SDL3_image::SDL3_image,IMPORTED_LOCATION>
            $<TARGET_PROPERTY:SDL3_mixer::SDL3_mixer,IMPORTED_LOCATION>
            DESTINATION .)
endif()
