#include "TextureManager.hpp"
#include "Config.hpp"
#include <SDL_image.h>
#include <cassert>
#include <iostream>

sdl::TexturePtr TextureManager::createFrameTexture(Config const &cfg,
                                                   SDL_Renderer *renderer,
                                                   SDL_Texture *base_frame)
{
    Uint32 texture_format;
    SDL_QueryTexture(base_frame, &texture_format, nullptr, nullptr, nullptr);
    sdl::TexturePtr frame_texture(SDL_CreateTexture(renderer,
                                                    SDL_PIXELFORMAT_RGBX8888,
                                                    SDL_TEXTUREACCESS_TARGET,
                                                    cfg.window_width, cfg.window_height));
    // Set render target to frame_texture: RenderTargetGuard is a "guard" object that sets
    // the render target in the constructor, and restores it to the original value in the
    // destructor. This ensures that the render target is always restored when the
    // function is exited.
    sdl::RenderTargetGuard render_guard(renderer, frame_texture);
    //  Clear texture with a transparent color
    auto status = SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    if (status < 0)
        throw sdl::Error();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    status = SDL_RenderClear(renderer);
    if (status < 0)
        throw sdl::Error();
    SDL_RenderFillRect(renderer, nullptr);

    SDL_Rect const frame_char = {0, 0, 8, 8};
    for (int x = 0; x < cfg.window_width; x += 8)
    {
        SDL_Rect dst_rect = {x, 0, 8, 8};
        status = SDL_RenderCopy(renderer, base_frame, &frame_char, &dst_rect);
        if (status < 0)
            throw sdl::Error();
        break;
        dst_rect.y = cfg.window_height - 8;
        status = SDL_RenderCopy(renderer, base_frame, &frame_char, &dst_rect);
        if (status < 0)
            throw sdl::Error();
    }

    // for (int y = 0; y < cfg.window_height; y += 8)
    // {
    //     SDL_Rect dst_rect = {0, y, 8, 8};
    //     status = SDL_RenderCopy(renderer, base_frame, &frame_char, &dst_rect);
    //     if (status < 0)
    //         throw sdl::Error();
    //     for (int x = cfg.window_width - 56; x < cfg.window_width; x += 8)
    //     {
    //         dst_rect.x = x;
    //         status = SDL_RenderCopy(renderer, base_frame, &frame_char, &dst_rect);
    //         if (status < 0)
    //             throw sdl::Error();
    //         std::cout << "Creating frame x=" << x << " y=" << y << std::endl;
    //     }
    // }

    SDL_SetTextureBlendMode(frame_texture, SDL_BLENDMODE_BLEND);
    return frame_texture;
}

TextureManager::TextureManager(Config const& cfg, SDL_Renderer *renderer)
{
    // This list must match the order of elements in the TextureID enum
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/frame.png"));
    texture_list.emplace_back(createFrameTexture(cfg, renderer, texture_list.back()));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/stone_1x_1.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/stone_1x_2.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/stone_1x_3.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/ropestartsprite_1x.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/ropemainsprite_1x.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/ropeendsprite_1x.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/skysprite_1x.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/sandsprite_1x.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/trapdoorsprite_1x.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop_seq_1.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop_seq_2.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop_seq_3.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop_seq_4.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop_seq_5.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop_seq_6.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop_seq_7.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop_seq_8.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop_seq_9.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop_seq_10.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop_seq_11.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_01.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_02.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_03.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_04.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_05.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_06.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_07.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_08.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_09.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_10.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_11.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_12.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_13.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_14.png"));
    assert(texture_list.size() == static_cast<size_t>(TextureID::COUNT));
}
