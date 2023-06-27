#include "Sprite.hpp"
#include "Frame.hpp"
#include "sdl.hpp"
#include "TextureManager.hpp"

Sprite::Sprite(Frame const &frame, MapPos const &pos,
               int char_width, int char_height)
    : sSpritePos(pos), sCharWidth(char_width), sCharHeight(char_height)
{
    sMinPos.x = frame.getTopLeft().x - char_width * MapPixelPos::PIXELS_PER_CHAR + 1;
    sMinPos.y = frame.getTopLeft().y - char_height * MapPixelPos::PIXELS_PER_CHAR + 1;
    sMaxPos = frame.getBottomRight();
}

bool Sprite::isVisible(Frame const &frame) const
{
    auto spos = frame.getScreenPosOf(sSpritePos);
    return spos.x > sMinPos.x && spos.y > sMinPos.y &&
           spos.x < sMaxPos.x && spos.y < sMaxPos.y;
}

void Sprite::render(Frame const &frame, TextureManager const &tmgr,
                    SDL_Renderer *renderer) const
{
    if (!isVisible(frame))
        return;
    auto [texture_id, center_pos] = getTexture();
    auto texture = tmgr.get(texture_id);
    SDL_Rect rect;
    auto spos = frame.getScreenPosOf(sSpritePos);
    rect.x = spos.x - center_pos.x;
    rect.y = spos.y - center_pos.y;
    auto query_error = SDL_QueryTexture(texture, nullptr, nullptr, &rect.w, &rect.h);
    if (query_error != 0)
        throw sdl::Error();
    auto render_error = SDL_RenderCopy(renderer, texture, nullptr, &rect);
    if (render_error != 0)
        throw sdl::Error();
}
