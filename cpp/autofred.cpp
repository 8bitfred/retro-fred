#include "sdl.hpp"
#include "Application.hpp"
#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <SDL_image.h>


class Player
{
    GameBase const &game;
    std::vector<CellPos> stack;
    std::vector<bool> visited;
    MapPos next_pos;
    bool visit(CellPos const &pos);
    CellPos getNextCell();

public:
    explicit Player(GameBase const &game);
    GameEvent getMove();
};

bool Player::visit(CellPos const &pos)
{
    auto index = pos.y * game.getGameMap().getWidth() + pos.x;
    bool result = visited[index];
    visited[index] = true;
    return result;
}

CellPos Player::getNextCell()
{
    std::pair<int, int> moves[] = {
        {  0, -1 },
        { -1,  0 },
        {  1,  0 },
        {  0,  1 },
    };
    auto pos = stack.back();
    for (auto [delta_x, delta_y] : moves)
    {
        CellPos next_pos{pos.x + delta_x, pos.y + delta_y};
        if (game.getGameMap().isStone(next_pos))
            continue;
        if (!visit(next_pos))
        {
            stack.emplace_back(next_pos);
            return next_pos;
        }
    }
    if (stack.size() == 1)
    {
        std::cerr << "Back to starting point\n";
        std::fill(visited.begin(), visited.end(), false);
        visit(stack.back());
    }
    else
        stack.pop_back();
    return stack.back();
}

Player::Player(GameBase const &game)
    : game(game)
    , visited(game.getGameMap().getWidth() * game.getGameMap().getHeight(), false)
    , next_pos(game.getFredPos())
{
    stack.emplace_back(next_pos.cellPos());
    visit(stack.back());
}

GameEvent Player::getMove()
{
    auto const &fred_pos = game.getFredPos();
    if (fred_pos == next_pos)
    {
        auto next_cell = getNextCell();
        if (next_cell.y < fred_pos.y())
            next_pos = MapPos(next_cell.x, next_cell.y, 0, 0);
        else if (next_cell.y > fred_pos.y())
            next_pos = MapPos(next_cell.x, next_cell.y, 0, 0);
        else if (game.getGameMap().isStone(next_cell, 0, 1))
            next_pos = MapPos(next_cell.x, next_cell.y, 0, 1);
        else
            next_pos = MapPos(next_cell.x, next_cell.y, 0, 0);
    }
    if (next_pos.getCharX() < fred_pos.getCharX())
        return GameEvent::LEFT;
    else if (next_pos.getCharX() > fred_pos.getCharX())
        return GameEvent::RIGHT;
    else if (next_pos.getCharY() < fred_pos.getCharY())
        return GameEvent::UP;
    else
        return GameEvent::DOWN;
}

void mainLoop(FredApp &fred_app, std::minstd_rand &random_engine)
{
    auto const &cfg = fred_app.getConfig();
    EventManager event_manager(cfg.ticks_per_frame, cfg.virtual_controller);
    Window play_window(cfg,
                       fred_app.getDisplayConfig().getGameWindowWidth(),
                       fred_app.getDisplayConfig().getGameWindowHeight());
    GameRunner game(cfg, random_engine, 0);

    game.initializeSprites(random_engine);
    play_window.setWindowPos(game.getFredCellPos());
    Player player(game);

    while (true)
    {
        fred_app.getDisplayConfig().setGameViewport();
        SDL_RenderClear(fred_app.getRenderer());
        game.render(fred_app.getTextureManager(), fred_app.getRenderer(), play_window.getGameWindow());
        play_window.renderFrame(game, fred_app.getRenderer(), fred_app.getTextureManager());
        SDL_RenderPresent(fred_app.getRenderer());
        game.playPendingSounds(fred_app.getSoundManager());

        EventMask event_mask;
        event_mask = event_manager.collectEvents(fred_app.getWindow());
        if (event_mask.check(GameEvent::QUIT))
            break;
        else if (event_mask.check(GameEvent::BACK))
            break;
        event_mask = EventMask();
        event_mask.set(player.getMove());
        auto level_status = game.update(event_mask);
        if (level_status != GameBase::LevelStatus::PLAY)
            break;
        play_window.setWindowPos(game.getFredCellPos());
    }
}


void dataLoop(Config const &cfg, std::minstd_rand &random_engine)
{
    Window play_window(cfg, 256, 192);
    GameRunner game(cfg, random_engine, 0);
    SDL_Rect capture_rect = play_window.getGameWindow().getWindowRect();
    capture_rect.x = 0;
    capture_rect.y = 0;
    sdl::SurfacePtr soft_surface(SDL_CreateRGBSurfaceWithFormat(0, capture_rect.w, capture_rect.h,
                                                                8, SDL_PIXELFORMAT_ARGB8888));
    sdl::RendererPtr soft_renderer(SDL_CreateSoftwareRenderer(soft_surface));
    TextureManager soft_tmgr(cfg, soft_renderer);
    GameWindow soft_game_window(capture_rect);

    game.initializeSprites(random_engine);
    play_window.setWindowPos(game.getFredCellPos());
    Player player(game);

    static int count = 0;
    LabelTable label_table(play_window.getGameWindow().getWindowRect());
    std::ofstream label_file("labels.txt");

    while (true)
    {
        soft_game_window.setPos(play_window.getGameWindow().getPos());
        SDL_RenderClear(soft_renderer);
        game.render(soft_tmgr, soft_renderer, soft_game_window);
        game.setLabels(label_table, play_window.getGameWindow());

        char file_name[200];
        ++count;
        std::snprintf(file_name, sizeof(file_name), "image_%04d.png", count);
        IMG_SavePNG(soft_surface, file_name);
        label_file << file_name << "\n"
                   << label_table.toString() << "\n\n\n";

        EventMask event_mask;
        event_mask.set(player.getMove());
        auto level_status = game.update(event_mask);
        if (level_status != GameBase::LevelStatus::PLAY)
            break;
        play_window.setWindowPos(game.getFredCellPos());
    }
}

int main(int argc, char* argv[])
{
    const Config cfg(argc, argv);

    std::minstd_rand random_engine;
    if (cfg.debug_map)
        random_engine.seed(1);
    else
    {
        std::random_device random_dev;
        std::uniform_int_distribution<std::uint32_t> distrib;
        random_engine.seed(distrib(random_dev));
    }
    sdl::App fred_app();
    dataLoop(cfg, random_engine);
    // FredApp fred_app(cfg, random_engine);
    // mainLoop(fred_app, random_engine);
    // fred_app.dataLoop();
    return 0;
}
