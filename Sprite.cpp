#include "Sprite.hpp"
#include "Window.hpp"
#include "sdl.hpp"
#include "TextureManager.hpp"

Sprite::Sprite(Window const &window, MapPos const &pos,
               int char_width, int char_height)
    : sprite_pos(pos)
    , char_width(char_width)
    , char_height(char_height)
    , max_visible_pos(window.getBottomRight())
{
    min_visible_pos.x = window.getTopLeft().x - char_width * MapPixelPos::PIXELS_PER_CHAR + 1;
    min_visible_pos.y = window.getTopLeft().y - char_height * MapPixelPos::PIXELS_PER_CHAR + 1;
}

bool Sprite::isVisible(Window const &window) const
{
    auto spos = window.getScreenPosOf(sprite_pos);
    return spos.x > min_visible_pos.x && spos.y > min_visible_pos.y &&
           spos.x < max_visible_pos.x && spos.y < max_visible_pos.y;
}

void Sprite::render(Window const &window, TextureManager const &tmgr,
                    SDL_Renderer *renderer) const
{
    if (!isVisible(window))
        return;
    auto const &render_info = getTexture();
    auto texture = tmgr.get(render_info.texture_id);
    SDL_Rect rect;
    auto spos = window.getScreenPosOf(sprite_pos);
    rect.x = spos.x - render_info.center_x;
    rect.y = spos.y - render_info.center_y;
    rect.w = render_info.src_rect.w;
    rect.h = render_info.src_rect.h;
    auto render_error = SDL_RenderCopy(renderer, texture, &render_info.src_rect, &rect);
    if (render_error != 0)
        throw sdl::Error();
}

void Sprite::update(Game &, unsigned)
{

}

bool Sprite::checkCollision(Sprite const &other)
{
    auto x2 = sprite_pos.getCharX() + char_width;
    auto other_x2 = other.sprite_pos.getCharX() + other.char_width;
    if (x2 <= other.sprite_pos.getCharX() ||
        other_x2 <= sprite_pos.getCharX())
        return false;

    auto y2 = sprite_pos.getCharY() + char_height;
    auto other_y2 = other.sprite_pos.getCharY() + other.char_height;
    if (y2 <= other.sprite_pos.getCharY() ||
        other_y2 <= sprite_pos.getCharY())
        return false;
    return true;
}
