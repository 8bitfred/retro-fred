#pragma once

#include <filesystem>
#include <string_view>
#include <vector>

struct Config {
    Config(int argc, char *argv[]);
    void load(std::filesystem::path config_path);
    void save(std::filesystem::path config_path) const;

    static constexpr int PIXELS_PER_CHAR = 8;
    static constexpr int CELL_WIDTH = 4;
    static constexpr int CELL_HEIGHT = 5;
    static constexpr int SCOREBOARD_WIDTH = 7;

    std::uint32_t ticks_per_frame = 1000 / 6;
    bool virtual_controller = false;
    int logical_width = 256, logical_height = 192;
    bool user_window_size = false;
    int window_width = 0, window_height = 0;
    bool full_screen = false;
    bool max_resolution = false;
    int map_width = 33, map_height = 33;
    bool debug_map = false;
    bool debug_keys = false;
    bool infinite_ammo = false;
    bool infinite_power = false;
    bool boxes = false;
    bool minimap_exit = false;
    std::filesystem::path resource_path;
    bool set_power_with_level = false;
    bool set_bullets_with_level = false;
    bool replenish_power = false;
    bool replenish_bullets = false;
    bool minimap_tracker = false;
    int level = 1;

private:
    static std::vector<std::pair<std::string, bool Config::*>> getBoolFlagList();
    bool parseFlag(std::string_view svarg);
};
