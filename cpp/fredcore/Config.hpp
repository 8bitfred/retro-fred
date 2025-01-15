#pragma once

#include <string>
#include <vector>
#include <cstdint>

struct Config {
    Config(int argc, char *argv[]);
    void load(std::string const& config_path);
    void save(std::string const& config_path) const;
    void normalMode();
    void classicMode();
    void resetCheats();

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
    bool max_resolution = false;
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
    static std::vector<std::pair<std::string, bool Config::*>> getBoolFlagList();
    bool parseFlag(std::string_view svarg);
};
