#include "Game.hpp"
#include "Config.hpp"
#include "Fred.hpp"
#include "Actions.hpp"
#include <iostream>
#include <cstdio>

namespace
{
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
        case SDLK_SPACE:
            return ACTION_FIRE;
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
    initializeFred(random_engine);
    game_map.initializeMapBlocks(tmgr, frame,
                                 getSpriteList(SpriteClass::BLOCK));
}

void Game::initializeFred(std::minstd_rand &random_engine)
{
    CellPos fred_cell_position = {0, game_map.getHeight() - 2};
    std::uniform_int_distribution<> distrib(1, game_map.getWidth() - 2);
    while (true) 
    {
        int x = distrib(random_engine);
        fred_cell_position.x = distrib(random_engine);
        if (game_map.getCell(fred_cell_position) == GameMap::Cell::EMPTY)
            break;
    }
    MapPos fred_initial_position = {fred_cell_position.x, fred_cell_position.y, 0, 1};
    getSpriteList(SpriteClass::FRED).emplace_back(std::make_unique<Fred>(frame, fred_initial_position));
    frame.adjustFramePos(fred_initial_position);
}

Fred *Game::getFred() 
{ 
    return dynamic_cast<Fred *>(getSpriteList(SpriteClass::FRED).front().get()); 
}

void Game::renderSprites(SDL_Renderer *renderer) const
{
    for (auto const &sprites: sprite_lists) {
        for (auto const &s: sprites)
            s->render(frame, tmgr, renderer);
    }
}

void Game::mainLoop(SDL_Renderer *renderer) 
{
    Uint32 start_ticks = SDL_GetTicks();
    std::uint32_t frame_count = 0;
    bool quit = false;
    unsigned action = 0;
    auto fred = getFred();
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
        auto [deltax, deltay] = fred->updateFred(game_map, action_this_cycle);
        frame.moveFrame(deltax, deltay);
        if (deltax > 0)
            game_map.updateMapBlocksRight(tmgr, frame, getSpriteList(SpriteClass::BLOCK));
        else if (deltax < 0)
            game_map.updateMapBlocksLeft(tmgr, frame, getSpriteList(SpriteClass::BLOCK));
        if (deltay > 0)
            game_map.updateMapBlocksDown(tmgr, frame, getSpriteList(SpriteClass::BLOCK));
        else if (deltay < 0)
            game_map.updateMapBlocksUp(tmgr, frame, getSpriteList(SpriteClass::BLOCK));

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