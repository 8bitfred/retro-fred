#include "Controller.hpp"
#include "platform/Renderer.hpp"
#include "TextureManager.hpp"
#include "DisplayConfig.hpp"
#include <algorithm>

namespace
{
    int getButtonSize(int window_w, int window_h)
    {
        if (window_w > window_h) // landscape orientation
            return window_w / 5;
        // portrait orientation
        return std::min(window_w / 2, window_h / 5);
    }

    int getOffset(int window_w, int window_h)
    {
        return std::min(window_w / 32, window_h / 32);
    }
}

Controller::Margins Controller::getMargins(int window_w, int window_h)
{
    // We only reserve half of the size of the buttons, so that the game is not too small
    auto half_size = getButtonSize(window_w, window_h) / 2;
    if (window_w > window_h) // landscape orientation
        return Margins{half_size, 0};
    // portrait orientation
    return Margins{0, half_size};
}

Controller::Controller(DisplayConfig const &display_cfg, bool back_button)
    : back_button(back_button)
{
    resetPosition(display_cfg);
}

void Controller::resetPosition(DisplayConfig const &display_cfg)
{
    auto [window_w, window_h] = display_cfg.getWindowSize();
    auto size = getButtonSize(window_w, window_h);
    auto offset = getOffset(window_w, window_h);

    arrows = Rect{offset, window_h - size - offset, size, size};
    fire = Rect{window_w - size - offset, window_h - size - offset, size, size};
    auto third = size / 3;
    back = Rect{window_w - third - offset, offset, third, third};

    float window_fw = static_cast<float>(window_w);
    float window_fh = static_cast<float>(window_h);
    farrows = FRect{arrows.x / window_fw, arrows.y / window_fh,
                   arrows.w / window_fw, arrows.h / window_fh};
    ffire = FRect{fire.x / window_fw, fire.y / window_fh,
                 fire.w / window_fw, fire.h / window_fh};
    fback = FRect{back.x / window_fw, back.y / window_fh,
                 back.w / window_fw, back.h / window_fh};
}

void Controller::render(Renderer &renderer,
                        TextureManager const &tmgr) const
{
    static Rect const arrows_src{0, 0, 360, 360};
    static Rect const fire_src{390, 0, 360, 360};
    static Rect const back_src{900, 120, 120, 120};
    renderer.setScale(1.0, 1.0);
    renderer.setViewport(nullptr);
    renderer.setTextureAlphaMod(tmgr.get(TextureID::CONTROLLER), 200);
    renderer.renderCopy(tmgr.get(TextureID::CONTROLLER), &arrows_src, &arrows);
    renderer.renderCopy(tmgr.get(TextureID::CONTROLLER), &fire_src, &fire);
    if (back_button)
        renderer.renderCopy(tmgr.get(TextureID::CONTROLLER), &back_src, &back);
}
