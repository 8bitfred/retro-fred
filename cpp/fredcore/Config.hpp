#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <optional>

// How the game window (the screen of the game) is laid out in the window
enum class MapMode
{
    // The original 256x192 screen, scaled to fit the window
    NORMAL,
    // The whole map, in a window with the size of the map
    FULL_MAP,
    // The game window takes the whole window, and changes size with it
    FULL_SIZE,
};

struct Config {
    Config(int argc, char *argv[]);
    void load(std::string const& config_path);
    void save(std::string const& config_path) const;
    void normalMode();
    void classicMode();
    void resetCheats();
    // The full map and full size modes are mutually exclusive. Call these after changing
    // the corresponding mode, to disable the other one.
    void fullMapModeChanged();
    void fullSizeModeChanged();
    MapMode getMapMode() const
    {
        return max_resolution ? MapMode::FULL_MAP : full_size ? MapMode::FULL_SIZE : MapMode::NORMAL;
    }

    static constexpr int PIXELS_PER_CHAR = 8;
    static constexpr int CELL_WIDTH = 4;
    static constexpr int CELL_HEIGHT = 5;
    static constexpr int SCOREBOARD_WIDTH = 7;

    std::uint32_t ticks_per_frame = 1000 / 6;
    bool virtual_controller = false;
    bool back_button = false;
    bool quit_option = true;
    int logical_width = 256, logical_height = 192;
    bool user_window_size = false;
    int window_width = 0, window_height = 0;
    bool full_screen = false;
    // Full map mode: show the whole map (as far as the window allows) instead of the
    // original 256x192 screen. Can be changed while the game is running.
    bool max_resolution = false;
    // Full size mode: the game window takes the whole window, showing as much of the map as
    // fits, instead of the original 256x192 screen. Can be changed while the game is
    // running. Ignored if the full map mode is enabled. In mobile devices (see
    // fixed_window_size) it is enabled by default and selected by the normal mode (and
    // disabled by the classic mode).
    bool full_size = false;
    // The size of the window is determined by the platform and cannot be changed by the
    // application (e.g. a phone screen). In full map mode the game is scaled to fill the
    // window, instead of changing the window size to fit the map.
    bool fixed_window_size = false;
    bool high_dpi = false;
    int map_width = 33, map_height = 33;
    bool debug_map = false;
    bool debug_keys = false;
    bool infinite_ammo = false;
    bool default_infinite_ammo = false;
    bool infinite_power = false;
    bool default_infinite_power = false;
    bool boxes = false;
    bool minimap_exit = true;
    std::string resource_path;
    bool set_power_with_level = true;
    bool set_bullets_with_level = true;
    bool refill_power = true;
    bool refill_bullets = true;
    bool minimap_tracker = true;
    int level = 1;

private:
    // Value of the full size mode selected in the command line, if any. It takes precedence
    // over the saved configuration.
    std::optional<bool> cli_full_size;

    static std::vector<std::pair<std::string, bool Config::*>> getBoolFlagList();
    bool parseFlag(std::string_view svarg);
};
