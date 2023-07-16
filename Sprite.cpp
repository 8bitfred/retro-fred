#include "Sprite.hpp"
#include "Frame.hpp"
#include "sdl.hpp"
#include "TextureManager.hpp"

Sprite::Sprite(Frame const &frame, MapPos const &pos,
               int char_width, int char_height)
    : sprite_pos(pos)
    , char_width(char_width)
    , char_height(char_height)
    , max_visible_pos(frame.getBottomRight())
{
    min_visible_pos.x = frame.getTopLeft().x - char_width * MapPixelPos::PIXELS_PER_CHAR + 1;
    min_visible_pos.y = frame.getTopLeft().y - char_height * MapPixelPos::PIXELS_PER_CHAR + 1;
}

bool Sprite::isVisible(Frame const &frame) const
{
    auto spos = frame.getScreenPosOf(sprite_pos);
    return spos.x > min_visible_pos.x && spos.y > min_visible_pos.y &&
           spos.x < max_visible_pos.x && spos.y < max_visible_pos.y;
}

void Sprite::render(Frame const &frame, TextureManager const &tmgr,
                    SDL_Renderer *renderer) const
{
    if (!isVisible(frame))
        return;
    auto render_info = getTexture();
    auto texture = tmgr.get(render_info.texture_id);
    SDL_Rect rect;
    auto spos = frame.getScreenPosOf(sprite_pos);
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
