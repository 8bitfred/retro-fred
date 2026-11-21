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
        "            [--full-screen] [--full-map] [--full-size]\n"
        "            [--minimap-exit]\n"
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
        "    --full-size\n"
        "              The game window takes the whole window\n"
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
        "    --back-button\n"
        "              Enable back button in the virtual controller.\n"
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
        "    --high-dpi\n"
        "              Enable high DPI mode.\n"
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
    fixed_window_size = true;
    // From Android 15 (target SDK 35) apps are laid out edge-to-edge, behind the status and
    // navigation bars, which would cover (and crop) the game. Use a full screen (immersive)
    // window, which hides the system bars.
    full_screen = true;
#endif
#ifdef RETRO_FRED_IOS
    virtual_controller = true;
    fixed_window_size = true;
    back_button = true;
    quit_option = false;
    high_dpi = true;
#endif
    // The full size mode is the default in mobile devices
    if (fixed_window_size)
        full_size = true;
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
        else if (svarg == "--full-size" || svarg == "--no-full-size")
        {
            full_size = (svarg == "--full-size");
            cli_full_size = full_size;
        }
        else if (svarg == "--infinite-ammo")
            infinite_ammo = default_infinite_ammo = true;
        else if (svarg == "--infinite-power")
            infinite_power = default_infinite_power = true;
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
        else if (svarg == "--back-button")
            back_button = true;
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
        else if (svarg == "--high-dpi")
            high_dpi = true;
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

void Config::load(std::string const& config_path)
{
    // The --full-map command-line option takes precedence over the saved configuration
    auto const cli_max_resolution = max_resolution;
    std::ifstream file(config_path);
    for (std::string line; std::getline(file, line);)
    {
        auto svarg = strip(line);
        // for now we just ignore version settings
        if (svarg.starts_with("version "))
            continue;
        parseFlag(svarg);
    }
    max_resolution = max_resolution || cli_max_resolution;
    if (cli_full_size)
        full_size = *cli_full_size;
}

void Config::save(std::string const& config_path) const
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

void Config::normalMode()
{
    minimap_tracker = true;
    minimap_exit = true;
    refill_power = true;
    refill_bullets = true;
    set_power_with_level = true;
    set_bullets_with_level = true;
    // In mobile devices the full size mode is part of the normal mode. In the other
    // platforms it is a separate option.
    if (fixed_window_size)
    {
        full_size = true;
        fullSizeModeChanged();
    }
}

void Config::classicMode()
{
    minimap_tracker = false;
    minimap_exit = false;
    refill_power = false;
    refill_bullets = false;
    set_power_with_level = false;
    set_bullets_with_level = false;
    if (fixed_window_size)
        full_size = false;
}

void Config::fullMapModeChanged()
{
    if (max_resolution)
        full_size = false;
}

void Config::fullSizeModeChanged()
{
    if (full_size)
        max_resolution = false;
}

void Config::resetCheats()
{
    infinite_ammo = default_infinite_ammo;
    infinite_power = default_infinite_power;
}

std::vector<std::pair<std::string, bool Config::*>> Config::getBoolFlagList()
{
    // Flags in this list will be saved and loaded. We restrict it to the flags that can
    // be set with the menu, so that users can change their values after they are loaded.
    static std::vector<std::pair<std::string, bool Config::*>> flag_list = {
        {"minimap-exit", &Config::minimap_exit},
        {"minimap-tracker", &Config::minimap_tracker},
        {"power-with-level", &Config::set_power_with_level},
        {"bullets-with-level", &Config::set_bullets_with_level},
        {"refill-power", &Config::refill_power},
        {"refill-bullets", &Config::refill_bullets},
        {"full-map", &Config::max_resolution},
        {"full-size", &Config::full_size},
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
