#include "Application.hpp"
#include "fredcore/GameEvent.hpp"
#include "fredcore/Config.hpp"
#include "fredcore/Game.hpp"
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
#include "fredcore/Controller.hpp"
#include "build_config.hpp"
#include <iostream>
#include <cstdio>
#include <fstream>

class BaseState;

struct AppState
{
    enum StateID
    {
        SPLASH_SCREEN,
        MAIN_MENU,
        CONFIG_MENU,
        TODAYS_GREATEST,
        PLAY,
        ENTER_HIGH_SCORE,
        EXIT,
        COUNT
    };
private:
    std::vector<std::unique_ptr<BaseState>> state_table;
    StateID state = SPLASH_SCREEN;

public:
    EventManager event_manager;
    unsigned new_high_score = 0;
    explicit AppState(Config &cfg);
    void set(StateID state_id, FredApp &app);
    void render(FredApp const &app);
    bool eventHandler(FredApp &app, EventMask const &event_mask);
};

class BaseState
{
public:
    virtual ~BaseState() = default;

    virtual void enter(FredApp &app, AppState &app_state) = 0;
    virtual void render(FredApp const &app) const = 0;
    virtual void eventHandler(FredApp &app,
                              AppState &app_state,
                              EventMask const &event_mask) = 0;
};

class StateSplashScreen : public BaseState
{
    static constexpr int LOADING_FRAME_TICKS = 25;
    int seq = 0;
    int counter = 5000 / LOADING_FRAME_TICKS;
    Uint32 sound_timer = 0;
    void enter(FredApp &app, AppState &app_state) final
    {
        seq = 0;
        counter = 5000 / LOADING_FRAME_TICKS;
        auto loading_ticks = app.getSoundManager().getDuration(SoundID::LOADING1);
        sound_timer = SDL_GetTicks() + loading_ticks;
        app.getSoundManager().play(SoundID::LOADING1);
        app_state.event_manager.setTimer(StateSplashScreen::LOADING_FRAME_TICKS);
    }
    void render(FredApp const &app) const final
    {
        app.getDisplayConfig().setIntroViewport();
        SDL_RenderClear(app.getRenderer());
        auto texture_id = static_cast<TextureID>(static_cast<int>(TextureID::LOADING1) + seq);
        SDL_RenderCopy(app.getRenderer(),
                       app.getTextureManager().get(texture_id), nullptr, nullptr);
        SDL_Rect splash_rect{32, 24, 256 - 64, 192 - 48};
        SDL_RenderCopy(app.getRenderer(),
                       app.getTextureManager().get(TextureID::SPLASH_SCREEN), nullptr, &splash_rect);
        SDL_RenderPresent(app.getRenderer());
    }
    void eventHandler(FredApp &app,
                      AppState &app_state,
                      EventMask const &event_mask) final
    {
        if (event_mask.check(GameEvent::ANY_KEY) ||
            (event_mask.check(GameEvent::TIMER) && counter <= 0))
        {
            app.getSoundManager().clearQueuedAudio();
            app_state.set(AppState::MAIN_MENU, app);
        }
        else if (event_mask.check(GameEvent::TIMER))
        {
            --counter;
            std::uniform_int_distribution<> distrib(0, 8 * 4 - 1);
            if (auto pick = distrib(app.getRandomEngine()); pick > 6 * 4)
                seq = pick % 4;
            else
                seq = (seq + 1) % 4;
            if ((sound_timer - SDL_GetTicks()) < 100)
            {
                auto sound_id = static_cast<SoundID>(static_cast<int>(SoundID::LOADING1) + seq);
                auto loading_ticks = app.getSoundManager().getDuration(sound_id);
                sound_timer += loading_ticks;
                app.getSoundManager().play(sound_id);
            }
            app_state.event_manager.setTimer(StateSplashScreen::LOADING_FRAME_TICKS);
        }
        else if (event_mask.check(GameEvent::BACK) && app.getConfig().quit_option)
            app_state.set(AppState::EXIT, app);
    }
};

class StateMainMenu : public BaseState
{
    enum MainMenu
    {
        MAIN_MENU_OPTIONS,
        MAIN_MENU_PLAY,
        MAIN_MENU_QUIT,
    };
    int counter = 0;
    Menu main_menu;
    void enter(FredApp &, AppState &app_state) final
    {
        counter = 0;
        app_state.event_manager.setTimer(500);
    }
    void render(FredApp const &app) const final
    {
        app.getDisplayConfig().setIntroViewport();
        auto renderer = app.getRenderer();
        auto const &tmgr = app.getTextureManager();
        SDL_RenderClear(renderer);
        SDL_Rect logo = {88, 8, 76, 20};
        SDL_RenderCopy(renderer, tmgr.get(TextureID::FRED_LOGO), nullptr, &logo);
        main_menu.render(renderer, tmgr);
        tmgr.renderText(renderer, "WRITTEN BY FERNANDO RADA,", 0, 88, 206, 206, 206);
        tmgr.renderText(renderer, "PACO MENENDEZ & CARLOS GRANADOS.", 0, 96, 206, 206, 206);
        tmgr.renderText(renderer, "       \x7f INDESCOMP SPAIN", 0, 112, 206, 206, 206);
        tmgr.renderText(renderer, "PRESENTED BY QUICKSILVA", 0, 120, 206, 206, 206);
        tmgr.renderText(renderer, "PRESENTATION SCREEN DESIGN:", 0, 128, 206, 206, 206);
        tmgr.renderText(renderer, "JUAN DELCAN, KIKI & MA", 0, 136, 206, 206, 206);
        tmgr.renderText(renderer, "CHARACTER DESIGN: GAELIC", 0, 144, 206, 206, 206);
        tmgr.renderText(renderer, "PET: SENATOR & DRULY'S DUCK", 0, 152, 206, 206, 206);
        tmgr.renderText(renderer, "MISTAKES: MARTA & PALOMA", 0, 160, 206, 206, 206);
        tmgr.renderText(renderer, "RETRO-FRED " RETRO_FRED_VERSION_STRING " \x7f 2024-2025",
                        0, 168, 206, 206, 0);
        tmgr.renderText(renderer, "    MIGUEL CATALINA", 0, 176, 206, 206, 0);
        tmgr.renderText(renderer, "    ALFREDO CATALINA", 0, 184, 206, 206, 0);
        app.renderController();
        SDL_RenderPresent(renderer);
    }
    void eventHandler(FredApp &app,
                      AppState &app_state,
                      EventMask const &event_mask) final
    {
        if (event_mask.check(GameEvent::BACK) || event_mask.check(GameEvent::ESCAPE))
        {
            if (app.getConfig().quit_option)
                app_state.set(AppState::EXIT, app);
        }
        else if (event_mask.check(GameEvent::TIMER))
        {
            ++counter;
            if (counter == 10)
                app_state.set(AppState::TODAYS_GREATEST, app);
            else
                app_state.event_manager.setTimer(500);
        }
        else
        {
            if (event_mask.check(GameEvent::UP) || event_mask.check(GameEvent::DOWN))
                counter = 0;
            main_menu.eventHandler(event_mask, app.getSoundManager());
            if (main_menu.isSelected(MAIN_MENU_PLAY))
                app_state.set(AppState::PLAY, app);
            else if (main_menu.isSelected(MAIN_MENU_OPTIONS))
                app_state.set(AppState::CONFIG_MENU, app);
            else if (main_menu.isSelected(MAIN_MENU_QUIT))
                app_state.set(AppState::EXIT, app);
        }
    }
public:
    StateMainMenu(bool has_quit_option)
        : main_menu(SDL_Rect{88, 40, 168, 24})
    {
        main_menu.addItem(std::make_unique<MenuItem>("OPTIONS"), MAIN_MENU_OPTIONS);
        main_menu.addItem(std::make_unique<MenuItem>("PLAY"), MAIN_MENU_PLAY, true);
        if (has_quit_option)
            main_menu.addItem(std::make_unique<MenuItem>("QUIT"), MAIN_MENU_QUIT);
    }
};

class StateConfigMenu : public BaseState
{
    enum ConfigMenu
    {
        CONFIG_MENU_BACK,
        CONFIG_MENU_CLASSIC,
        CONFIG_MENU_NORMAL,
    };
    Config &app_cfg;
    Menu config_menu;

    void enter(FredApp &, AppState &) final {}
    void render(FredApp const &app) const final
    {
        app.getDisplayConfig().setIntroViewport();
        SDL_RenderClear(app.getRenderer());
        SDL_Rect logo = {88, 8, 76, 20};
        SDL_RenderCopy(app.getRenderer(), app.getTextureManager().get(TextureID::FRED_LOGO), nullptr, &logo);
        config_menu.render(app.getRenderer(), app.getTextureManager());
        app.renderController();
        SDL_RenderPresent(app.getRenderer());
    }
    void eventHandler(FredApp &app,
                      AppState &app_state,
                      EventMask const &event_mask) final
    {
        if (event_mask.check(GameEvent::BACK) || event_mask.check(GameEvent::ESCAPE))
        {
            app_state.set(AppState::MAIN_MENU, app);
            app.saveConfig();
        }
        else
        {
            config_menu.eventHandler(event_mask, app.getSoundManager());
            if (config_menu.isSelected(CONFIG_MENU_BACK))
            {
                app_state.set(AppState::MAIN_MENU, app);
                app.saveConfig();
            }
            else if (config_menu.isSelected(CONFIG_MENU_CLASSIC))
                app_cfg.classicMode();
            else if (config_menu.isSelected(CONFIG_MENU_NORMAL))
                app_cfg.normalMode();
        }
    }

public:
    StateConfigMenu(Config &cfg)
        : app_cfg(cfg), config_menu(SDL_Rect{16, 56, 224, 160})
    {
        config_menu.addItem(std::make_unique<MenuItem>("BACK TO MAIN MENU"), CONFIG_MENU_BACK);
        config_menu.addItem(std::make_unique<MenuItem>("Classic mode"), CONFIG_MENU_CLASSIC);
        config_menu.addItem(std::make_unique<MenuItem>("Normal mode"), CONFIG_MENU_NORMAL);
        config_menu.addItem(std::make_unique<CheckBox>("Minimap tracker", &cfg.minimap_tracker));
        config_menu.addItem(std::make_unique<CheckBox>("Show exit on minimap", &cfg.minimap_exit));
        config_menu.addItem(std::make_unique<CheckBox>("Reset power on new level", &cfg.refill_power));
        config_menu.addItem(std::make_unique<CheckBox>("Reset ammo on new level", &cfg.refill_bullets));
        config_menu.addItem(std::make_unique<CheckBox>("Level-dependent power up", &cfg.set_power_with_level));
        config_menu.addItem(std::make_unique<CheckBox>("Level-dependent bullets up", &cfg.set_bullets_with_level));
    }
};

class StateTodaysGreatest : public BaseState
{
    void enter(FredApp &app, AppState &app_state) final
    {
        app.getSoundManager().play(SoundID::FUNERAL_MARCH);
        app_state.event_manager.setTimer(8000);

    }
    void render(FredApp const &app) const final
    {
        app.getDisplayConfig().setIntroViewport();
        SDL_RenderClear(app.getRenderer());
        SDL_RenderCopy(app.getRenderer(), app.getTextureManager().get(TextureID::TODAYS_GREATEST),
                       nullptr, nullptr);
        sdl::ColorGuard color_guard(app.getRenderer(), 255, 255, 255, 255);
        SDL_Rect rect_initials = {24, 80, 24, 8}; // 88
        SDL_Rect rect_score = {19, 130, 35, 5};
        for (auto const &[score, name] : app.getHighScores())
        {
            if (score == 0)
                continue;
            if (SDL_RenderFillRect(app.getRenderer(), &rect_initials) < 0)
                throw sdl::Error();
            if (SDL_RenderFillRect(app.getRenderer(), &rect_score) < 0)
                throw sdl::Error();
            app.getTextureManager().renderText(app.getRenderer(), name,
                                               rect_initials.x, rect_initials.y, 0, 0, 0);
            app.getTextureManager().renderScore(app.getRenderer(), score,
                                                rect_score.x, rect_score.y, 0, 0, 0);
            rect_initials.x += 64;
            rect_score.x += 64;
        }
        SDL_RenderPresent(app.getRenderer());
    }
    void eventHandler(FredApp &app,
                      AppState &app_state,
                      EventMask const &event_mask) final
    {
        if (event_mask.check(GameEvent::TIMER))
            app_state.set(AppState::MAIN_MENU, app);
        else if (event_mask.check(GameEvent::BACK))
        {
            app.getSoundManager().clearQueuedAudio();
            app_state.set(AppState::MAIN_MENU, app);
        }
    }
};

class StatePlay : public BaseState
{
    enum GameMenu
    {
        GAME_MENU_BACK,
        GAME_MENU_QUIT,
        GAME_MENU_CLASSIC,
        GAME_MENU_NORMAL,
        GAME_MENU_CHEATS,
    };
    enum CheatsMenu
    {
        CHEATS_MENU_BACK,
        CHEATS_MENU_REFILL_POWER,
        CHEATS_MENU_REFILL_AMMO,
        CHEATS_MENU_MOVE_TO_EXIT,
        CHEATS_MENU_MINIMAP,
    };
    enum class PauseMode
    {
        OFF,
        GAME_MENU,
        CHEATS_MENU,
    };
    Config &game_cfg;
    Menu game_menu, cheats_menu;
    std::optional<Window> play_window;
    std::optional<GameRunner> game;
    int counter = 0;
    PauseMode pause_mode = PauseMode::OFF;
    bool cheat = false;
    void enter(FredApp &app, AppState &) final
    {
        game_cfg.resetCheats();
        play_window.emplace(game_cfg, app.getDisplayConfig().getGameWindowRect());
        game.emplace(game_cfg, app.getRandomEngine(),
                     app.getHighScores().front().first);
        game->initializeSprites(app.getRandomEngine());
        play_window->setWindowPos(game->getFredCellPos());
        counter = 0;
        pause_mode = PauseMode::OFF;
        cheat = false;
    }
    void renderLevelTransition(FredApp const &app) const
    {
        app.getDisplayConfig().setIntroViewport();
        auto const &tmgr = app.getTextureManager();
        SDL_RenderClear(app.getRenderer());
        tmgr.renderText(app.getRenderer(), "AT LAST YOU GOT OUT!", 24, 24,
                        206, 206, 206);
        tmgr.renderText(app.getRenderer(), "BONUS FOR GETTING OUT:", 24, 40,
                        206, 206, 206);
        tmgr.renderText(app.getRenderer(), "5000", 96, 56,
                        206, 206, 206);
        tmgr.renderText(app.getRenderer(), "TREASURES YOU GOT:", 24, 72,
                        206, 206, 206);
        char buf[12];
        // TODO: get rid of snprintf
        std::snprintf(buf, sizeof(buf), "%02d", game->getTreasureCount());
        tmgr.renderText(app.getRenderer(), buf, 96, 88, 206, 206, 206);
        tmgr.renderText(app.getRenderer(), "BONUS FOR TREASURES:", 24, 104,
                        206, 206, 206);
        std::snprintf(buf, sizeof(buf), "%05d", game->getTreasureCount() * 1000);
        tmgr.renderText(app.getRenderer(), buf, 96, 120, 206, 206, 206);
        SDL_Rect fred_puffing{88, 128, 32, 40};
        SDL_RenderCopy(app.getRenderer(), tmgr.get(TextureID::FRED_PUFFING),
                       nullptr, &fred_puffing);
        play_window->renderFrame(*game, app.getDisplayConfig(), tmgr);
        SDL_RenderPresent(app.getRenderer());
    }

    void render(FredApp const &app) const final
    {
        if (game->getLevelStatus() == GameBase::LevelStatus::NEXT_LEVEL)
            renderLevelTransition(app);
        else
        {
            app.getDisplayConfig().setGameViewport();
            SDL_RenderClear(app.getRenderer());
            game->render(app.getTextureManager(),
                         app.getRenderer(), play_window->getGameWindow());
            play_window->renderFrame(*game, app.getDisplayConfig(), app.getTextureManager());
            app.getDisplayConfig().setIntroViewport();
            if (pause_mode == PauseMode::GAME_MENU)
                game_menu.render(app.getRenderer(), app.getTextureManager());
            else if (pause_mode == PauseMode::CHEATS_MENU)
                cheats_menu.render(app.getRenderer(), app.getTextureManager());
            app.renderController();
            SDL_RenderPresent(app.getRenderer());
        }
    }

    void moveToExit()
    {
        auto fred = dynamic_cast<Fred *>(game->getSpriteList(SpriteClass::FRED).front().get());
        if (fred->dbgMoveToHatch())
        {
            play_window->setWindowPos(game->getFredCellPos());
            play_window->resetUserOffset();
        }
    }

    void debugMode(EventMask event_mask)
    {
        auto fred = dynamic_cast<Fred *>(game->getSpriteList(SpriteClass::FRED).front().get());
        if (event_mask.check(GameEvent::DBG_CENTER_FRED))
        {
            if (fred->dbgResetPosition(play_window->getCenter()))
            {
                play_window->setWindowPos(game->getFredCellPos());
                play_window->resetUserOffset();
            }
        }
        else if (event_mask.check(GameEvent::DBG_MOVE_TO_EXIT))
            moveToExit();
        else if (event_mask.check(GameEvent::DBG_DIE))
            fred->dbgDie();
    }

    void updateGame(FredApp &app, AppState &app_state, EventMask event_mask)
    {
        play_window->update(event_mask);
        game->update(event_mask);
        if (game_cfg.debug_keys)
            debugMode(event_mask);
        auto level_status = game->getLevelStatus();
        if (level_status == GameBase::LevelStatus::GAME_OVER)
        {
            app.getSoundManager().play(SoundID::GAME_OVER);
            app_state.event_manager.setTimer(500);
        }
        else if (level_status == GameBase::LevelStatus::NEXT_LEVEL)
        {
            app.getSoundManager().play(SoundID::EXIT_MAZE);
            app_state.event_manager.setTimer(7000);
        }
        else
        {
            play_window->setWindowPos(game->getFredCellPos());
            game->playPendingSounds(app.getSoundManager());
        }
    }

    void setEndOfGameState(FredApp &app, AppState &app_state)
    {
        // Check if the score is higher than the lowest high score. We only add a score to
        // the high score list if no cheats were used during the game.
        if (game->getScore() > app.getHighScores().back().first && !cheat)
        {
            app_state.new_high_score = game->getScore();
            app_state.set(AppState::ENTER_HIGH_SCORE, app);
        }
        else
            app_state.set(AppState::TODAYS_GREATEST, app);
    }

    void handleGameMenuEvent(FredApp &app,
                             AppState &app_state,
                             EventMask const &event_mask)
    {
        if (event_mask.check(GameEvent::BACK) || event_mask.check(GameEvent::ESCAPE))
        {
            pause_mode = PauseMode::OFF;
            app.saveConfig();
        }
        else
        {
            game_menu.eventHandler(event_mask, app.getSoundManager());
            if (game_menu.isSelected(GAME_MENU_BACK))
            {
                pause_mode = PauseMode::OFF;
                app.saveConfig();
            }
            else if (game_menu.isSelected(GAME_MENU_QUIT))
                app_state.set(AppState::MAIN_MENU, app);
            else if (game_menu.isSelected(GAME_MENU_CLASSIC))
                game_cfg.classicMode();
            else if (game_menu.isSelected(GAME_MENU_NORMAL))
                game_cfg.normalMode();
            else if (game_menu.isSelected(GAME_MENU_CHEATS))
                pause_mode = PauseMode::CHEATS_MENU;
        }
    }

    void handleCheatsMenuEvent(FredApp &app,
                              AppState &,
                              EventMask const &event_mask)
    {
        if (event_mask.check(GameEvent::BACK) || event_mask.check(GameEvent::ESCAPE))
            pause_mode = PauseMode::GAME_MENU;
        else
        {
            cheats_menu.eventHandler(event_mask, app.getSoundManager());
            if (cheats_menu.isSelected(GAME_MENU_BACK))
                pause_mode = PauseMode::GAME_MENU;
            else if (cheats_menu.isSelected(CHEATS_MENU_REFILL_POWER))
            {
                game->resetPower();
                cheat = true;
            }
            else if (cheats_menu.isSelected(CHEATS_MENU_REFILL_AMMO))
            {
                game->resetBullets();
                cheat = true;
            }
            else if (cheats_menu.isSelected(CHEATS_MENU_MOVE_TO_EXIT))
            {
                moveToExit();
                cheat = true;
            }
            else if (cheats_menu.isSelected(CHEATS_MENU_MINIMAP))
            {
                game->setMinimapPos(game->getFredPos().cellPos());;
                cheat = true;
            }
            if (game_cfg.infinite_power || game_cfg.infinite_ammo)
                cheat = true;
        }
    }

    void eventHandler(FredApp &app,
                      AppState &app_state,
                      EventMask const &event_mask) final
    {
        if (game->getLevelStatus() == GameBase::LevelStatus::PLAY)
        {
            if (pause_mode == PauseMode::GAME_MENU)
                handleGameMenuEvent(app, app_state, event_mask);
            else if (pause_mode == PauseMode::CHEATS_MENU)
                handleCheatsMenuEvent(app, app_state, event_mask);
            else if (event_mask.check(GameEvent::BACK) || event_mask.check(GameEvent::ESCAPE))
                pause_mode = PauseMode::GAME_MENU;
            else
                updateGame(app, app_state, event_mask);
        }
        else if (game->getLevelStatus() == GameBase::LevelStatus::NEXT_LEVEL)
        {
            if (event_mask.check(GameEvent::TIMER) || event_mask.check(GameEvent::BACK))
            {
                app.getSoundManager().clearQueuedAudio();
                game->addScore(5000 + game->getTreasureCount() * 1000);
                game->nextLevel(app.getRandomEngine());
                game->initializeSprites(app.getRandomEngine());
                play_window->setWindowPos(game->getFredCellPos());
            }
        }
        else if (game->getLevelStatus() == GameBase::LevelStatus::GAME_OVER)
        {
            if (event_mask.check(GameEvent::BACK))
            {
                app.getSoundManager().clearQueuedAudio();
                setEndOfGameState(app, app_state);
            }
            else if (event_mask.check(GameEvent::TIMER))
            {
                ++counter;
                if (counter < 6)
                {
                    auto fred = dynamic_cast<Fred*>(game->getSpriteList(SpriteClass::FRED).front().get());
                    fred->updateFred(EventMask());
                    app_state.event_manager.setTimer(500);
                }
                else if (counter == 6)
                {
                    auto pos = game->getFredCellPos();
                    pos.xadd(-2);
                    game->getSpriteList(SpriteClass::FRED).pop_back();
                    game->getSpriteList(SpriteClass::TOMB).emplace_back(std::make_unique<Tomb>(pos));
                    app_state.event_manager.setTimer(5000);
                }
                else
                    setEndOfGameState(app, app_state);
            }
        }
    }

public:
    explicit StatePlay(Config &cfg)
        : game_cfg(cfg)
        , game_menu(SDL_Rect{16, 32, 224, 104})
        , cheats_menu(SDL_Rect{16, 32, 224, 72})
    {
        game_menu.addItem(std::make_unique<MenuItem>("BACK TO GAME"), GAME_MENU_BACK);
        game_menu.addItem(std::make_unique<MenuItem>("QUIT GAME"), GAME_MENU_QUIT);
        game_menu.addItem(std::make_unique<MenuItem>("Classic mode"), GAME_MENU_CLASSIC);
        game_menu.addItem(std::make_unique<MenuItem>("Normal mode"), GAME_MENU_NORMAL);
        game_menu.addItem(std::make_unique<CheckBox>("Minimap tracker", &game_cfg.minimap_tracker));
        game_menu.addItem(std::make_unique<CheckBox>("Show exit on minimap", &game_cfg.minimap_exit));
        game_menu.addItem(std::make_unique<CheckBox>("Reset power on new level", &game_cfg.refill_power));
        game_menu.addItem(std::make_unique<CheckBox>("Reset ammo on new level", &game_cfg.refill_bullets));
        game_menu.addItem(std::make_unique<CheckBox>("Level-dependent power up", &game_cfg.set_power_with_level));
        game_menu.addItem(std::make_unique<CheckBox>("Level-dependent ammo up", &game_cfg.set_bullets_with_level));
        game_menu.addItem(std::make_unique<MenuItem>("POKEs"), GAME_MENU_CHEATS);

        cheats_menu.addItem(std::make_unique<MenuItem>("BACK"), CHEATS_MENU_BACK);
        cheats_menu.addItem(std::make_unique<CheckBox>("Infinite power", &game_cfg.infinite_power));
        cheats_menu.addItem(std::make_unique<CheckBox>("Infinite ammo", &game_cfg.infinite_ammo));
        cheats_menu.addItem(std::make_unique<MenuItem>("Refill power"), CHEATS_MENU_REFILL_POWER);
        cheats_menu.addItem(std::make_unique<MenuItem>("Refill ammo"), CHEATS_MENU_REFILL_AMMO);
        cheats_menu.addItem(std::make_unique<MenuItem>("Move to exit"), CHEATS_MENU_MOVE_TO_EXIT);
        cheats_menu.addItem(std::make_unique<MenuItem>("Set minimap"), CHEATS_MENU_MINIMAP);
    }
};

class StateEnterHighScore : public BaseState
{
    std::string initials = "A";
    size_t index = 0;

    void enter(FredApp &, AppState &) final
    {
        initials = "A";
        index = 0;
    }
    void render(FredApp const &app) const final
    {
        app.getDisplayConfig().setIntroViewport();
        SDL_RenderClear(app.getRenderer());
        auto const &tmgr = app.getTextureManager();
        sdl::ColorGuard color_guard(app.getRenderer(), 0, 206, 0, 255);
        if (SDL_RenderFillRect(app.getRenderer(), nullptr) < 0)
            throw sdl::Error();
        tmgr.renderText(app.getRenderer(), "CONGRATULATIONS", 8 * 8, 0, 0, 0, 0);
        tmgr.renderText(app.getRenderer(), "YOU HAVE ONE OF TODAY'S GREATEST",
                        0, 8, 0, 0, 0);
        tmgr.renderText(app.getRenderer(), "ENTER YOUR INITIALS WITH LEFT,",
                        0, 16, 0, 0, 0);
        tmgr.renderText(app.getRenderer(), "RIGHT & FIRE",
                        0, 24, 0, 0, 0);
        tmgr.renderText(app.getRenderer(), initials, 14 * 8, 96, 0, 0, 0);
        for (size_t i = initials.size(); i < 3; ++i)
            tmgr.renderText(app.getRenderer(), "_", static_cast<int>(14 + i) * 8, 96, 0, 0, 0);
        app.renderController();
        SDL_RenderPresent(app.getRenderer());
    }
    void eventHandler(FredApp &app,
                      AppState &app_state,
                      EventMask const &event_mask) final
    {
        static std::string_view alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ<";
        if (event_mask.check(GameEvent::LEFT))
        {
            index = (index + alphabet.size() - 1) % alphabet.size();
            if (initials.size() == 1 && alphabet[index] == '<')
                index = alphabet.size() - 2;
        }
        else if (event_mask.check(GameEvent::RIGHT))
        {
            index = (index + 1) % alphabet.size();
            if (initials.size() == 1 && alphabet[index] == '<')
                index = 0;
        }
        else if (event_mask.check(GameEvent::FIRE))
        {
            if (alphabet[index] == '<')
            {
                if (initials.size() > 1)
                    initials.pop_back();
            }
            else if (initials.size() == 3)
            {
                app.addHighScore(app_state.new_high_score, initials);
                app_state.set(AppState::TODAYS_GREATEST, app);
                return;
            }
            else
                initials += "A";
            index = 0;
        }
        initials.back() = alphabet[index];
    }
};

AppState::AppState(Config &cfg)
    : state_table(COUNT)
    , event_manager(cfg.ticks_per_frame)
{
    state_table[SPLASH_SCREEN] = std::make_unique<StateSplashScreen>();
    state_table[MAIN_MENU] = std::make_unique<StateMainMenu>(cfg.quit_option);
    state_table[CONFIG_MENU] = std::make_unique<StateConfigMenu>(cfg);
    state_table[TODAYS_GREATEST] = std::make_unique<StateTodaysGreatest>();
    state_table[PLAY] = std::make_unique<StatePlay>(cfg);
    state_table[ENTER_HIGH_SCORE] = std::make_unique<StateEnterHighScore>();
}

void AppState::set(StateID state_id, FredApp &app)
{
    state = state_id;
    if (state_table[state])
        state_table[state]->enter(app, *this);
}

void AppState::render(FredApp const &app)
{
    if (state_table[state])
        state_table[state]->render(app);
}

bool AppState::eventHandler(FredApp &app, EventMask const &event_mask)
{
    if (state_table[state])
        state_table[state]->eventHandler(app, *this, event_mask);
    return static_cast<bool>(state_table[state]);
}

FredApp::FredApp(Config const &cfg, std::minstd_rand &random_engine)
    : cfg(cfg)
    , random_engine(random_engine)
    , display_cfg(cfg)
    , tmgr(cfg, getRenderer()), smgr(cfg)
    , high_scores(4, {0, ""})
{
    auto pref_path = getPrefPath();
    high_scores_path = pref_path + "/" + "high_scores.tbl";
    config_path = pref_path + "/" + "retro-fred.cfg";
    this->cfg.load(config_path);
    loadHighScores();
    SDL_SetWindowTitle(getWindow(), "Retro-Fred");
    SDL_SetWindowIcon(getWindow(), tmgr.getFredIcon());
    if (cfg.virtual_controller)
        controller.emplace(display_cfg, cfg.back_button);
}

std::string FredApp::getPrefPath()
{
    auto pref_path_ptr = SDL_GetPrefPath("8bitfred", "Retro-Fred");
    std::string pref_path_str(pref_path_ptr);
    SDL_free(pref_path_ptr);
    return pref_path_str;
}

void FredApp::saveHighScores() const
{
    std::ofstream hs_file(high_scores_path);
    for (auto const &[score, name]: high_scores)
    {
        if (score == 0 || name.empty())
            continue;
        hs_file << name << " " << score << std::endl;
    }
}

void FredApp::loadHighScores()
{
    std::ifstream hs_file(high_scores_path);
    if (!hs_file.is_open())
        return;
    for (auto &[score, name]: high_scores)
    {
        std::string input_name;
        int input_score;
        hs_file >> input_name >> input_score;
        if (hs_file.fail() || hs_file.eof())
            break;
        score = input_score;
        name = input_name;
    }
}

void FredApp::addHighScore(unsigned score, std::string const &initials)
{
    auto pos = std::upper_bound(high_scores.begin(), high_scores.end(),
                                score, [](unsigned x, auto const &y)
                                { return x > y.first; });
    high_scores.emplace(pos, score, initials);
    if (high_scores.size() > 4)
        high_scores.resize(4);
    saveHighScores();
}

void FredApp::saveConfig() const
{
    cfg.save(config_path);
}

void FredApp::renderController() const
{
    if (controller)
        controller->render(getRenderer(), getTextureManager());
}

void FredApp::mainLoop()
{
    AppState app_state(cfg);
    app_state.set(AppState::SPLASH_SCREEN, *this);
    while (true)
    {
        if (controller)
            controller->resetPosition(display_cfg);
        app_state.render(*this);
        auto event_mask = app_state.event_manager.collectEvents(controller);
        if (event_mask.check(GameEvent::QUIT))
        {
            if (cfg.quit_option)
                break;
        }
        if (!app_state.eventHandler(*this, event_mask))
        {
            if (cfg.quit_option)
               break;
        }
    }
}
