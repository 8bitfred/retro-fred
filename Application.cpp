#include "Application.hpp"
#include "Config.hpp"
#include "Game.hpp"
#include "Fred.hpp"
#include "Actions.hpp"

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


FredApp::FredApp(Config const &cfg, std::minstd_rand &random_engine)
    : cfg(cfg)
    , random_engine(random_engine)
    , w_and_r(sdl::createWindowAndRenderer(cfg.scale_x * cfg.window_width,
                                           cfg.scale_y * cfg.window_height)),
      tmgr(getRenderer())
{
    SDL_RenderSetScale(getRenderer(), cfg.scale_x, cfg.scale_y);
}

void FredApp::playGame()
{
    static constexpr std::uint32_t FRAMES_PER_SECOND = 20;
    Game game(cfg, random_engine, tmgr);
    auto fred = initializeFred(game);
    game.getGameMap().initializeMapBlocks(tmgr, game.getFrame(),
                                          game.getSpriteList(SpriteClass::BLOCK));


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
                }
            }
            else if (event.type == SDL_KEYUP) {
                auto action_of_key = getActionOfKey(event.key.keysym.sym);
                action &= ~action_of_key;
            }
        }

        action_this_cycle |= action;
        auto [deltax, deltay] = fred->updateFred(game.getGameMap(), action_this_cycle);
        game.getFrame().moveFrame(deltax, deltay);
        if (deltax > 0)
            game.getGameMap().updateMapBlocksRight(tmgr, game.getFrame(), game.getSpriteList(SpriteClass::BLOCK));
        else if (deltax < 0)
            game.getGameMap().updateMapBlocksLeft(tmgr, game.getFrame(), game.getSpriteList(SpriteClass::BLOCK));
        if (deltay > 0)
            game.getGameMap().updateMapBlocksDown(tmgr, game.getFrame(), game.getSpriteList(SpriteClass::BLOCK));
        else if (deltay < 0)
            game.getGameMap().updateMapBlocksUp(tmgr, game.getFrame(), game.getSpriteList(SpriteClass::BLOCK));

        SDL_RenderClear(getRenderer());
        game.renderSprites(getRenderer());
        SDL_RenderPresent(getRenderer());

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

Fred* FredApp::initializeFred(Game &game)
{
    CellPos fred_cell_position = {0, game.getGameMap().getHeight() - 2};
    std::uniform_int_distribution<> distrib(1, game.getGameMap().getWidth() - 2);
    while (true)
    {
        int x = distrib(random_engine);
        fred_cell_position.x = distrib(random_engine);
        if (game.getGameMap().getCell(fred_cell_position) == GameMap::Cell::EMPTY)
            break;
    }
    MapPos fred_initial_position = {fred_cell_position.x, fred_cell_position.y, 0, 1};
    auto fred_unique_ptr = std::make_unique<Fred>(game.getFrame(), fred_initial_position);
    auto fred_ptr = fred_unique_ptr.get();
    game.getFrame().adjustFramePos(fred_initial_position);
    game.getSpriteList(SpriteClass::FRED).emplace_back(std::move(fred_unique_ptr));
    return fred_ptr;
}
