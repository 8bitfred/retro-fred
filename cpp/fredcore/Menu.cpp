#include "Menu.hpp"
#include "platform/Renderer.hpp"
#include "TextureManager.hpp"
#include "platform/GameEvent.hpp"
#include "SoundManager.hpp"

void MenuItem::render(Renderer &renderer, TextureManager const &tmgr,
                      Rect const *rect) const
{
    Rect selector_rect = {rect->x, rect->y, 8, 8};
    renderMarker(renderer, tmgr, &selector_rect);
    tmgr.renderText(renderer, label, rect->x + 8, rect->y, Color{206, 206, 206});
}

void CheckBox::renderMarker(Renderer &renderer, TextureManager const &tmgr,
                            Rect const *rect) const
{
    if (*data)
        tmgr.renderText(renderer, "*", rect->x, rect->y, Color{0, 206, 0});
}

void Menu::render(Renderer &renderer, TextureManager const &tmgr) const
{
    renderer.renderFillRect(&rect, Color{0, 0, 0, 255});
    Rect item_rect = {rect.x + 8, rect.y + 8, rect.w - 8, 8};
    for (size_t i = 0; i < item_list.size(); ++i)
    {
        if (current == i)
            tmgr.renderText(renderer, ">", rect.x, item_rect.y, Color{206, 206, 206});
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
