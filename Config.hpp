#pragma once


struct Config {
    Config();

    static constexpr int PIXELS_PER_CHAR = 8;
    static constexpr int CELL_WIDTH = 4;
    static constexpr int CELL_HEIGHT = 5;

    int window_width, window_height;
    float scale_x, scale_y;
    int map_width, map_height;
    int map_x, map_y;
};
