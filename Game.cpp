#include "Game.hpp"
#include "Config.hpp"

Game::Game(Config const &cfg, std::minstd_rand &random_engine)
    : frame(cfg), game_map(random_engine, cfg.map_width, cfg.map_height)
{
}
