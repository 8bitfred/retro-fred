#include "Application.hpp"
#include "GameEvent.hpp"
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
#include "Tomb.hpp"
#include <iostream>
#include <cstdio>

FredApp::FredApp(Config const &cfg, std::minstd_rand &random_engine)
    : cfg(cfg)
    , random_engine(random_engine)
    , w_and_r(sdl::createWindowAndRenderer(static_cast<int>(cfg.scale_x * cfg.window_width),
                                           static_cast<int>(cfg.scale_y * cfg.window_height)))
    , tmgr(cfg, getRenderer())
    , smgr(cfg)
    , high_scores(4, {0, ""})
{
    SDL_SetWindowTitle(getWindow(), "Fred2023");
    SDL_SetWindowIcon(getWindow(), tmgr.getFredIcon());
    SDL_RenderSetScale(getRenderer(), cfg.scale_x, cfg.scale_y);
}

void FredApp::updateGame(Game &game, EventManager &event_manager, EventMask event_mask)
{
    auto fred = dynamic_cast<Fred *>(game.getSpriteList(SpriteClass::FRED).front().get());

    updateSprites(game);

    if (cfg.debug_keys)
        debugMode(game, event_mask);
    fred->updateFred(event_mask);

    checkBulletCollisions(game);
    checkCollisionsWithEnemies(game);
    if (game.getLevelStatus() == Game::LevelStatus::GAME_OVER)
    {
        game.playSound(SoundID::GAME_OVER);
        event_manager.setTimer(500);
        game.render(getRenderer());
        state = State::GAME_OVER_SEQUENCE;
        state_timer = 0;
        return;
    }
    else if (game.getLevelStatus() == Game::LevelStatus::NEXT_LEVEL)
    {
        transitionToNextLevel(game, event_manager);
        state = State::NEXT_LEVEL;
        return;
    }
    fred->checkCollisionWithObject();
    game.render(getRenderer());
    game.playPendingSounds();
}

void FredApp::initializeSprites(Game &game)
{
    Rat::initialize(random_engine, game);
    AcidDrop::initialize(random_engine, game);
    Ghost::initialize(random_engine, game);
    Mummy::initialize(random_engine, game);
    Vampire::initialize(random_engine, game);
    Chameleon::initialize(random_engine, game);
    Skeleton::initialize(random_engine, game);
    Object::initialize(random_engine, game);
    initializeFred(game);
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
    auto fred_unique_ptr = std::make_unique<Fred>(game, fred_initial_position);
    game.updateFredPos(fred_initial_position, 1);
    game.getSpriteList(SpriteClass::FRED).emplace_back(std::move(fred_unique_ptr));
}

void FredApp::updateSprites(Game &game)
{
    for (auto const &sprite : game.getSpriteList(SpriteClass::BULLET))
        sprite->update(0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::ACID_DROP))
        sprite->update(0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::RAT))
        sprite->update(0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::GHOST))
        sprite->update(0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::CHAMELEON))
        sprite->update(0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::MUMMY))
        sprite->update(0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::VAMPIRE))
        sprite->update(0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::SKELETON))
        sprite->update(0);
    auto &smoke_list = game.getSpriteList(SpriteClass::SMOKE);
    for (size_t i = 0; i < smoke_list.size();)
    {
        auto &smoke = dynamic_cast<Smoke &>(*game.getSpriteList(SpriteClass::SMOKE)[i]);
        smoke.update(0);
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
            fred->checkCollisionWithEnemy(*sprite);
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
    if (!bullet.isAlive())
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

void FredApp::updateGameOverSequence(Game &game, EventManager &event_manager)
{
    auto fred = dynamic_cast<Fred *>(game.getSpriteList(SpriteClass::FRED).front().get());
    ++state_timer;
    if (state_timer < 6)
    {
        fred->updateFred(EventMask());
        event_manager.setTimer(500);
    }
    else {
        auto pos = game.getFredCellPos();
        pos.xadd(-2);
        game.getSpriteList(SpriteClass::FRED).pop_back();
        game.getSpriteList(SpriteClass::TOMB).emplace_back(std::make_unique<Tomb>(pos));
        event_manager.setTimer(5000);
        state = State::GAME_OVER;
    }
    game.render(getRenderer());
}

void FredApp::transitionToNextLevel(Game &game, EventManager &event_manager)
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
    event_manager.setTimer(7000);
    state = State::NEXT_LEVEL;
}

void FredApp::debugMode(Game &game, EventMask event_mask)
{
    auto fred = dynamic_cast<Fred *>(game.getSpriteList(SpriteClass::FRED).front().get());
    if (event_mask.check(GameEvent::DBG_LEFT))
        game.getFrame().addUserOffset(-MapPos::CELL_WIDTH_PIXELS, 0);
    else if (event_mask.check(GameEvent::DBG_RIGHT))
        game.getFrame().addUserOffset(MapPos::CELL_WIDTH_PIXELS, 0);
    else if (event_mask.check(GameEvent::DBG_UP))
        game.getFrame().addUserOffset(0, -MapPos::CELL_HEIGHT_PIXELS);
    else if (event_mask.check(GameEvent::DBG_DOWN))
        game.getFrame().addUserOffset(0, MapPos::CELL_HEIGHT_PIXELS);
    else if (event_mask.check(GameEvent::DBG_CENTER_FRED))
        fred->dbgResetPosition();
    else if (event_mask.check(GameEvent::DBG_CENTER_WINDOW))
        game.getFrame().resetUserOffset();
    else if (event_mask.check(GameEvent::DBG_EXIT_LEFT))
        game.dbgModifyGameMap().dbgMoveHatch(-1);
    else if (event_mask.check(GameEvent::DBG_EXIT_RIGHT))
        game.dbgModifyGameMap().dbgMoveHatch(1);
    else if (event_mask.check(GameEvent::DBG_MOVE_TO_EXIT))
        fred->dbgMoveToHatch();
    else if (event_mask.check(GameEvent::DBG_DIE))
        fred->dbgDie();
    else if (event_mask.check(GameEvent::DBG_MAP))
        game.setMinimapPos(game.getFredPos().cellPos());
}

void FredApp::mainLoop()
{
    EventManager event_manager(cfg.ticks_per_frame);
    std::optional<Game> game;
    std::string initials;

    splashScreen();
    event_manager.setTimer(5000);
    state = State::SPLASH_SCREEN;
    while (true)
    {
        auto event_mask = event_manager.collectEvents();
        if (event_mask.check(GameEvent::QUIT))
            break;
        switch (state)
        {
        case State::SPLASH_SCREEN:
            if (event_mask.check(GameEvent::TIMER) || 
                event_mask.check(GameEvent::ANY_KEY))
            {
                state_timer = 0;
                state = State::MENU;
                menu();
                event_manager.setTimer(500);
            }
            break;
        case State::MENU:
            if (event_mask.check(GameEvent::ANY_KEY))
            {
                game.emplace(cfg, random_engine, tmgr, smgr, high_scores.front().first);
                initializeSprites(*game);
                game->render(getRenderer());
                state = State::PLAY_GAME;
            }
            else if (event_mask.check(GameEvent::TIMER))
            {
                ++state_timer;
                if (state_timer == 10)
                {
                    todaysGreatest();
                    event_manager.setTimer(8000);
                    state = State::HIGH_SCORES;
                }
                else
                {
                    menu();
                    event_manager.setTimer(500);
                }
            }
            break;
        case State::HIGH_SCORES:
            if (event_mask.check(GameEvent::TIMER))
            {
                state_timer = 0;
                state = State::MENU;
                menu();
                event_manager.setTimer(500);
            }
            break;
        case State::PLAY_GAME:
            updateGame(*game, event_manager, event_mask);
            break;
        case State::NEXT_LEVEL:
            if (event_mask.check(GameEvent::TIMER)) {
                game->addScore(5000 + game->getTreasureCount() * 1000);
                game->nextLevel(random_engine);
                initializeSprites(*game);
                game->render(getRenderer());
                state = State::PLAY_GAME;
            }
            break;
        case State::GAME_OVER_SEQUENCE:
            if (event_mask.check(GameEvent::TIMER))
                updateGameOverSequence(*game, event_manager);
            break;
        case State::GAME_OVER:
            if (event_mask.check(GameEvent::TIMER))
            {
                if (game->getScore() > high_scores.back().first)
                {
                    initials = "A";
                    renderHighScoreScreen(initials);
                    state = State::ENTER_HIGH_SCORE;
                }
                else
                {
                    todaysGreatest();
                    event_manager.setTimer(8000);
                    state = State::HIGH_SCORES;
                }
            }
            break;
        case State::ENTER_HIGH_SCORE:
            updateHighScore(initials, game->getScore(), event_manager, event_mask);
            break;
        }
    }
}

void FredApp::splashScreen()
{
    SDL_RenderCopy(getRenderer(), tmgr.get(TextureID::SPLASH_SCREEN), nullptr, nullptr);
    tmgr.renderText(getRenderer(), "2023 REMAKE:  MIGUEL CATALINA &", 0, 176, 0, 0, 0);
    tmgr.renderText(getRenderer(), "              ALFREDO CATALINA", 0, 184, 0, 0, 0);
    SDL_RenderPresent(getRenderer());
}

void FredApp::menu()
{
    SDL_RenderClear(getRenderer());
    SDL_Rect logo = {88, 8, 76, 20};
    SDL_RenderCopy(getRenderer(), tmgr.get(TextureID::FRED_LOGO), nullptr, &logo);
    if ((state_timer % 2) == 0)
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
    smgr.play(SoundID::FUNERAL_MARCH);
}

void FredApp::renderHighScoreScreen(std::string const &initials)
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
}

void FredApp::updateHighScore(std::string &initials, unsigned score,
                              EventManager &event_manager, EventMask event_mask)
{
    if (event_mask.check(GameEvent::LEFT))
    {
        if (initials.back() == 'A')
            initials.back() = 'Z';
        else
            --initials.back();
    }
    else if (event_mask.check(GameEvent::RIGHT))
    {
        if (initials.back() == 'Z')
            initials.back() = 'A';
        else
            ++initials.back();
    }
    else if (event_mask.check(GameEvent::FIRE))
    {
        if (initials.size() == 3)
        {
            auto pos = std::upper_bound(high_scores.begin(), high_scores.end(),
                                        score, [](unsigned x, auto const &y)
                                        { return x > y.first; });
            high_scores.emplace(pos, score, std::move(initials));
            if (high_scores.size() > 4)
                high_scores.resize(4);
            todaysGreatest();
            event_manager.setTimer(8000);
            state = State::HIGH_SCORES;
            return;
        }
        else
            initials += "A";
    }
    renderHighScoreScreen(initials);
}
