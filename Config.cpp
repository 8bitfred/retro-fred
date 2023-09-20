#include "Config.hpp"
#include <string_view>
#include <iostream>
#include <cstdlib>

namespace {
    const char *usage =
        "usage: fred [--help]\n"
        "            [--debug] [--fullmap]\n"
        "            [--infinite-ammo] [--infinite-power]\n"
        "            [--scale f.f]\n"
        "\n"
        "    [--help]    Show this message\n"
        "    [--debug-map]\n"
        "                Use debug map, use debug levels for\n"
        "                creatures and objects\n"
        "    [--debug-keys]\n"
        "                Enable debug keys.\n"
        "    [--fullmap] Create a window with the whole map\n"
        "    [--infinite-ammo]\n"
        "                Unlimited number of bullets\n"
        "    [--infinite-power]\n"
        "                Unlimited power\n"
        "    [--boxes]   Show sprite bound and hit boxes\n"
        "    [--scale f.f]\n"
        "                Set the scale of the SDL renderer.\n"
        "                Defaults to 5.\n"
        "\n"
        "\n"
        "keybindings during gameplay:\n"
        "\n"
        "    left, right   Move left and right\n"
        "    up, down      Jump/move up and move down\n"
        "    space         Fire\n"
        "    Ctrl+Q        Quit\n"
        "    LShift+arrows Move window around the map\n"
        "    LShift+H      Move Fred to the exit\n"
        "    LShift+F      Move Fred to the closes position\n"
        "    LShift+O      Move exit to the left\n"
        "    LShift+P      Move exit to the right\n"
        "\n";
}

Config::Config(int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        std::string_view svarg(argv[i]);
        if (svarg == "--help")
        {
            std::cout << usage;
            std::exit(0);
        }
        else if (svarg == "--debug-map")
            debug_map = true;
        else if (svarg == "--debug-keys")
            debug_keys = true;
        else if (svarg == "--fullmap")
        {
            window_width = (map_width + 2) * CELL_WIDTH * PIXELS_PER_CHAR;
            window_height = (map_height + 2) * CELL_HEIGHT * PIXELS_PER_CHAR;
            scale_x = scale_y = 1;
        }
        else if (svarg == "--infinite-ammo")
            infinite_ammo = true;
        else if (svarg == "--infinite-power")
            infinite_power = true;
        else if (svarg == "--boxes")
            boxes = true;
        else if (svarg == "--scale")
        {
            ++i;
            if (i >= argc)
            {
                std::cerr << "missing argument for option --scale" << std::endl;
                std::exit(2);
            }
            scale_x = scale_y = static_cast<float>(std::atof(argv[i]));
        }
        else if (svarg == "--fps")
        {
            ++i;
            if (i >= argc)
            {
                std::cerr << "missing argument for option --fps" << std::endl;
                std::exit(2);
            }
            auto fps = std::atoi(argv[i]);
            ticks_per_frame = 1000 / fps;
        }
        else
        {
            std::cerr << "unknown option: " << svarg << std::endl;
            std::cerr << usage << std::endl;
            std::exit(2);
        }
    }
}
