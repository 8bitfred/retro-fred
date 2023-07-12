#include "Game.hpp"
#include "Config.hpp"
#include "Fred.hpp"
#include "SoundManager.hpp"
#include <iostream>

unsigned Game::getEventOfKey(SDL_Keycode keycode)
{
    switch (keycode)
    {
    case SDLK_UP:
        return EVENT_UP;
    case SDLK_DOWN:
        return EVENT_DOWN;
    case SDLK_LEFT:
        return EVENT_LEFT;
    case SDLK_RIGHT:
        return EVENT_RIGHT;
    case SDLK_SPACE:
        return EVENT_FIRE;
    case SDLK_LSHIFT:
        return EVENT_SHIFT;
    case SDLK_f:
        return EVENT_RESET_FRED;
    case SDLK_o:
        return EVENT_HATCH_LEFT;
    case SDLK_p:
        return EVENT_HATCH_RIGHT;
    case SDLK_h:
        return EVENT_MOVE_TO_HATCH;
    default:
        return 0;
    }
}

Game::Game(Config const &cfg, std::minstd_rand &random_engine,
    TextureManager const &tmgr, SoundManager &smgr)
    : tmgr(tmgr), smgr(smgr)
    , frame(cfg), game_map(cfg, random_engine)
    , sprite_lists(static_cast<size_t>(SpriteClass::COUNT))
{
}

void Game::renderSprites(SDL_Renderer *renderer) const
{
    for (auto const &sprites: sprite_lists) {
        for (auto const &s: sprites)
            s->render(frame, tmgr, renderer);
    }
}

void Game::moveFrame(int deltax, int deltay)
{

    frame.moveFrame(deltax, deltay);
    if (deltax > 0)
        game_map.updateMapBlocksRight(frame, getSpriteList(SpriteClass::BLOCK));
    else if (deltax < 0)
        game_map.updateMapBlocksLeft(frame, getSpriteList(SpriteClass::BLOCK));
    if (deltay > 0)
        game_map.updateMapBlocksDown(frame, getSpriteList(SpriteClass::BLOCK));
    else if (deltay < 0)
        game_map.updateMapBlocksUp(frame, getSpriteList(SpriteClass::BLOCK));
}

void Game::playSound(SoundID sound_id)
{
    smgr.play(sound_id);
}

void Game::dbgResetMapBlocks()
{
    auto &block_list = getSpriteList(SpriteClass::BLOCK);
    block_list.clear();
    game_map.initializeMapBlocks(frame, block_list);
}
