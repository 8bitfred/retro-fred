#pragma once

#include <filesystem>

struct Config {
    Config(int argc, char *argv[]);

    static constexpr int PIXELS_PER_CHAR = 8;
    static constexpr int CELL_WIDTH = 4;
    static constexpr int CELL_HEIGHT = 5;

    std::uint32_t ticks_per_frame = 1000 / 6;
    bool virtual_controller = false;
    int logical_width = 256, logical_height = 192;
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
    int level = 1;
};
