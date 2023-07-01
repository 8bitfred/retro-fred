#include "Game.hpp"
#include "Config.hpp"
#include "Fred.hpp"

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
    default:
        return 0;
    }
}

Game::Game(Config const &cfg, std::minstd_rand &random_engine,
    TextureManager const &tmgr)
    : tmgr(tmgr)
    , frame(cfg), game_map(random_engine, cfg.map_width, cfg.map_height)
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
        game_map.updateMapBlocksRight(tmgr, frame, getSpriteList(SpriteClass::BLOCK));
    else if (deltax < 0)
        game_map.updateMapBlocksLeft(tmgr, frame, getSpriteList(SpriteClass::BLOCK));
    if (deltay > 0)
        game_map.updateMapBlocksDown(tmgr, frame, getSpriteList(SpriteClass::BLOCK));
    else if (deltay < 0)
        game_map.updateMapBlocksUp(tmgr, frame, getSpriteList(SpriteClass::BLOCK));
}
