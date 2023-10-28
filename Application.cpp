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
#include "Tomb.hpp"
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
    SDL_SetWindowTitle(getWindow(), "Fred2023");
    SDL_SetWindowIcon(getWindow(), tmgr.getFredIcon());
    SDL_RenderSetScale(getRenderer(), cfg.scale_x, cfg.scale_y);
}

FredApp::LevelStatus FredApp::playLevel(Game &game)
{
    initializeSprites(game);
    auto fred = dynamic_cast<Fred *>(game.getSpriteList(SpriteClass::FRED).front().get());

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
        {
            debugMode(game, events);
            events = 0;
        }
        fred->updateFred(events);

        checkBulletCollisions(game);
        checkCollisionsWithEnemies(game);
        if (fred->gameOver())
        {
            gameOverSequence(game);
            return LevelStatus::GAME_OVER;
        }
        else if (fred->exiting())
        {
            endOfLevelSequence(game);
            return LevelStatus::NEXT_LEVEL;
        }
        fred->checkCollisionWithObject();

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

void FredApp::endOfLevelSequence(Game &game)
{
    auto fred = dynamic_cast<Fred *>(game.getSpriteList(SpriteClass::FRED).front().get());
    game.render(getRenderer());
    SDL_Delay(cfg.ticks_per_frame);
    for (int i = static_cast<int>(SpriteClass::RAT);
         i <= static_cast<int>(SpriteClass::SMOKE); ++i)
        game.getSpriteList(static_cast<SpriteClass>(i)).clear();
    for (int i = 0; i < 4; ++i)
    {
        fred->updateFred(0);
        game.render(getRenderer());
        SDL_Delay(cfg.ticks_per_frame);
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
        fred->updateFred(0);
    }
    auto pos = game.getFredCellPos();
    pos.xadd(-2);
    game.getSpriteList(SpriteClass::FRED).pop_back();
    game.getSpriteList(SpriteClass::TOMB).emplace_back(std::make_unique<Tomb>(pos));
    game.render(getRenderer());
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
        game.getFrame().addUserOffset(-MapPos::CELL_WIDTH_PIXELS, 0);
    else if ((events & Game::EVENT_RIGHT) != 0)
        game.getFrame().addUserOffset(MapPos::CELL_WIDTH_PIXELS, 0);
    else if ((events & Game::EVENT_UP) != 0)
        game.getFrame().addUserOffset(0, -MapPos::CELL_HEIGHT_PIXELS);
    else if ((events & Game::EVENT_DOWN) != 0)
        game.getFrame().addUserOffset(0, MapPos::CELL_HEIGHT_PIXELS);
    else if ((events & Game::EVENT_MOVE_FRED) != 0)
        fred->dbgResetPosition();
    else if ((events & Game::EVENT_RESET_USER_OFFSET) != 0)
        game.getFrame().resetUserOffset();
    else if ((events & Game::EVENT_HATCH_LEFT) != 0)
        game.getGameMap().dbgMoveHatch(-1);
    else if ((events & Game::EVENT_HATCH_RIGHT) != 0)
        game.getGameMap().dbgMoveHatch(1);
    else if ((events & Game::EVENT_MOVE_TO_HATCH) != 0)
        fred->dbgMoveToHatch();
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
    SDL_Event event;
    while (SDL_PollEvent(&event))
        ;
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
