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
#if 0 // enable to show bounding and hit boxes
    if (static_cast<int>(render_info.texture_id) < static_cast<int>(TextureID::ACID_DROP))
        return;
    Uint8 saved_r, saved_g, saved_b, saved_a;
    SDL_BlendMode saved_bm;
    SDL_GetRenderDrawColor(renderer, &saved_r, &saved_g, &saved_b, &saved_a);
    SDL_GetRenderDrawBlendMode(renderer, &saved_bm);
    SDL_SetRenderDrawColor(renderer, 200, 0, 200, 128);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    if (getHitBoxes().empty())
        SDL_RenderDrawRect(renderer, &rect);
    else {
        for (auto const &box : getHitBoxes())
        {
            SDL_Rect r2{spos.x + box.x, spos.y + box.y, box.w, box.h};
            SDL_RenderDrawRect(renderer, &r2);
        }
    }
    SDL_SetRenderDrawColor(renderer, saved_r, saved_g, saved_b, saved_a);
    SDL_SetRenderDrawBlendMode(renderer, saved_bm);
#endif
}

void Sprite::update(Game &, unsigned)
{

}

bool Sprite::checkHitBoxes(SDL_Rect const &rect2) const
{
    for (auto const &box : getHitBoxes())
    {
        SDL_Rect rect1{sprite_pos.px() + box.x,
                       sprite_pos.py() + box.y,
                       box.w, box.h};
        if (SDL_HasIntersection(&rect1, &rect2))
            return true;
    }
    return false;
}

bool Sprite::checkCollision(Sprite const &other)
{
    RenderInfo const &ri1 = getTexture();
    RenderInfo const &ri2 = other.getTexture();
    SDL_Rect bounds1{sprite_pos.px() - ri1.center_x,
                     sprite_pos.py() - ri1.center_y,
                     ri1.src_rect.w, ri1.src_rect.h};
    SDL_Rect bounds2{other.sprite_pos.px() - ri2.center_x,
                     other.sprite_pos.py() - ri2.center_y,
                     ri2.src_rect.w, ri2.src_rect.h};
    if (!SDL_HasIntersection(&bounds1, &bounds2))
        return false;

    if (getHitBoxes().empty() && other.getHitBoxes().empty())
        return true;

    if (getHitBoxes().empty())
        return other.checkHitBoxes(bounds1);
    else if (other.getHitBoxes().empty())
        return checkHitBoxes(bounds2);
    else
    {
        for (auto const &box : getHitBoxes())
        {
            SDL_Rect hbox1{sprite_pos.px() + box.x,
                           sprite_pos.py() + box.y,
                           box.w, box.h};
            if (other.checkHitBoxes(hbox1))
                return true;
        }
    }
    return false;
}
