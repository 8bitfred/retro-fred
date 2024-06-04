#pragma once

#include <memory>
#include <string>
#include <vector>
#include "fredcore/sdl.hpp"

class TextureManager;
class EventMask;

class MenuItem
{
    std::string label;
public:
    explicit MenuItem(std::string const &label): label(label) {}
    explicit MenuItem(std::string &&label): label(std::move(label)) {}
    virtual ~MenuItem() = default;

    void render(SDL_Renderer *renderer, TextureManager const &tmgr,
                SDL_Rect const *rect) const;

    virtual void renderMarker(SDL_Renderer *, TextureManager const &,
                              SDL_Rect const *) const {}
    virtual void action() {}
};

class CheckBox: public MenuItem
{
    bool *data;

    void renderMarker(SDL_Renderer *renderer, TextureManager const &tmgr, 
                      SDL_Rect const *rect) const final;
    void action() final { *data = !*data; }

public:
    CheckBox(std::string &&label, bool *data)
    : MenuItem(std::move(label)), data(data) {}
};

class Menu
{
    SDL_Rect rect;
    size_t current = 0, selected = 0;
    std::vector<std::unique_ptr<MenuItem>> item_list;

public:
    explicit Menu(SDL_Rect const &rect): rect(rect) {}
    void addItem(std::unique_ptr<MenuItem> &&item, bool make_default = false)
    {
        if (make_default)
            current = item_list.size();
        item_list.emplace_back(std::move(item));
    }
    void render(SDL_Renderer *renderer, TextureManager const &tmgr) const;
    void eventHandler(EventMask const &event_mask);
    bool isSelected(size_t i) const { return selected == i; }
};
