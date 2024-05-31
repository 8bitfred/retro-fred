#include "Config.hpp"
#include <string_view>
#include <iostream>
#include <cstdlib>

namespace {
    const char *usage =
        "usage: fred [--help]\n"
        "            [--level LEVEL] [--debug-map] [--debug-keys]\n"
        "            [--full-screen] [--full-map] [--minimap-exit]\n"
        "            [--infinite-ammo] [--infinite-power]\n"
        "            [--boxes] [--fps FPS] [--virtual-controller]\n"
        "            [--power-with-level] [--bullets-with-level]\n"
        "            [--replenish-power] [--replenish-bullets]\n"
        "            [--minimap-tracker]\n"
        "\n"
        "    --help    Show this message\n"
        "    --level LEVEL\n"
        "              Start at the given level.\n"
        "    --debug-map\n"
        "              Use debug map, use debug levels for\n"
        "              creatures and objects\n"
        "    --debug-keys\n"
        "              Enable debug keys.\n"
        "    --full-screen\n"
        "              Enable full screen mode\n"
        "    --full-map\n"
        "              Create a window with the whole map\n"
        "    --minimap-exit\n"
        "              Show map exit in minimap\n"
        "    --infinite-ammo\n"
        "              Unlimited number of bullets\n"
        "    --infinite-power\n"
        "              Unlimited power\n"
        "    --boxes   Show sprite bound and hit boxes\n"
        "    --fps FPS\n"
        "              Set frames per second to FPS.\n"
        "              Defaults to 6.\n"
        "    --virtual-controller\n"
        "              Enable virtual controller.\n"
        "    --power-with-level\n"
        "              Change the amount that the power object\n"
        "              increases the power with the level.\n"
        "    --bullets-with-level\n"
        "              Change the max number of bullets with the\n"
        "              level.\n"
        "    --replenish-power\n"
        "              Refill the power when a level is completed.\n"
        "    --replenish-bullets\n"
        "              Refill the bullets when a level is completed.\n"
        "    --minimap-tracker\n"
        "              Show position of Fred in minimap.\n"
        "\n"
        "\n"
        "keybindings during gameplay:\n"
        "\n"
        "    left, right   Move left and right\n"
        "    up, down      Jump/move up and move down\n"
        "    space         Fire\n"
        "    Ctrl+Q        Quit\n"
        "    LShift+arrows Move window around the map\n"
        "    LShift+F      Move Fred to the center of the window.\n"
        "    LShift+R      Reset window position to Fred position.\n"
        "    LShift+H      Move Fred to the exit\n"
        "    LShift+O      Move exit to the left\n"
        "    LShift+P      Move exit to the right\n"
        "    LShift+K      Force game over sequence\n"
        "    LShift+M      Show mini-map from current position.\n"
        "\n";
}

Config::Config(int argc, char *argv[])
{
#ifdef __ANDROID__
    virtual_controller = true;
#endif
    for (int i = 1; i < argc; ++i)
    {
        std::string_view svarg(argv[i]);
        if (svarg == "--help")
        {
            std::cout << usage;
            std::exit(0);
        }
        else if (svarg == "--level")
        {
            ++i;
            if (i >= argc)
            {
                std::cerr << "missing argument for option --level" << std::endl;
                std::exit(2);
            }
            level = std::atoi(argv[i]);
        }
        else if (svarg == "--debug-map")
            debug_map = true;
        else if (svarg == "--debug-keys")
            debug_keys = true;
        else if (svarg == "--full-screen")
            full_screen = true;
        else if (svarg == "--full-map")
            max_resolution = true;
        else if (svarg == "--minimap-exit")
            minimap_exit = true;
        else if (svarg == "--infinite-ammo")
            infinite_ammo = true;
        else if (svarg == "--infinite-power")
            infinite_power = true;
        else if (svarg == "--boxes")
            boxes = true;
        else if (svarg == "--fps")
        {
            ++i;
            if (i >= argc)
            {
                std::cerr << "missing argument for option --fps" << std::endl;
                std::exit(2);
            }
            auto fps = std::atof(argv[i]);
            ticks_per_frame = static_cast<unsigned>(1000 / fps);
        }
        else if (svarg == "--virtual-controller")
            virtual_controller = true;
        else if (svarg == "--power-with-level")
            set_power_with_level = true;
        else if (svarg == "--bullets-with-level")
            set_bullets_with_level = true;
        else if (svarg == "--replenish-power")
            replenish_power = true;
        else if (svarg == "--replenish-bullets")
            replenish_bullets = true;
        else if (svarg == "--minimap-tracker")
            minimap_tracker = true;
        else
        {
            std::cerr << "unknown option: " << svarg << std::endl;
            std::cerr << usage << std::endl;
            std::exit(2);
        }
    }
}
