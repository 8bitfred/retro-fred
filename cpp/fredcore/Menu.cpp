#include "Menu.hpp"
#include "TextureManager.hpp"
#include "GameEvent.hpp"
#include "SoundManager.hpp"

void MenuItem::render(SDL_Renderer *renderer, TextureManager const &tmgr,
                      SDL_Rect const *rect) const
{
    SDL_Rect selector_rect = {rect->x, rect->y, 8, 8};
    renderMarker(renderer, tmgr, &selector_rect);
    tmgr.renderText(renderer, label, rect->x + 8, rect->y, 206, 206, 206);
}

void CheckBox::renderMarker(SDL_Renderer *renderer, TextureManager const &tmgr,
                            SDL_Rect const *rect) const
{
    if (*data)
        tmgr.renderText(renderer, "*", rect->x, rect->y, 0, 206, 0);
}

void Menu::render(SDL_Renderer *renderer, TextureManager const &tmgr) const
{
    sdl::ColorGuard draw_color(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &rect);
    SDL_Rect item_rect = {rect.x + 8, rect.y + 8, rect.w - 8, 8};
    for (size_t i = 0; i < item_list.size(); ++i)
    {
        if (current == i)
            tmgr.renderText(renderer, ">", rect.x, item_rect.y, 206, 206, 206);
        item_list[i].menu_item->render(renderer, tmgr, &item_rect);
        item_rect.y += 8;
    }
}

void Menu::eventHandler(EventMask const &event_mask, SoundManager &smgr)
{
    selected = -1;
    if (event_mask.check(GameEvent::DOWN))
    {
        current = (current + 1) % item_list.size();
        smgr.play(SoundID::WALK);
    }
    else if (event_mask.check(GameEvent::UP))
    {
        current = (current + item_list.size() - 1) % item_list.size();
        smgr.play(SoundID::WALK);
    }
    else if (event_mask.check(GameEvent::FIRE) || event_mask.check(GameEvent::RETURN))
    {
        selected = item_list[current].selection_code;
        item_list[current].menu_item->action();
        smgr.play(SoundID::FIRE);
    }
}