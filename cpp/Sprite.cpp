#include "Sprite.hpp"
#include "Window.hpp"
#include "sdl.hpp"
#include "TextureManager.hpp"
#include "Config.hpp"

bool Sprite::isVisible(Window const &window) const
{
    auto const &box_params = getBoxParams();
    auto spos = window.getScreenPosOf(sprite_pos);
    SDL_Rect dst_rect = {spos.x + box_params.bounding_box.x,
                         spos.y + box_params.bounding_box.y,
                         box_params.bounding_box.w, box_params.bounding_box.h};
    return SDL_HasIntersection(&window.rect(), &dst_rect);
}

void Sprite::render(Config const &cfg,
                    Window const &window, TextureManager const &tmgr,
                    SDL_Renderer *renderer) const
{
    auto const &box_params = getBoxParams();
    auto spos = window.getScreenPosOf(sprite_pos);
    SDL_Rect dst_rect = {spos.x + box_params.bounding_box.x,
                         spos.y + box_params.bounding_box.y,
                         box_params.bounding_box.w, box_params.bounding_box.h};
    if (!SDL_HasIntersection(&window.rect(), &dst_rect))
        return;
    auto render_params = getRenderParams();
    SDL_Rect src_rect = {box_params.pos_x + box_params.bounding_box.x,
                         box_params.pos_y + box_params.bounding_box.y,
                         box_params.bounding_box.w, box_params.bounding_box.h};
    auto texture = tmgr.get(render_params.texture_id);
    SDL_SetTextureColorMod(texture,
                           render_params.color_mod.r, 
                           render_params.color_mod.g,
                           render_params.color_mod.b);
    auto render_error = SDL_RenderCopyEx(renderer, texture,
                                         &src_rect, &dst_rect, 0, nullptr,
                                         render_params.hflip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    if (render_error != 0)
        throw sdl::Error();

    if (cfg.boxes)
    {
        if (static_cast<int>(render_params.texture_id) < static_cast<int>(TextureID::ACID_DROP))
            return;
        Uint8 saved_r, saved_g, saved_b, saved_a;
        SDL_BlendMode saved_bm;
        SDL_GetRenderDrawColor(renderer, &saved_r, &saved_g, &saved_b, &saved_a);
        SDL_GetRenderDrawBlendMode(renderer, &saved_bm);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 128);
        SDL_RenderDrawRect(renderer, &dst_rect);
        SDL_SetRenderDrawColor(renderer, 255, 200, 0, 128);
        for (auto const &box : box_params.hit_boxes)
        {
            SDL_Rect hitbox_rect = getHitboxPos(spos.x, spos.y,
                                                box_params.bounding_box, box, render_params.hflip);
            SDL_RenderDrawRect(renderer, &hitbox_rect);
        }
        SDL_SetRenderDrawColor(renderer, saved_r, saved_g, saved_b, saved_a);
        SDL_SetRenderDrawBlendMode(renderer, saved_bm);
    }
}

SDL_Rect Sprite::getHitboxPos(int px, int py,
                              SDL_Rect const &bounding_box,
                              SDL_Rect const &hitbox, bool hflip)
{
    if (hflip)
    {
        auto dx = bounding_box.x + bounding_box.w - (hitbox.x + hitbox.w);
        return {px + bounding_box.x + dx, py + hitbox.y, hitbox.w, hitbox.h};
    }
    else 
    {
        return {px + hitbox.x, py + hitbox.y, hitbox.w, hitbox.h};
    }
}

bool Sprite::checkHitBoxes(SDL_Rect const &rect2, SDL_Rect const &bounding_box, bool hflip) const
{
    for (auto const &box : getBoxParams().hit_boxes)
    {
        SDL_Rect rect1 = getHitboxPos(sprite_pos.px(), sprite_pos.py(),
                                      bounding_box, box, hflip);
        if (SDL_HasIntersection(&rect1, &rect2))
            return true;
    }
    return false;
}

bool Sprite::checkCollision(Sprite const &other)
{
    auto const &box_params1 = getBoxParams();
    auto const &box_params2 = other.getBoxParams();
    SDL_Rect bounds1{sprite_pos.px() + box_params1.bounding_box.x,
                     sprite_pos.py() + box_params1.bounding_box.y,
                     box_params1.bounding_box.w, box_params1.bounding_box.h};
    SDL_Rect bounds2{other.sprite_pos.px() + box_params2.bounding_box.x,
                     other.sprite_pos.py() + box_params2.bounding_box.y,
                     box_params2.bounding_box.w, box_params2.bounding_box.h};
    if (!SDL_HasIntersection(&bounds1, &bounds2))
        return false;

    if (box_params1.hit_boxes.empty() && box_params2.hit_boxes.empty())
        return true;

    auto render_params1 = getRenderParams();
    auto render_params2 = other.getRenderParams();
    if (box_params1.hit_boxes.empty())
        return other.checkHitBoxes(bounds1, box_params2.bounding_box, render_params2.hflip);
    else if (box_params2.hit_boxes.empty())
        return checkHitBoxes(bounds2, box_params1.bounding_box, render_params1.hflip);
    else
    {
        for (auto const &box : box_params1.hit_boxes)
        {
            SDL_Rect hbox1 = getHitboxPos(sprite_pos.px(), sprite_pos.py(),
                                          box_params1.bounding_box, box, render_params1.hflip);
            if (other.checkHitBoxes(hbox1, box_params2.bounding_box, render_params2.hflip))
                return true;
        }
    }
    return false;
}
