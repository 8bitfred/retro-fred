#include "TextureManager.hpp"
#include "Config.hpp"
#include <cassert>
#include <cstdio>
#include <map>
#include <algorithm>

TextureManager::TextureManager(Config const &cfg, Renderer &renderer)
{
    static char const *sprite_list[] = {
        "sprites/splash_screen.png",
        "sprites/fred_logo.png",
        "sprites/todays_greatest.png",
        "sprites/block.png",
        "sprites/bullet.png",
        "sprites/aciddrop.png",
        "sprites/skeleton.png",
        "sprites/mummy.png",
        "sprites/vampire.png",
        "sprites/rat.png",
        "sprites/chameleon.png",
        "sprites/object.png",
        "sprites/ghost.png",
        "sprites/smoke.png",
        "sprites/fred.png",
        "sprites/tomb.png",
        "sprites/window.png",
        "sprites/zx_font.png",
        "sprites/small_digits.png",
        "sprites/power.png",
        "sprites/fred_puffing.png",
        "sprites/controller.png",
        "sprites/fred_icon.png",
        "sprites/loading1.png",
        "sprites/loading2.png",
        "sprites/loading3.png",
        "sprites/loading4.png",
    };
    static_assert(std::size(sprite_list) == static_cast<size_t>(TextureID::COUNT));
    for (auto p : sprite_list) {
        auto path = cfg.resource_path.empty() ? p : cfg.resource_path + "/" + p;
        texture_list.emplace_back(renderer.loadTexture(path));
    }

    auto const &fred_icon_path = sprite_list[static_cast<int>(TextureID::FRED_ICON)];
    auto fred_path = cfg.resource_path.empty() ? fred_icon_path : cfg.resource_path + "/" + fred_icon_path;
    fred_icon = renderer.loadSurface(fred_path);
}

void TextureManager::renderText(Renderer &renderer, std::string_view text,
                                int x, int y, Color const &color) const
{
    auto &texture = get(TextureID::ZX_FONT);
    renderer.setTextureColorMod(texture, color);
    Rect src{0, 0, 8, 8};
    Rect dst{x, y, 8, 8};
    for (auto ch : text)
    {
        auto c = static_cast<unsigned>(ch);
        if (c > ' ' && c < 0x90)
        {
            src.x = 10 * (c % 16) + 1;
            src.y = 10 * ((c - ' ') / 16) + 1;
            renderer.renderCopy(texture, &src, &dst);
        }
        dst.x += 8;
    }
}

void TextureManager::renderScore(Renderer &renderer, unsigned score,
                                 int x, int y, Color const &color) const
{
    auto &texture = get(TextureID::SMALL_DIGITS);
    renderer.setTextureColorMod(texture, color);
    score = std::min(score, 999999u);
    Rect src{0, 0, 5, 5};
    Rect dst{x + 5 * 6, y, 5, 5};
    for (int i = 0; i < 6; ++i)
    {
        int digit = score % 10;
        score /= 10;
        src.x = 5 * digit;
        renderer.renderCopy(texture, &src, &dst);
        dst.x -= 6;
    }
}
