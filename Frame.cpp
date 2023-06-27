#include "Frame.hpp"
#include "Config.hpp"
#include <iostream>

// Game screen
//
// The game screen consists of a 1 characer frame on all sides of the screen, a game
// window where the map and characters are shown, and a scoreboard where the score and
// other game information is displayed. The scoreboard is 7 characters wide and goes from
// the top to the bottom of the screen. We allow the size of the screen to be
// configurable, so the size of the game window varies with the screen size. 
//
// In the original ZX Spectrum game the screen is 256x192 pixels (32x24 characters) and
// the game window is 192x176 pixels (24x22 characters):
//
//       |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |   8   |
//       |0|1|2|3|0|1|2|3|0|1|2|3|0|1|2|3|0|1|2|3|0|1|2|3|0|1|2|3|0|1|2|3|0|1|2|3|
//     --+-------+-------+-------+-------+-------+-------+-------+-------+-------+--
//     0 |F      |       |       |       |       |       |       |       |       |
//     1 |  S X X|X X X X|X X X X|X X X X|X X X X|X X X X|X X X X|X X X X|X      |
//   0 2 |  X    |       |       |       |       |       |    X  |       |X      |
//     3 |  X    |       |       |       |       |       |    X  |       |X      |
//     4 |  X    |       |       |       |       |       |    X  |       |X      |
//     --+-------+-------+-------+-------+-------+-------+-------+-------+-------+--
//     0 |  X    |       |       |       |       |       |    X X|X X X X|X      |
//     1 |  X    |       |       |       |       |       |    X  |M A P  |X      |
//   1 2 |  X    |       |       |       |       |       |    X  |       |X      |
//     3 |  X    |       |       |       |       |       |    X  |       |X      |
//     4 |  X    |       |       |       |       |       |    X  |       |X      |
//     --+-------+-------+-------+-------+-------+-------+-------+-------+-------+--
//     0 |  X    |       |       |       |       |       |    X  |       |X      |
//     1 |  X    |       |       |  OO   |       |       |    X  |       |X      |
//   2 2 |  X    |       |       |+ mmm  |       |       |    X X|X X X X|X      |
//     3 |  X    |       |       | ===== |       |       |    X P|O W E R|X      |
//     4 |  X    |       |       |  d d  |       |       |    X  |       |X      |
//     --+-------+-------+-------+-------+-------+-------+-------+-------+-------+--
//     0 |  X    |       |       |       |       |       |    X  |       |X      |
//     1 |  X    |       |       |       |       |       |    X  |       |X      |
//   3 2 |  X    |       |       |       |       |       |    X X|X X X X|X      |
//     3 |  X    |       |       |       |       |       |    X S|C O R E|X      |
//     4 |  X    |       |       |       |       |       |    X  |       |X      |
//     --+-------+-------+-------+-------+-------+-------+-------+-------+-------+--
//     0 |  X    |       |       |       |       |       |    X X|X X X X|X      |
//     1 |  X    |       |       |       |       |       |    X H|I G H  |X      |
//   4 2 |  X    |       |       |       |       |       |    X  |       |X      |
//     3 |  X    |       |       |       |       |       |    X X|X X X X|X      |
//     4 |  X X X|X X X X|X X X X|X X X X|X X X X|X X X X|X X X X|X X X X|X      |
//     --+-------+-------+-------+-------+-------+-------+-------+-------+-------+--
//
// Our main character, Fred, is always placed on the center of the screen. Local variable
// fred_pos contains the screen coordinates of Fred.
//
// We use the F point to determine which part of the map should be shown in the scren. We
// define the F point to be an integer number of cells to the left and to the top of Fred,
// such that the cell of the F point is always visible in the game window. Member variable
// gFrame stores the position of F in map coordinates (x, y, cx, cy). To move Fred around
// the map we just update the coordinates of F. Since Fred is always a given number of
// cells from F, Fred moves as F moves.
//
// The S point above is the top left corner of the screen. Member variable gFramePos
// contains the position of F with respect to S.
//
// Moving to the left:
//
//         START                STEP 1                STEP 2                STEP 3        
//   |   0   |   1   |     |   0   |   1   |     |   0   |   1   |     |   0   |   1   |  
//   |0|1|2|3|0|1|2|3|     |0|1|2|3|0|1|2|3|     |0|1|2|3|0|1|2|3|     |0|1|2|3|0|1|2|3|  
// --+-------+-------+   --+-------+-------+   --+-------+-------+   --+-------+-------+  
// 0 |       |F      |   0 |      F|       |   0 |    F  |       |   0 |  F    |       |  
// 1 |       |  S X X|   1 |       |S X X X|   1 |      S|X X X X|   1 |    S X|X X X X|  
// 2 |       |  X    |   2 |       |X      |   2 |      X|       |   2 |    X  |       |  
// 3 |       |  X    |   3 |       |X      |   3 |      X|       |   3 |    X  |       |
//     F.x=1 F.cx=0          F.x=0 F.cx=3          F.x=0 F.cx=2          F.x=0 F.cx=1
//
// When moving to the left a new column of blocks becomes visible on the left side when
// the cx coordinate of the left-most visible character in the frame is 3, in step 3, when
// F.cx is 1. When this happens we need to draw a new column of blocks at F.x. The value
// of F.cx when a new column becomes visible on the left side when moving left depends on
// the width of the game window. We store it in gNewLeftCol_cx.
//
// When the game starts we draw on the screen all the map blocks that have any portion of
// them visible on the game window. When the character moves we update the position of
// those blocks as necessary. As the character moves we may need to draw new blocks when
// new columns or rows become visible on the edges of the screen.
//
// Moving to the right:
//
//         START                STEP 1                STEP 2                STEP 3        
//   |   6   |   7   |     |   6   |   7   |     |   6   |   7   |     |   6   |   7   |   
//   |0|1|2|3|0|1|2|3|     |0|1|2|3|0|1|2|3|     |0|1|2|3|0|1|2|3|     |0|1|2|3|0|1|2|3|   
// --+-------+-------+   --+-------+-------+   --+-------+-------+   --+-------+-------+   
// 0 |       |       |   0 |       |       |   0 |       |       |   0 |       |       |   
// 1 |X X X X|X X X X|   1 |X X X X|X X X X|   1 |X X X X|X X X X|   1 |X X X X|X X X X|   
// 2 |    X  |       |   2 |      X|       |   2 |       |X      |   2 |       |  X    |   
// 3 |    X  |       |   3 |      X|       |   3 |       |X      |   3 |       |  X    |   
//     F.x=0 F.cx=0          F.x=0 F.cx=1          F.x=0 F.cx=2          F.x=0 F.cx=3
//
// When moving to the right a new column of blocks becomes visible on the right side when
// the cx coordinate of the right-most visible character in the frame is 0, in step 3,
// when F.cx is 3. When this happens we need to draw a new column of blocks at F.x+7.
// Similarly to gNewLeftCol_cx, the value of F.cx when a new column becomes visible on the
// right side when moving to the right is stored in gNewRightCol_cx. The offset, in number
// of cells, from F.x to the column that is added to the right is stored in
// gNewRightColOffset.
//
// The diagrams below show examples of different values of gNewLeftCol_cx, gNewRightCol_cx
// and gNewRightColOffset depending on the width of the game window. Note that there is a
// frame of 8 pixels around the game window, so the game window starts 8 pixels to the
// right of the edge of the screen. For each example we show the width of the game window,
// the position of the Fred character with respect to the left edge of the screen, and the
// values of gNewLeftCol_cx, gNewRightCol_cx and gNewRightColOffset.
//
//  |   0   |   1 ... 5   |   6   |  width of game window: 208 pixels (26 characters)
//  |0|1|2|3|0|1| ... |2|3|0|1|2|3|  screen position of Fred: 96 pixels
//  |F X X X|X X  ...  X X|X X X X|  gNewLeftCol_cx=2 gNewRightCol_cx=2
//  |X      |     ...     |      X|  gNewRightColOffset=7
//
//  |   0   |   1 ... 5   |   6   |  width of game window: 192 pixels (24 characters)
//  |0|1|2|3|0|1| ... |2|3|0|1|2|3|  screen position of Fred: 88 pixels
//  |F X X X|X X  ...  X X|X X X X|  gNewLeftCol_cx=1 gNewRightCol_cx=3
//  |  X    |     ...     |    X  |  gNewRightColOffset=7
//
//  |   0   |   1 ... 5   |   6   |  width of game window: 176 pixels (22 characters)
//  |0|1|2|3|0|1| ... |2|3|0|1|2|3|  screen position of Fred: 80 pixels
//  |F   X X|X X  ...  X X|X X X X|  gNewLeftCol_cx=0 gNewRightCol_cx=0
//  |    X  |     ...     |  X    |  gNewRightColOffset=6
//
//  |   0   |   1 ... 5   |   6   |  width of game window: 160 pixels (20 characters)
//  |0|1|2|3|0|1| ... |2|3|0|1|2|3|  screen position of Fred: 72
//  |      X|F X  ...  X X|X X X X|  gNewLeftCol_cx=3 gNewRightCol_cx=1
//  |      X|     ...     |X      |  gNewRightColOffset=5
//
// The same mechanism applies when the character moves up or down. Variables
// gNewTopRow_cy, gNewBottomRow_cy and gNewBottomRowOffset are used to draw new rows at
// top or the bottom of the screen when they become visible in the game window.
//
Frame::Frame(Config const &cfg)
{
    // Screen coordinates of the game window
    //   (in the example: top_left={8,8} bottom right={200,184})
    top_left.x = MapPixelPos::PIXELS_PER_CHAR;  // x2
    top_left.y = MapPixelPos::PIXELS_PER_CHAR;  // y1
    bottom_right.x = cfg.window_width - SCOREBOARD_WIDTH * MapPixelPos::PIXELS_PER_CHAR; // x1
    bottom_right.y = cfg.window_height - MapPixelPos::PIXELS_PER_CHAR;              // y2

    // Position of Fred: in the center of the screen, rounded down to a character
    //   (in the example: fred_pos.x = 88, fred_pos.y = 72)
    auto center_x = (top_left.x + bottom_right.x - MapPixelPos::CELL_WIDTH_PIXELS) / 2;
    ScreenPos fred_pos;
    fred_pos.x = round_down(center_x, MapPixelPos::PIXELS_PER_CHAR);
    auto center_y = (top_left.y + bottom_right.y - MapPixelPos::CELL_HEIGHT_PIXELS) / 2;
    fred_pos.y = round_down(center_y, MapPixelPos::PIXELS_PER_CHAR);
    // We want F to be an integer number of cells fom the left and top of the Fred sprite,
    // so we calculate how many cells Fred is from F
    //   (in the example gFredOffset_x = 3, gFredOffset_y = 2)
    gFredOffset_x = ceil_of_div(fred_pos.x - top_left.x, MapPixelPos::CELL_WIDTH_PIXELS);
    gFredOffset_y = ceil_of_div(fred_pos.y - top_left.y, MapPixelPos::CELL_HEIGHT_PIXELS);
    // Coordinates of F, with reference to the screen (S), based on the position of Fred
    // in the Screen
    //   (in the example: gFramePos.x = -8, gFramePos.y = -8)
    gFramePos.x = fred_pos.x - gFredOffset_x * MapPixelPos::CELL_WIDTH_PIXELS;
    gFramePos.y = fred_pos.y - gFredOffset_y * MapPixelPos::CELL_HEIGHT_PIXELS;
    // Note that gFramePos.x and gFramePos.y are always less than or equal to 0.

    // Value of gFrame.cx that triggers drawing a column of blocks on the left of the
    // screen, at gFrame.x. We first get the position of the first visible character on
    // the left side of the screen with respect to F. Then we figure out the value of
    // gFrame.cx when that falls on the last character of a block. (in the example:
    // first_left=2 gNewLeftCol_cx)
    auto first_left = (top_left.x - gFramePos.x) / MapPixelPos::PIXELS_PER_CHAR;
    gNewLeftCol_cx = MapPos::CELL_WIDTH - 1 - first_left;
    // Same for gFrame.cy and new rows on the top
    auto first_top = (top_left.y - gFramePos.y) / MapPixelPos::PIXELS_PER_CHAR;
    gNewTopRow_cy = MapPos::CELL_HEIGHT - 1 - first_top;
    // Similarly, for the right side we calculate the position of the last visible
    // character with respect to F. Note that bottom_right.x is the position of the first
    // non-visible pixel, so we need to subtract one pixel.
    //   (in the example: last_right=25 gRightVT = 3)
    auto last_right = (bottom_right.x - 1 - gFramePos.x) / MapPixelPos::PIXELS_PER_CHAR;
    gNewRightCol_cx = (MapPos::CELL_WIDTH - (last_right % MapPos::CELL_WIDTH)) % MapPos::CELL_WIDTH;
    gNewRightColOffset = ceil_of_div(last_right, MapPos::CELL_WIDTH);
    // Same for gFrame.cy and new rows on the bottom
    auto last_bottom = (bottom_right.y - 1 - gFramePos.y) / MapPixelPos::PIXELS_PER_CHAR;
    gNewBottomRow_cy = (MapPos::CELL_HEIGHT - (last_bottom % MapPos::CELL_HEIGHT)) % MapPos::CELL_HEIGHT;
    gNewBottomRowOffset = ceil_of_div(last_bottom, MapPos::CELL_HEIGHT);

    std::cout << "Frame info:"
              << std::endl
              << " top_left.x (x2)=" << top_left.x
              << " top_left.y (y1)=" << top_left.y
              << " bottom_right.x (x1)=" << bottom_right.x
              << " bottom_right.y (y2)=" << bottom_right.y
              << std::endl
              << " fred_pos.x=" << fred_pos.x
              << " fred_pos.y=" << fred_pos.y
              << std::endl
              << " gFredOffset_x=" << gFredOffset_x
              << " gFredOffset_y=" << gFredOffset_y
              << std::endl
              << " gFramePos.x=" << gFramePos.x
              << " gFramePos.y=" << gFramePos.y
              << std::endl
              << " gNewLeftCol_cx=" << gNewLeftCol_cx
              << " gNewRightCol_cx=" << gNewRightCol_cx
              << " gNewRightColOffset=" << gNewRightColOffset
              << std::endl
              << " gNewTopRow_cy=" << gNewTopRow_cy
              << " gNewBottomRow_cy=" << gNewBottomRow_cy
              << " gNewBottomRowOffset=" << gNewBottomRowOffset
              << std::endl;
}

ScreenPos Frame::getScreenPosOf(MapPos const &sprite_pos) const
{
    ScreenPos spos;
    spos.x = gFramePos.x +
             (sprite_pos.x - gFrame.x) * MapPixelPos::CELL_WIDTH_PIXELS +
             (sprite_pos.cx - gFrame.cx) * MapPixelPos::PIXELS_PER_CHAR;
    spos.y = gFramePos.y +
             (sprite_pos.y - gFrame.y) * MapPixelPos::CELL_HEIGHT_PIXELS +
             (sprite_pos.cy - gFrame.cy) * MapPixelPos::PIXELS_PER_CHAR;
    return spos;
}

void Frame::adjustFramePos(MapPos fred_pos)
{
    gFrame = {fred_pos.x - gFredOffset_x,
              fred_pos.y - gFredOffset_y,
              fred_pos.cx,
              fred_pos.cy};
}
