#include "Game.hpp"
#include "Config.hpp"
#include "Fred.hpp"

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
