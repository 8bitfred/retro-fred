#include "Application.hpp"
#include "Config.hpp"
#include "Game.hpp"
#include "Fred.hpp"
#include "AcidDrop.hpp"
#include "Rat.hpp"
#include "Ghost.hpp"
#include "Chameleon.hpp"
#include "Mummy.hpp"
#include "Vampire.hpp"
#include "Skeleton.hpp"
#include "Bullet.hpp"
#include "Smoke.hpp"
#include "Object.hpp"
#include <iostream>
#include <cstdio>

namespace
{
    class KeyState
    {
        unsigned state = 0;

    public:
        std::pair<bool, unsigned> updateKeyState()
        {
            SDL_Event event;
            unsigned key_presses = 0;
            while (SDL_PollEvent(&event) != 0)
            {
                if (event.type == SDL_QUIT)
                    return {true, 0};
                else if (event.type == SDL_KEYDOWN)
                {
                    if (event.key.repeat == 0)
                    {
                        if ((state & (Game::EVENT_RCTRL | Game::EVENT_LCTRL)) != 0 &&
                            event.key.keysym.sym == SDLK_q)
                            return {true, 0};
                        auto events_of_key = Game::getEventOfKey(event.key.keysym.sym);
                        state |= events_of_key;
                        key_presses |= events_of_key;
                    }
                }
                else if (event.type == SDL_KEYUP)
                {
                    auto events_of_key = Game::getEventOfKey(event.key.keysym.sym);
                    state &= ~events_of_key;
                }
            }
            return {false, state | key_presses};
        }
    };

}

FredApp::FredApp(Config const &cfg, std::minstd_rand &random_engine)
    : cfg(cfg)
    , random_engine(random_engine)
    , w_and_r(sdl::createWindowAndRenderer(static_cast<int>(cfg.scale_x * cfg.window_width),
                                           static_cast<int>(cfg.scale_y * cfg.window_height)))
    , tmgr(cfg, getRenderer())
    , smgr(cfg)
    , high_scores(4, {0, ""})
{
    SDL_RenderSetScale(getRenderer(), cfg.scale_x, cfg.scale_y);
}

FredApp::LevelStatus FredApp::playLevel(Game &game)
{
    initializeSprites(game);
    auto fred = dynamic_cast<Fred *>(game.getSpriteList(SpriteClass::FRED).front().get());
    game.getGameMap().initializeMapBlocks(game.getFrame(),
                                          game.getSpriteList(SpriteClass::BLOCK));

    std::uint32_t frame_count = 0;
    KeyState key_state;
    while (true)
    {
        Uint32 const start_ticks = SDL_GetTicks();
        auto [quit, events] = key_state.updateKeyState();
        if (quit)
            return LevelStatus::QUIT;
        updateSprites(game);

        if ((events & Game::EVENT_SHIFT) != 0 && cfg.debug_keys)
            debugMode(game, events);
        else {
            fred->updateFred(game, events);
            if (fred->exiting())
            {
                endOfLevelSequence(game);
                return LevelStatus::NEXT_LEVEL;
            }
        }

        checkBulletCollisions(game);
        checkCollisionsWithEnemies(game);
        if (fred->gameOver())
        {
            gameOverSequence(game);
            return LevelStatus::GAME_OVER;
        }
        fred->checkCollisionWithObject(game);

        game.render(getRenderer());
        game.playPendingSounds();

        ++frame_count;
        Uint32 const ticks = SDL_GetTicks() - start_ticks;
        if (ticks < cfg.ticks_per_frame)
            SDL_Delay(cfg.ticks_per_frame - ticks);
    }
}

void FredApp::initializeSprites(Game &game)
{
    initializeFred(game);
    initializeAcidDrops(game);
    initializeRats(game);
    initializeGhosts(game);
    initializeChameleons(game);
    initializeMummies(game);
    initializeVampires(game);
    initializeSkeletons(game);
    initializeObjects(game);
}

void FredApp::initializeFred(Game &game)
{
    MapPos fred_initial_position;
    if (cfg.debug_map)
        fred_initial_position = {23, 31, 0, 1};
    else
    {
        CellPos fred_cell_position = {0, game.getGameMap().getHeight() - 2};
        std::uniform_int_distribution<> distrib(1, game.getGameMap().getWidth() - 2);
        while (true)
        {
            fred_cell_position.x = distrib(random_engine);
            if (game.getGameMap().getBlock(fred_cell_position) == GameMap::Cell::EMPTY)
                break;
        }
        fred_initial_position = {fred_cell_position.x, fred_cell_position.y, 0, 1};
    }
    auto fred_unique_ptr = std::make_unique<Fred>(fred_initial_position);
    game.getFrame().adjustFramePos(fred_initial_position);
    game.getSpriteList(SpriteClass::FRED).emplace_back(std::move(fred_unique_ptr));
}


void FredApp::initializeAcidDrops(Game &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::ACID_DROP);
    std::uniform_int_distribution<> distrib_x(1, game.getGameMap().getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, game.getGameMap().getHeight() - 4);
    std::uniform_int_distribution<> distrib_frame(0, 3);
    for (int i = 0; i < game.getSpriteCount().acid_drops; ++i) {
        while (true)
        {
            MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 1, 0};
            if (!game.getGameMap().isStone(pos.cellPos(), 0, -1) &&
                game.getGameMap().getBlock(pos.cellPos(), 0, -1) != GameMap::Cell::TRAPDOOR)
                continue;
            if (game.getGameMap().isStone(pos.cellPos()))
                continue;
            if (!game.getGameMap().isStone(pos.cellPos(), 0, 1))
                continue;
            if (auto cell = game.getGameMap().getBlock(pos.cellPos(), -1);
                cell == GameMap::Cell::ROPE_END ||
                cell == GameMap::Cell::ROPE_MAIN ||
                cell == GameMap::Cell::ROPE_START)
                continue;
            if (auto cell = game.getGameMap().getBlock(pos.cellPos(), 1);
                cell == GameMap::Cell::ROPE_END ||
                cell == GameMap::Cell::ROPE_MAIN ||
                cell == GameMap::Cell::ROPE_START)
                continue;
            auto initial_state = distrib_frame(random_engine);
            sprite_list.emplace_back(std::make_unique<AcidDrop>(pos, initial_state));
            break;
        }
    }
}

void FredApp::initializeRats(Game &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::RAT);
    std::uniform_int_distribution<> distrib_x(1, game.getGameMap().getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, game.getGameMap().getHeight() - 4);
    for (int i = 0; i < game.getSpriteCount().rats; ++i) {
        while (true)
        {
            MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 0, 4};
            if (game.getGameMap().isStone(pos.cellPos()))
                continue;
            if (game.getGameMap().isStone(pos.cellPos(), -1))
                continue;
            if (game.getGameMap().isStone(pos.cellPos(), 1))
                continue;
            if (!game.getGameMap().isStone(pos.cellPos(), 0, 1))
                continue;
            if (!game.getGameMap().isStone(pos.cellPos(), -1, 1))
                continue;
            if (!game.getGameMap().isStone(pos.cellPos(), 1, 1))
                continue;
            sprite_list.emplace_back(std::make_unique<Rat>(pos));
            break;
        }
    }
}

void FredApp::initializeGhosts(Game &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::GHOST);
    std::uniform_int_distribution<> distrib_x(1, game.getGameMap().getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, game.getGameMap().getHeight() - 4);
    for (int i = 0; i < game.getSpriteCount().ghosts; ++i) {
        while (true)
        {
            MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 0, 1};
            if (game.getGameMap().isStone(pos.cellPos()))
                continue;
            sprite_list.emplace_back(std::make_unique<Ghost>(pos, random_engine));
            break;
        }
    }
}

void FredApp::initializeChameleons(Game &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::CHAMELEON);
    std::uniform_int_distribution<> distrib_x(1, game.getGameMap().getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, game.getGameMap().getHeight() - 4);
    for (int i = 0; i < game.getSpriteCount().chameleons; ++i) {
        while (true)
        {
            MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 0, 0};
            if (!Chameleon::isValidCell(game.getGameMap(), pos.cellPos()))
                continue;
            sprite_list.emplace_back(std::make_unique<Chameleon>(pos, random_engine));
            break;
        }
    }
}

void FredApp::initializeMummies(Game &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::MUMMY);
    for (int i = 0; i < game.getSpriteCount().mummies; ++i) {
        sprite_list.emplace_back(std::make_unique<Mummy>(game, random_engine));
    }
}

void FredApp::initializeVampires(Game &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::VAMPIRE);
    std::uniform_int_distribution<> distrib_x(1, game.getGameMap().getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, game.getGameMap().getHeight() - 4);
    for (int i = 0; i < game.getSpriteCount().vampires; ++i) {
        while (true)
        {
            MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 0, 0};
            if (game.getGameMap().getBlock(pos.cellPos()) != GameMap::Cell::EMPTY)
                continue;
            sprite_list.emplace_back(std::make_unique<Vampire>(pos, random_engine));
            break;
        }
    }
}

void FredApp::initializeSkeletons(Game &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::SKELETON);
    std::uniform_int_distribution<> distrib_x(1, game.getGameMap().getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, game.getGameMap().getHeight() - 4);
    for (int i = 0; i < game.getSpriteCount().skeletons; ++i) {
        while (true)
        {
            MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 0, 1};
            if (game.getGameMap().getBlock(pos.cellPos()) != GameMap::Cell::EMPTY)
                continue;
            sprite_list.emplace_back(std::make_unique<Skeleton>(pos, random_engine));
            break;
        }
    }
}

void FredApp::initializeObjects(Game &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::OBJECT);
    std::uniform_int_distribution<> distrib_x(1, game.getGameMap().getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, game.getGameMap().getHeight() - 4);
    std::uniform_int_distribution<> distrib_t(0, static_cast<int>(Object::Type::COUNT));
    // TODO: the number of objects depends on the game level. Also not all objects are
    // allowed in all levels.
    for (int counter = 0; counter < game.getSpriteCount().objects;)
    {
        MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 1, 3};
        if (game.getGameMap().getBlock(pos.cellPos()) != GameMap::Cell::EMPTY)
            continue;
        Object::Type object_type = {};
        if (counter >= 16)
            object_type = Object::Type::BULLETS;
        else if (counter < static_cast<int>(Object::Type::COUNT))
            object_type = static_cast<Object::Type>(counter);
        else
            object_type = static_cast<Object::Type>(distrib_t(random_engine));
        while ((!game.getSpriteCount().has_busts && object_type == Object::Type::BUST) ||
               (!game.getSpriteCount().has_stones && object_type == Object::Type::STONE) ||
               (!game.getSpriteCount().has_masks && object_type == Object::Type::MASK))
        {
            object_type = static_cast<Object::Type>(distrib_t(random_engine));
        }
        sprite_list.emplace_back(std::make_unique<Object>(pos, object_type));
        ++counter;
    }
}


void FredApp::updateSprites(Game &game)
{
    for (auto const &sprite : game.getSpriteList(SpriteClass::BULLET))
        sprite->update(game, 0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::ACID_DROP))
        sprite->update(game, 0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::RAT))
        sprite->update(game, 0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::GHOST))
        sprite->update(game, 0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::CHAMELEON))
        sprite->update(game, 0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::MUMMY))
        sprite->update(game, 0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::VAMPIRE))
        sprite->update(game, 0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::SKELETON))
        sprite->update(game, 0);
    auto &smoke_list = game.getSpriteList(SpriteClass::SMOKE);
    for (size_t i = 0; i < smoke_list.size();)
    {
        auto &smoke = dynamic_cast<Smoke &>(*game.getSpriteList(SpriteClass::SMOKE)[i]);
        smoke.update(game, 0);
        if (!smoke.isAlive())
            smoke_list.erase(smoke_list.begin() + i);
        else
            ++i;
    }
    // TODO: I would like to avoid exposing the toggleClimbingFrame() API by using
    // some signal or callback
    Skeleton::toggleClimbingFrame();
    Mummy::toggleMummyTimer();
}

void FredApp::checkCollisionsWithEnemies(Game &game)
{
    static SpriteClass enemies[] = { SpriteClass::ACID_DROP,
                                     SpriteClass::RAT,
                                     SpriteClass::GHOST,
                                     SpriteClass::CHAMELEON,
                                     SpriteClass::MUMMY,
                                     SpriteClass::VAMPIRE,
                                     SpriteClass::SKELETON };
    auto fred = dynamic_cast<Fred *>(game.getSpriteList(SpriteClass::FRED).front().get());
    for (auto enemy_class : enemies)
    {
        for (auto const &sprite : game.getSpriteList(enemy_class))
        {
            if (fred->collisionInProgress())
                return;
            fred->checkCollisionWithEnemy(game, *sprite);
        }
    }
}

void FredApp::checkBulletCollisions(Game &game)
{
    static SpriteClass bullet_enemies[] = {
        SpriteClass::GHOST,
        SpriteClass::MUMMY,
        SpriteClass::VAMPIRE,
        SpriteClass::SKELETON,
    };

    auto &bullet_list = game.getSpriteList(SpriteClass::BULLET);
    if (bullet_list.empty())
        return;
    auto &bullet = dynamic_cast<Bullet &>(*bullet_list.back());
    if (!bullet.isAlive(game))
    {
        bullet_list.pop_back();
        return;
    }
    for (auto enemy_class : bullet_enemies)
    {
        auto &sprite_list = game.getSpriteList(enemy_class);
        for (size_t i = 0; i < sprite_list.size(); ++i)
        {
            auto &sprite = *sprite_list[i];
            if (!sprite.checkCollision(bullet))
                continue;
            auto effect = sprite.bulletHit();
            if (effect == Sprite::BulletEffect::HIT)
            {
                bullet_list.pop_back();
                return;
            }
            else if (effect == Sprite::BulletEffect::DIE)
            {
                auto &smoke_list = game.getSpriteList(SpriteClass::SMOKE);
                auto smoke_pos = sprite.getPos();
                smoke_pos.yadd(1);
                smoke_list.emplace_back(std::make_unique<Smoke>(smoke_pos));
                sprite_list.erase(sprite_list.begin() + i);
                bullet_list.pop_back();
                return;
            }
        }
    }
}

void FredApp::endOfLevelSequence(Game &game)
{
    auto fred = dynamic_cast<Fred *>(game.getSpriteList(SpriteClass::FRED).front().get());
    game.render(getRenderer());
    SDL_Delay(cfg.ticks_per_frame / 2);
    for (int i = 0; i < 4; ++i)
    {
        fred->updateFred(game, 0);
        game.render(getRenderer());
        SDL_Delay(cfg.ticks_per_frame / 2);
    }
}

void FredApp::gameOverSequence(Game &game)
{
    auto fred = dynamic_cast<Fred *>(game.getSpriteList(SpriteClass::FRED).front().get());
    game.playSound(SoundID::GAME_OVER);
    for (int i = 0; i < 6; ++i)
    {
        game.render(getRenderer());
        SDL_Delay(500);
        fred->updateFred(game, 0);
    }
    game.getSpriteList(SpriteClass::FRED).pop_back();
    auto spos = game.getFrame().getCenterCell();
    SDL_Rect game_over_pos{spos.x - 16, spos.y, 72, 40};
    SDL_Rect game_over_rect{ 1, 1, 72, 40 };
    SDL_RenderCopy(getRenderer(), tmgr.get(TextureID::GAME_OVER),
                   &game_over_rect, &game_over_pos);
    SDL_RenderPresent(getRenderer());
}

void FredApp::showLevelSummary(Game &game)
{
    SDL_RenderClear(getRenderer());
    tmgr.renderText(getRenderer(), "AT LAST YOU GOT OUT!", 24, 24,
                    206, 206, 206);
    tmgr.renderText(getRenderer(), "BONUS FOR GETTING OUT:", 24, 40,
                    206, 206, 206);
    tmgr.renderText(getRenderer(), "5000", 96, 56,
                    206, 206, 206);
    tmgr.renderText(getRenderer(), "TREASURES YOU GOT:", 24, 72,
                    206, 206, 206);
    char buf[12];
    // TODO: get rid of snprintf
    std::snprintf(buf, sizeof (buf), "%02d", game.getTreasureCount());
    tmgr.renderText(getRenderer(), buf, 96, 88, 206, 206, 206);
    tmgr.renderText(getRenderer(), "BONUS FOR TREASURES:", 24, 104,
                    206, 206, 206);
    std::snprintf(buf, sizeof (buf), "%05d", game.getTreasureCount() * 1000);
    tmgr.renderText(getRenderer(), buf, 96, 120, 206, 206, 206);
    SDL_Rect fred_puffing{88, 128, 32, 40};
    SDL_RenderCopy(getRenderer(), tmgr.get(TextureID::FRED_PUFFING),
                   nullptr, &fred_puffing);
    game.getFrame().renderFrame(game, getRenderer(), tmgr);
    SDL_RenderPresent(getRenderer());
    game.playSound(SoundID::EXIT_MAZE);
    SDL_Delay(7000);
    game.addScore(5000 + game.getTreasureCount() * 1000);
}

void FredApp::debugMode(Game &game, unsigned events)
{
    auto fred = dynamic_cast<Fred *>(game.getSpriteList(SpriteClass::FRED).front().get());
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
    else if ((events & Game::EVENT_DIE) != 0)
        fred->dbgDie();
    else if ((events & Game::EVENT_MAP) != 0)
        game.setMinimapPos(game.getFredPos().cellPos());
}

void FredApp::mainLoop()
{
    splashScreen();
    while (true)
    {
        if (state == State::QUIT)
            break;

        auto ticks = SDL_GetTicks();
        SDL_Event event;
        if (timer > 0) {
            // Empty the event queue
            while (SDL_PollEvent(&event) != 0)
            {
                if (event.type == SDL_QUIT)
                    return;
            }
            if (SDL_WaitEventTimeout(&event, timer))
            {
                timer -= SDL_GetTicks() - ticks;
                if (event.type == SDL_QUIT)
                    return;
                if (event.type == SDL_KEYDOWN)
                {
                    if (event.key.keysym.sym == SDLK_q &&
                        (event.key.keysym.mod & KMOD_CTRL) != 0)
                        return;
                    if (event.key.keysym.sym > 0x7f)
                        continue;
                }
                else
                    continue;
            }
            else
                timer = 0;
        }

        switch (state)
        {
        case State::SPLASH_SCREEN:
            menu();
            break;
        case State::MENU:
            if (timer > 0)
            {
                playGame();
                if (state == State::QUIT)
                    return;
            }
            todaysGreatest();
            break;
        case State::HIGH_SCORES:
            if (timer <= 0)
                menu();
            break;
        case State::QUIT:
            return;
        }
    }
}

void FredApp::playGame()
{
    Game game(cfg, random_engine, tmgr, smgr, high_scores.front().first);

    while (true)
    {
        auto status = playLevel(game);
        if (status == LevelStatus::NEXT_LEVEL)
        {
            showLevelSummary(game);
            game.nextLevel(random_engine);
        }
        else if (status == LevelStatus::GAME_OVER)
        {
            SDL_Delay(5000);
            if (game.getScore() > high_scores.back().first)
                enterHighScore(game.getScore());
            return;
        }
        else if (status == LevelStatus::QUIT)
        {
            state = State::QUIT;
            return;
        }
    }
}

void FredApp::splashScreen()
{
    SDL_RenderCopy(getRenderer(), tmgr.get(TextureID::SPLASH_SCREEN), nullptr, nullptr);
    tmgr.renderText(getRenderer(), "2023 REMAKE:  MIGUEL CATALINA &", 0, 176, 0, 0, 0);
    tmgr.renderText(getRenderer(), "              ALFREDO CATALINA", 0, 184, 0, 0, 0);
    SDL_RenderPresent(getRenderer());
    timer = 5000;
    state = State::SPLASH_SCREEN;
}

void FredApp::menu()
{
    SDL_RenderClear(getRenderer());
    SDL_Rect logo = {88, 8, 76, 20};
    SDL_RenderCopy(getRenderer(), tmgr.get(TextureID::FRED_LOGO), nullptr, &logo);
    tmgr.renderText(getRenderer(), "PRESS ANY KEY TO START", 40, 56, 206, 206, 206);
    tmgr.renderText(getRenderer(), "WRITTEN BY FERNANDO RADA,", 0, 104, 206, 206, 206);
    tmgr.renderText(getRenderer(), "PACO MENENDEZ & CARLOS GRANADOS.", 0, 112, 206, 206, 206);
    tmgr.renderText(getRenderer(), "       \x7f INDESCOMP SPAIN", 0, 120, 206, 206, 206);
    tmgr.renderText(getRenderer(), "PRESENTED BY QUICKSILVA", 0, 128, 206, 206, 206);
    tmgr.renderText(getRenderer(), "PRESENTATION SCREEN DESIGN:", 0, 136, 206, 206, 206);
    tmgr.renderText(getRenderer(), "JUAN DELCAN, KIKI & MA", 0, 144, 206, 206, 206);
    tmgr.renderText(getRenderer(), "CHARACTER DESIGN: GAELIC", 0, 152, 206, 206, 206);
    tmgr.renderText(getRenderer(), "PET: SENATOR & DRULY'S DUCK", 0, 160, 206, 206, 206);
    tmgr.renderText(getRenderer(), "MISTAKES: MARTA & PALOMA", 0, 168, 206, 206, 206);
    tmgr.renderText(getRenderer(), "2023 REMAKE:  MIGUEL CATALINA &", 0, 176, 206, 206, 206);
    tmgr.renderText(getRenderer(), "              ALFREDO CATALINA", 0, 184, 206, 206, 206);
    SDL_RenderPresent(getRenderer());

    timer = 5000;
    state = State::MENU;
}

void FredApp::todaysGreatest()
{
    SDL_RenderClear(getRenderer());
    SDL_RenderCopy(getRenderer(), tmgr.get(TextureID::TODAYS_GREATEST), nullptr, nullptr);
    sdl::ColorGuard color_guard(getRenderer(), 255, 255, 255, 255);
    SDL_Rect rect_initials = {24, 80, 24, 8}; // 88
    SDL_Rect rect_score = {19, 130, 35, 5};
    for (auto const &[score, name] : high_scores)
    {
        if (score == 0)
            continue;
        if (SDL_RenderFillRect(getRenderer(), &rect_initials) < 0)
            throw sdl::Error();
        if (SDL_RenderFillRect(getRenderer(), &rect_score) < 0)
            throw sdl::Error();
        tmgr.renderText(getRenderer(), name, rect_initials.x, rect_initials.y, 0, 0, 0);
        tmgr.renderScore(getRenderer(), score, rect_score.x, rect_score.y, 0, 0, 0);
        rect_initials.x += 64;
        rect_score.x += 64;
    }
    SDL_RenderPresent(getRenderer());

    timer = 8000;
    state = State::HIGH_SCORES;
    smgr.play(SoundID::FUNERAL_MARCH);
}

void FredApp::enterHighScore(unsigned score)
{
    std::string initials = "A";
    KeyState key_state;
    while (true)
    {
        SDL_RenderClear(getRenderer());
        sdl::ColorGuard color_guard(getRenderer(), 0, 206, 0, 255);
        if (SDL_RenderFillRect(getRenderer(), nullptr) < 0)
            throw sdl::Error();
        tmgr.renderText(getRenderer(), "CONGRATULATIONS", 8 * 8, 0, 0, 0, 0);
        tmgr.renderText(getRenderer(), "YOU HAVE ONE OF TODAY'S GREATEST",
                        0, 8, 0, 0, 0);
        tmgr.renderText(getRenderer(), "ENTER YOUR INITIALS WITH LEFT,",
                        0, 16, 0, 0, 0);
        tmgr.renderText(getRenderer(), "RIGHT & SPACE",
                        0, 24, 0, 0, 0);
        tmgr.renderText(getRenderer(), initials, 14 * 8, 96, 0, 0, 0);
        SDL_RenderPresent(getRenderer());

        SDL_Delay(cfg.ticks_per_frame);

        auto [quit, events] = key_state.updateKeyState();
        if (quit) {
            state = State::QUIT;
            return;
        }
        if (events & Game::EVENT_LEFT)
        {
            if (initials.back() == 'A')
                initials.back() = 'Z';
            else
                --initials.back();
        }
        else if (events & Game::EVENT_RIGHT)
        {
            if (initials.back() == 'Z')
                initials.back() = 'A';
            else
                ++initials.back();
        }
        else if (events & Game::EVENT_FIRE)
        {
            if (initials.size() == 3)
            {
                auto pos = std::upper_bound(high_scores.begin(), high_scores.end(),
                                            score, [](unsigned x, auto const &y)
                                            { return x > y.first; });
                high_scores.emplace(pos, score, std::move(initials));
                if (high_scores.size() > 4)
                    high_scores.resize(4);
                break;
            }
            else
                initials += "A";
        }
   }
}
