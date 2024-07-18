#include "Config.hpp"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <charconv>
#include "build_config.hpp"

namespace {
    const char *usage =
        "usage: fred [--help]\n"
        "            [--level LEVEL] [--debug-map] [--debug-keys]\n"
        "            [--full-screen] [--full-map] [--minimap-exit]\n"
        "            [--infinite-ammo] [--infinite-power]\n"
        "            [--boxes] [--fps FPS] [--virtual-controller]\n"
        "            [--power-with-level] [--bullets-with-level]\n"
        "            [--refill-power] [--refill-bullets]\n"
        "            [--minimap-tracker] [--window-size WIDTHxHEIGHT]\n"
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
        "    --refill-power\n"
        "              Refill the power when a level is completed.\n"
        "    --refill-bullets\n"
        "              Refill the bullets when a level is completed.\n"
        "    --minimap-tracker\n"
        "              Show position of Fred in minimap.\n"
        "    --window-size WIDTHxHEIGHT\n"
        "              Select window size\n"
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
        else if (svarg == "--window-size")
        {
            ++i;
            if (i >= argc)
            {
                std::cerr << "missing argument for option --widow-size" << std::endl;
                std::exit(2);
            }
            auto wsize = std::string_view(argv[i]);
            auto xpos = wsize.find('x');
            if (xpos == wsize.npos)
            {
                std::cerr << "invalid argument: window size must be WIDTHxHEIGHT" << std::endl;
                std::exit(2);
            }
            std::from_chars(wsize.data(), wsize.data() + xpos, window_width);
            std::from_chars(wsize.data() + xpos + 1, wsize.data() + wsize.size(), window_height);
            user_window_size = true;
        }
        else if (svarg.starts_with("--") && !parseFlag(svarg.substr(2)))
        {
            std::cerr << "unknown option: " << svarg << std::endl;
            std::cerr << usage << std::endl;
            std::exit(2);
        }
    }
}

namespace {
    std::string_view strip(std::string_view in)
    {
        auto s = in.find_first_not_of(' ');
        if (s == in.npos)
            return {};
        auto e = in.find_last_not_of(' ');
        return in.substr(s, e - s + 1);
    }
}

void Config::load(std::filesystem::path config_path)
{
    std::ifstream file(config_path);
    for (std::string line; std::getline(file, line);)
    {
        auto svarg = strip(line);
        // for now we just ignore version settings
        if (svarg.starts_with("version "))
            continue;
        parseFlag(svarg);
    }
}

void Config::save(std::filesystem::path config_path) const
{
    std::ofstream file(config_path);
    file << "version " << RETRO_FRED_VERSION_STRING << std::endl;
    for (auto const &[name, ptr] : getBoolFlagList())
    {
        if (this->*ptr)
            file << name << std::endl;
        else
            file << "no-" << name << std::endl;
    }
}

std::vector<std::pair<std::string, bool Config::*>> Config::getBoolFlagList()
{
    // Flags in this list will be saved and loaded. We restrict it to the flags that can
    // be set with the menu, so that users can change their values after they are loaded.
    static std::vector<std::pair<std::string, bool Config::*>> flag_list = {
        {"minimap-exit", &Config::minimap_exit},
        {"infinite-ammo", &Config::infinite_ammo},
        {"infinite-power", &Config::infinite_power},
        {"power-with-level", &Config::set_power_with_level},
        {"bullets-with-level", &Config::set_bullets_with_level},
        {"refill-power", &Config::refill_power},
        {"refill-bullets", &Config::refill_bullets},
        {"minimap-tracker", &Config::minimap_tracker},
    };
    return flag_list;
}

bool Config::parseFlag(std::string_view svarg)
{
    for (auto const &[name, ptr]: getBoolFlagList())
    {
        if (svarg == name)
        {
            this->*ptr = true;
            return true;
        }
        else if (svarg.starts_with("no-") && svarg.substr(3) == name)
        {
            this->*ptr = false;
            return true;
        }
    }
    return false;
}