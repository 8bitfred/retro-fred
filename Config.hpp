#pragma once

#include <filesystem>

struct Config {
    Config(int argc, char *argv[]);

    static constexpr int PIXELS_PER_CHAR = 8;
    static constexpr int CELL_WIDTH = 4;
    static constexpr int CELL_HEIGHT = 5;

    int window_width = 256, window_height = 192;
    float scale_x = 5, scale_y = 5;
    int map_width = 33, map_height = 33;
    bool debug_map = false;
    bool infinite_ammo = false;
    bool infinite_power = false;
    bool boxes = false;
    std::filesystem::path resource_path;
};
