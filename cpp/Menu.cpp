#include "Menu.hpp"
#include "fredcore/TextureManager.hpp"
#include "fredcore/GameEvent.hpp"
#include "fredcore/SoundManager.hpp"

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
    SDL_Rect item_rect = {rect.x + 8, rect.y, rect.w - 8, 8};
    for (size_t i = 0; i < item_list.size(); ++i)
    {
        if (current == i)
            tmgr.renderText(renderer, ">", rect.x, item_rect.y, 206, 206, 206);
        item_list[i]->render(renderer, tmgr, &item_rect);
        item_rect.y += 8;
    }
}

void Menu::eventHandler(EventMask const &event_mask, SoundManager &smgr)
{
    selected = item_list.size();
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
    else if (event_mask.check(GameEvent::FIRE))
    {
        selected = current;
        item_list[current]->action();
        smgr.play(SoundID::FIRE);
    }
}