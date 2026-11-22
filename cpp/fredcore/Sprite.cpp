#include "Sprite.hpp"
#include "GameWindow.hpp"
#include "platform/Renderer.hpp"
#include "TextureManager.hpp"
#include "Config.hpp"

void Sprite::render(Config const &cfg,
                    GameWindow const &game_window, TextureManager const &tmgr,
                    Renderer &renderer) const
{
    auto const &box_params = getBoxParams();
    auto spos = game_window.getScreenPosOf(sprite_pos);
    Rect dst_rect = game_window.getScreenRect(spos, box_params.bounding_box);
    if (!hasIntersection(game_window.getWindowRect(), dst_rect))
        return;
    auto render_params = getRenderParams();
    Rect src_rect = {box_params.pos_x + box_params.bounding_box.x,
                     box_params.pos_y + box_params.bounding_box.y,
                     box_params.bounding_box.w, box_params.bounding_box.h};
    auto &texture = tmgr.get(render_params.texture_id);
    renderer.setTextureColorMod(texture, Color{render_params.color_mod.r,
                                               render_params.color_mod.g,
                                               render_params.color_mod.b});
    renderer.renderCopy(texture, &src_rect, &dst_rect, render_params.hflip);

    if (cfg.boxes)
    {
        if (static_cast<int>(render_params.texture_id) < static_cast<int>(TextureID::ACID_DROP))
            return;
        renderer.renderDrawRect(dst_rect, Color{255, 255, 255, 128});
        for (auto const &box : box_params.hit_boxes)
        {
            Rect hitbox_rect = getHitboxPos(spos.x, spos.y,
                                            box_params.bounding_box, box, render_params.hflip);
            renderer.renderDrawRect(hitbox_rect, Color{255, 200, 0, 128});
        }
    }
}

void Sprite::setLabel(GameWindow const &game_window, LabelTable &label_table) const
{
    auto const &box_params = getBoxParams();
    auto spos = game_window.getScreenPosOf(sprite_pos);
    Rect dst_rect = game_window.getScreenRect(spos, box_params.bounding_box);
    if (!hasIntersection(game_window.getWindowRect(), dst_rect))
        return;
    label_table.set(dst_rect, getLabel());
}

Rect Sprite::getHitboxPos(int px, int py,
                          Rect const &bounding_box,
                          Rect const &hitbox, bool hflip)
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

bool Sprite::checkHitBoxes(Rect const &rect2, Rect const &bounding_box, bool hflip) const
{
    for (auto const &box : getBoxParams().hit_boxes)
    {
        Rect rect1 = getHitboxPos(sprite_pos.px(), sprite_pos.py(),
                                  bounding_box, box, hflip);
        if (hasIntersection(rect1, rect2))
            return true;
    }
    return false;
}

bool Sprite::checkCollision(Sprite const &other)
{
    auto const &box_params1 = getBoxParams();
    auto const &box_params2 = other.getBoxParams();
    Rect bounds1{sprite_pos.px() + box_params1.bounding_box.x,
                sprite_pos.py() + box_params1.bounding_box.y,
                box_params1.bounding_box.w, box_params1.bounding_box.h};
    Rect bounds2{other.sprite_pos.px() + box_params2.bounding_box.x,
                other.sprite_pos.py() + box_params2.bounding_box.y,
                box_params2.bounding_box.w, box_params2.bounding_box.h};
    if (!hasIntersection(bounds1, bounds2))
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
            Rect hbox1 = getHitboxPos(sprite_pos.px(), sprite_pos.py(),
                                      box_params1.bounding_box, box, render_params1.hflip);
            if (other.checkHitBoxes(hbox1, box_params2.bounding_box, render_params2.hflip))
                return true;
        }
    }
    return false;
}
