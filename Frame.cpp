#include "Frame.hpp"
#include "Config.hpp"
#include <iostream>

Frame::Frame(Config const &cfg)
{
    // Screen coordinates of the visible part of the screen
    p1.x = MapPixelPos::PIXELS_PER_CHAR;
    p1.y = MapPixelPos::PIXELS_PER_CHAR;
    p2.x = cfg.window_width - SCORE_WIDTH * MapPixelPos::PIXELS_PER_CHAR;
    p2.y = cfg.window_height - MapPixelPos::PIXELS_PER_CHAR;

    auto visible_w = p2.x - p1.x;
    auto visible_h = p2.y - p1.y;

    auto fred_x = (visible_w - MapPixelPos::CELL_WIDTH_PIXELS) / 2;
    auto fred_y = (visible_h - MapPixelPos::CELL_HEIGHT_PIXELS) / 2;
    gFredPos.x = p1.x + fred_x;
    gFredPos.y = p1.y + fred_y;

    int ncells_x = (fred_x + MapPixelPos::CELL_WIDTH_PIXELS - 1) / MapPixelPos::CELL_WIDTH_PIXELS;
    int ncells_y = (fred_y + MapPixelPos::CELL_HEIGHT_PIXELS - 1) / MapPixelPos::CELL_HEIGHT_PIXELS;
    gFramePos.x = gFredPos.x - ncells_x * MapPixelPos::CELL_WIDTH_PIXELS;
    gFramePos.y = gFredPos.y - ncells_y * MapPixelPos::CELL_HEIGHT_PIXELS;

    std::cout << "Frame: x1=" << p1.x
              << " y1=" << p1.y
              << " x2=" << p2.x
              << " y2=" << p2.y << std::endl
              << " fred_x=" << gFredPos.x
              << " fred_y=" << gFredPos.y
              << " frame_pos_x=" << gFramePos.x
              << " frame_pos_y=" << gFramePos.y
              << std::endl;
}
