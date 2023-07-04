#include "Application.hpp"
#include "Config.hpp"
#include "Game.hpp"
#include "Fred.hpp"
#include "AcidDrop.hpp"


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
    static constexpr std::uint32_t FRAMES_PER_SECOND = 5;
    Game game(cfg, random_engine, tmgr);
    auto fred = initializeFred(game);
    game.getGameMap().initializeMapBlocks(game.getFrame(),
                                          game.getSpriteList(SpriteClass::BLOCK));
    initializeAcidDrops(game);

    Uint32 start_ticks = SDL_GetTicks();
    std::uint32_t frame_count = 0;
    bool quit = false;
    unsigned events = 0;
    while (!quit)
    {
        SDL_Event event;
        unsigned events_this_cycle = 0;
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
                quit = true;
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.repeat == 0) {
                    auto events_of_key = Game::getEventOfKey(event.key.keysym.sym);
                    events |= events_of_key;
                    events_this_cycle |= events_of_key;
                }
            }
            else if (event.type == SDL_KEYUP) {
                auto events_of_key = Game::getEventOfKey(event.key.keysym.sym);
                events &= ~events_of_key;
            }
        }

        events_this_cycle |= events;
        if ((events_this_cycle & Game::EVENT_SHIFT) != 0)
            debugMode(game, fred, events_this_cycle);
        else
            fred->updateFred(game, events_this_cycle);

        for (auto const& sprite: game.getSpriteList(SpriteClass::ACID_DROP))
            sprite->update(game, 0);

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


void FredApp::initializeAcidDrops(Game &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::ACID_DROP);
    std::uniform_int_distribution<> distrib_x(1, game.getGameMap().getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, game.getGameMap().getHeight() - 4);
    std::uniform_int_distribution<> distrib_frame(static_cast<int>(TextureID::ACID_DROP1),
                                                  static_cast<int>(TextureID::ACID_DROP4));
    for (int i = 0; i < 40; ++i) {
        while (true)
        {
            MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 0, 0};
            if (!game.getGameMap().isStone(pos.cellPos().vmove(-1)))
                continue;
            if (game.getGameMap().getCell(pos.cellPos()) != GameMap::Cell::EMPTY)
                continue;
            if (!game.getGameMap().isStone(pos.cellPos().vmove(1)))
                continue;
            if (auto cell = game.getGameMap().getCell(pos.cellPos().hmove(-1));
                cell == GameMap::Cell::ROPE_END ||
                cell == GameMap::Cell::ROPE_MAIN ||
                cell == GameMap::Cell::ROPE_START)
                continue;
            if (auto cell = game.getGameMap().getCell(pos.cellPos().hmove(1));
                cell == GameMap::Cell::ROPE_END ||
                cell == GameMap::Cell::ROPE_MAIN ||
                cell == GameMap::Cell::ROPE_START)
                continue;
            auto texture_id = static_cast<TextureID>(distrib_frame(random_engine));
            sprite_list.emplace_back(std::make_unique<AcidDrop>(game.getFrame(),
                                                                pos, texture_id));
            break;
        }
    }
}

void FredApp::debugMode(Game &game, Fred *fred, unsigned events)
{
    if ((events & Game::EVENT_LEFT) != 0)
        game.moveFrame(-1, 0);
    else if ((events & Game::EVENT_RIGHT) != 0)
        game.moveFrame(1, 0);
    else if ((events & Game::EVENT_UP) != 0)
        game.moveFrame(0, -1);
    else if ((events & Game::EVENT_DOWN) != 0)
        game.moveFrame(0, 1);
    else if ((events & Game::EVENT_RESET_FRED) != 0)
        fred->dbgResetPosition(game);
    else if ((events & Game::EVENT_HATCH_LEFT) != 0)
    {
        if (game.getGameMap().dbgMoveHatch(-1))
            game.dbgResetMapBlocks();
    }
    else if ((events & Game::EVENT_HATCH_RIGHT) != 0)
    {
        if (game.getGameMap().dbgMoveHatch(1))
            game.dbgResetMapBlocks();
    }
    else if ((events & Game::EVENT_MOVE_TO_HATCH) != 0)
        fred->dbgMoveToHatch(game);
}
