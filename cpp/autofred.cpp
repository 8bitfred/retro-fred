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


class GameWrapper
{
    Window play_window;
    GameWindow capture_window;
    GameRunner game;
    sdl::SurfacePtr capture_surface;
    sdl::RendererPtr capture_renderer;
    TextureManager capture_tmgr;
    LabelTable label_table;

public:
    GameWrapper(Config const &cfg, std::minstd_rand &random_engine,
                int width = 256, int height = 192)
        : play_window(cfg, width, height)
        , capture_window(SDL_Rect{0, 0,
                                  play_window.getGameWindow().getWindowRect().w,
                                  play_window.getGameWindow().getWindowRect().h})
        , game(cfg, random_engine, 0)
        , capture_surface(SDL_CreateRGBSurfaceWithFormat(0,
                                                         capture_window.getWindowRect().w,
                                                         capture_window.getWindowRect().h,
                                                         8, SDL_PIXELFORMAT_RGBA8888))
        , capture_renderer(SDL_CreateSoftwareRenderer(capture_surface))
        , capture_tmgr(cfg, capture_renderer)
        , label_table(play_window.getGameWindow().getWindowRect())
    {
        game.initializeSprites(random_engine);
        play_window.setWindowPos(game.getFredCellPos());
    }

    GameRunner const &getGame() const { return game; }
    SDL_Surface *getCaptureSurface() const { return capture_surface; }
    LabelTable const &getLabelTable() const { return label_table; }

    void renderToMemory()
    {
        capture_window.setPos(play_window.getGameWindow().getPos());
        SDL_RenderClear(capture_renderer);
        game.render(capture_tmgr, capture_renderer, capture_window);
        game.setLabels(label_table, play_window.getGameWindow());
    }

    void renderAndPlay(DisplayConfig const &display_cfg,
                       TextureManager const &tmgr,
                       SoundManager &smgr,
                       SDL_Renderer *renderer)
    {
        display_cfg.setGameViewport();
        SDL_RenderClear(renderer);
        game.render(tmgr, renderer, play_window.getGameWindow());
        play_window.renderFrame(game, renderer, tmgr);
        SDL_RenderPresent(renderer);
        game.playPendingSounds(smgr);

    }

    GameBase::LevelStatus update(EventMask event_mask)
    {
        auto level_status = game.update(event_mask);
        play_window.setWindowPos(game.getFredCellPos());
        return level_status;
    }
};

void dataLoop(Config const &cfg, std::minstd_rand &random_engine)
{
    sdl::App fred_app();
    GameWrapper game_wrapper(cfg, random_engine);
    Player player(game_wrapper.getGame());

    static int count = 0;
    std::ofstream label_file("labels.txt");

    while (true)
    {
        game_wrapper.renderToMemory();

        char file_name[200];
        ++count;
        std::snprintf(file_name, sizeof(file_name), "image_%04d.png", count);
        IMG_SavePNG(game_wrapper.getCaptureSurface(), file_name);
        label_file << file_name << "\n"
                   << game_wrapper.getLabelTable().toString() << "\n\n\n";

        EventMask event_mask;
        event_mask.set(player.getMove());
        if (game_wrapper.update(event_mask) != GameBase::LevelStatus::PLAY)
            break;
    }
}


void gameLoop(Config const &cfg, std::minstd_rand &random_engine)
{
    FredApp fred_app(cfg, random_engine);
    GameWrapper game_wrapper(cfg, random_engine);
    Player player(game_wrapper.getGame());

    static int count = 0;
    std::ofstream label_file("labels.txt");

    EventManager event_manager(cfg.ticks_per_frame, cfg.virtual_controller);
    while (true)
    {
        game_wrapper.renderAndPlay(fred_app.getDisplayConfig(),
                                   fred_app.getTextureManager(),
                                   fred_app.getSoundManager(),
                                   fred_app.getRenderer());
        game_wrapper.renderToMemory();

        char file_name[200];
        ++count;
        std::snprintf(file_name, sizeof(file_name), "image_%04d.png", count);
        IMG_SavePNG(game_wrapper.getCaptureSurface(), file_name);
        label_file << file_name << "\n"
                   << game_wrapper.getLabelTable().toString() << "\n\n\n";

        EventMask event_mask;
        event_mask = event_manager.collectEvents(fred_app.getWindow());
        if (event_mask.check(GameEvent::QUIT))
            break;
        else if (event_mask.check(GameEvent::BACK))
            break;

        EventMask player_event;
        player_event.set(player.getMove());
        if (game_wrapper.update(player_event) != GameBase::LevelStatus::PLAY)
            break;
    }
}


int main(int argc, char* argv[])
{
    Config cfg(argc, argv);
    cfg.infinite_power = true;
    cfg.infinite_ammo = true;
    cfg.level = 5;

    std::minstd_rand random_engine;
    if (cfg.debug_map)
        random_engine.seed(1);
    else
    {
        std::random_device random_dev;
        std::uniform_int_distribution<std::uint32_t> distrib;
        random_engine.seed(distrib(random_dev));
    }
    // example 1: run a game and collect frames and their labels
    //dataLoop(cfg, random_engine);
    // example 2: run a game, show it in a window, and collect frames and labels
    gameLoop(cfg, random_engine);
    return 0;
}
