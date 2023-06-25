#include "Game.hpp"
#include "Config.hpp"
#include <iostream>
#include <cstdio>

namespace
{
    static constexpr unsigned ACTION_LEFT = 0x01;
    static constexpr unsigned ACTION_RIGHT = 0x02;
    static constexpr unsigned ACTION_UP = 0x04;
    static constexpr unsigned ACTION_DOWN = 0x08;

    unsigned getActionOfKey(SDL_Keycode keycode)
    {
        switch (keycode)
        {
        case SDLK_UP:
            return ACTION_UP;
        case SDLK_DOWN:
            return ACTION_DOWN;
        case SDLK_LEFT:
            return ACTION_LEFT;
        case SDLK_RIGHT:
            return ACTION_RIGHT;
        default:
            return 0;
        }
    }
} // namespace


Game::Game(Config const &cfg, std::minstd_rand &random_engine,
    TextureManager const &tmgr)
    : tmgr(tmgr)
    , frame(cfg), game_map(random_engine, cfg.map_width, cfg.map_height)
    , sprite_lists(static_cast<size_t>(SpriteClass::COUNT))
{
    game_map.initializeMapBlocks(tmgr, frame,
                                 getSpriteList(SpriteClass::BLOCK));
}

void Game::renderSprites(SDL_Renderer *renderer) const
{
    for (auto const &sprites: sprite_lists) {
        for (auto const &s: sprites)
            s->render(frame, renderer);
    }
}

void Game::mainLoop(SDL_Renderer *renderer) 
{
    Uint32 start_ticks = SDL_GetTicks();
    std::uint32_t frame_count = 0;
    bool quit = false;
    unsigned action = 0;
    while (!quit)
    {
        SDL_Event event;
        unsigned action_this_cycle = 0;
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
                quit = true;
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.repeat == 0) {
                    auto action_of_key = getActionOfKey(event.key.keysym.sym);
                    action |= action_of_key;
                    action_this_cycle |= action_of_key;
                    printf("KEY DOWN: %02x %02x %02x\n", action_of_key, action, action_this_cycle);
                }
            }
            else if (event.type == SDL_KEYUP) {
                auto action_of_key = getActionOfKey(event.key.keysym.sym);
                action &= ~action_of_key;
                printf("KEY UP: %02x %02x\n", action_of_key, action);
            }
        }

        action_this_cycle |= action;
        if ((action_this_cycle & ACTION_RIGHT) != 0)
        {
            frame.right();
            game_map.updateMapBlocksRight(tmgr, frame, getSpriteList(SpriteClass::BLOCK));
        }
        if ((action_this_cycle & ACTION_LEFT) != 0) {
            frame.left();
            game_map.updateMapBlocksLeft(tmgr, frame, getSpriteList(SpriteClass::BLOCK));
        }
        if ((action_this_cycle & ACTION_UP) != 0) {
            frame.up();
            game_map.updateMapBlocksUp(tmgr, frame, getSpriteList(SpriteClass::BLOCK));
        }
        if ((action_this_cycle & ACTION_DOWN) != 0) {
            frame.down();
            game_map.updateMapBlocksDown(tmgr, frame, getSpriteList(SpriteClass::BLOCK));
        }
        if (action_this_cycle != 0) {
            std::cout << "frame: x=" << frame.getFrame().x << " y=" << frame.getFrame().y
                      << " cx=" << frame.getFrame().cx << " cy=" << frame.getFrame().cy
                      << std::endl;
        }

        SDL_RenderClear(renderer);
        renderSprites(renderer);
        SDL_RenderPresent(renderer);

        ++frame_count;
        Uint32 frame_time = frame_count * 1000 / FRAMES_PER_SECOND;
        Uint32 ticks = SDL_GetTicks() - start_ticks;
        if (ticks < frame_time)
            SDL_Delay(frame_time - ticks);
        if (frame_count == (10 * FRAMES_PER_SECOND))
        {
            frame_count = 0;
            start_ticks += frame_time;
        }
    }
}