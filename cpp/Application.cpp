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
        auto const &wav_data = app.getSoundManager().get(SoundID::LOADING1);
        sound_timer = SDL_GetTicks() + wav_data.getLenTicks();
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
                auto const &wav_data = app.getSoundManager().get(sound_id);
                sound_timer += wav_data.getLenTicks();
                app.getSoundManager().play(sound_id);
            }
            app_state.event_manager.setTimer(StateSplashScreen::LOADING_FRAME_TICKS);
        }
        else if (event_mask.check(GameEvent::BACK))
            app_state.set(AppState::EXIT, app);
    }
};

class StateMainMenu : public BaseState
{
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
        tmgr.renderText(renderer, "RETRO-FRED \x7f 2024", 0, 168, 206, 206, 0);
        tmgr.renderText(renderer, "    MIGUEL CATALINA", 0, 176, 206, 206, 0);
        tmgr.renderText(renderer, "    ALFREDO CATALINA", 0, 184, 206, 206, 0);
        if (app.getConfig().virtual_controller)
            Controller::render(app.getWindow(), app.getRenderer(), app.getTextureManager());
        SDL_RenderPresent(renderer);
    }
    void eventHandler(FredApp &app,
                      AppState &app_state,
                      EventMask const &event_mask) final
    {
        if (event_mask.check(GameEvent::BACK))
            app_state.set(AppState::EXIT, app);
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
            main_menu.eventHandler(event_mask);
            if (main_menu.isSelected(1))
                app_state.set(AppState::PLAY, app);
            else if (main_menu.isSelected(0))
                app_state.set(AppState::CONFIG_MENU, app);
        }
    }
public:
    StateMainMenu()
        : main_menu(SDL_Rect{88, 56, 168, 16})
    {
        main_menu.addItem(std::make_unique<MenuItem>("OPTIONS"));
        main_menu.addItem(std::make_unique<MenuItem>("PLAY"), true);
    }
};

class StateConfigMenu : public BaseState
{
    Menu config_menu;

    void enter(FredApp &, AppState &) final {}
    void render(FredApp const &app) const final
    {
        app.getDisplayConfig().setIntroViewport();
        SDL_RenderClear(app.getRenderer());
        SDL_Rect logo = {88, 8, 76, 20};
        SDL_RenderCopy(app.getRenderer(), app.getTextureManager().get(TextureID::FRED_LOGO), nullptr, &logo);
        config_menu.render(app.getRenderer(), app.getTextureManager());
        if (app.getConfig().virtual_controller)
            Controller::render(app.getWindow(), app.getRenderer(), app.getTextureManager());
        SDL_RenderPresent(app.getRenderer());
    }
    void eventHandler(FredApp &app,
                      AppState &app_state,
                      EventMask const &event_mask) final
    {
        config_menu.eventHandler(event_mask);
        if (config_menu.isSelected(0))
            app_state.set(AppState::MAIN_MENU, app);
    }

public:
    StateConfigMenu(Config &cfg)
        : config_menu(SDL_Rect{16, 56, 224, 160})
    {
        config_menu.addItem(std::make_unique<MenuItem>("BACK TO MAIN MENU"));
        config_menu.addItem(std::make_unique<CheckBox>("Infinite power", &cfg.infinite_power));
        config_menu.addItem(std::make_unique<CheckBox>("Infinite ammo", &cfg.infinite_ammo));
        config_menu.addItem(std::make_unique<CheckBox>("Reset power on new level", &cfg.replenish_power));
        config_menu.addItem(std::make_unique<CheckBox>("Reset ammo on new level", &cfg.replenish_bullets));
        config_menu.addItem(std::make_unique<CheckBox>("Minimap tracker", &cfg.minimap_tracker));
        config_menu.addItem(std::make_unique<CheckBox>("Show exit on minimap", &cfg.minimap_exit));
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
    std::optional<Window> play_window;
    std::optional<GameRunner> game;
    int counter = 0;
    void enter(FredApp &app, AppState &) final
    {
        counter = 0;
        play_window.emplace(app.getConfig(),
                            app.getDisplayConfig().getGameWindowWidth(),
                            app.getDisplayConfig().getGameWindowHeight());
        game.emplace(app.getConfig(), app.getRandomEngine(),
                     app.getHighScores().front().first);
        game->initializeSprites(app.getRandomEngine());
        play_window->setWindowPos(game->getFredCellPos());
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
        app.getDisplayConfig().setGameViewport();
        play_window->renderFrame(*game, app.getRenderer(), tmgr);
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
            play_window->renderFrame(*game, app.getRenderer(), app.getTextureManager());
            if (app.getConfig().virtual_controller)
                Controller::render(app.getWindow(), app.getRenderer(), app.getTextureManager());
            SDL_RenderPresent(app.getRenderer());
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
        {
            if (fred->dbgMoveToHatch())
            {
                play_window->setWindowPos(game->getFredCellPos());
                play_window->resetUserOffset();
            }
        }
        else if (event_mask.check(GameEvent::DBG_DIE))
            fred->dbgDie();
    }

    void updateGame(FredApp &app, AppState &app_state, EventMask event_mask)
    {
        play_window->update(event_mask);
        game->update(event_mask);
        if (app.getConfig().debug_keys)
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

    void eventHandler(FredApp &app,
                      AppState &app_state,
                      EventMask const &event_mask) final
    {
        if (event_mask.check(GameEvent::BACK))
            app_state.set(AppState::MAIN_MENU, app);
        else if (game->getLevelStatus() == GameBase::LevelStatus::PLAY)
            updateGame(app, app_state, event_mask);
        else if (game->getLevelStatus() == GameBase::LevelStatus::NEXT_LEVEL)
        {
            if (event_mask.check(GameEvent::TIMER))
            {
                game->addScore(5000 + game->getTreasureCount() * 1000);
                game->nextLevel(app.getRandomEngine());
                game->initializeSprites(app.getRandomEngine());
                play_window->setWindowPos(game->getFredCellPos());
            }
        }
        else if (game->getLevelStatus() == GameBase::LevelStatus::GAME_OVER)
        {
            if (event_mask.check(GameEvent::TIMER))
            {
                auto fred = dynamic_cast<Fred *>(game->getSpriteList(SpriteClass::FRED).front().get());
                ++counter;
                if (counter < 6)
                {
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
                {
                    if (game->getScore() > app.getHighScores().back().first)
                    {
                        app_state.new_high_score = game->getScore();
                        app_state.set(AppState::ENTER_HIGH_SCORE, app);
                    }
                    else
                        app_state.set(AppState::TODAYS_GREATEST, app);
                }
            }
        }
    }
};

class StateEnterHighScore : public BaseState
{
    std::string initials = "A";

    void enter(FredApp &, AppState &) final
    {
        initials = "A";
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
        if (app.getConfig().virtual_controller)
            Controller::render(app.getWindow(), app.getRenderer(), tmgr);
        SDL_RenderPresent(app.getRenderer());
    }
    void eventHandler(FredApp &app,
                      AppState &app_state,
                      EventMask const &event_mask) final
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
                app.addHighScore(app_state.new_high_score, initials);
                app_state.set(AppState::TODAYS_GREATEST, app);
            }
            else
                initials += "A";
        }
    }
};

AppState::AppState(Config &cfg)
    : state_table(COUNT)
    , event_manager(cfg.ticks_per_frame, cfg.virtual_controller)
{
    state_table[SPLASH_SCREEN] = std::make_unique<StateSplashScreen>();
    state_table[MAIN_MENU] = std::make_unique<StateMainMenu>();
    state_table[CONFIG_MENU] = std::make_unique<StateConfigMenu>(cfg);
    state_table[TODAYS_GREATEST] = std::make_unique<StateTodaysGreatest>();
    state_table[PLAY] = std::make_unique<StatePlay>();
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
    , w_and_r(initDisplay(cfg))
    , display_cfg(cfg, w_and_r.first, w_and_r.second)
    , tmgr(cfg, getRenderer()), smgr(cfg)
    , high_scores(4, {0, ""})
    , high_scores_path(getPrefPath() + "high_scores.tbl")
{
    loadHighScores();
    SDL_SetWindowTitle(getWindow(), "Retro-Fred");
    SDL_SetWindowIcon(getWindow(), tmgr.getFredIcon());
}

std::pair<sdl::WindowPtr, sdl::RendererPtr> FredApp::initDisplay(Config const &cfg)
{
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    Uint32 window_flags = 0;
    int width = display_mode.w, height = display_mode.h;
    if (cfg.full_screen)
        window_flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
    else if (cfg.max_resolution)
    {
        auto max_w = (cfg.map_width * MapPos::CELL_WIDTH + 20) * MapPos::PIXELS_PER_CHAR;
        auto max_h = (cfg.map_height * MapPos::CELL_HEIGHT + 15) * MapPos::PIXELS_PER_CHAR;
        width = std::min(max_w, display_mode.w * 19 / 20);
        height = std::min(max_h, display_mode.h * 19 / 20);
        window_flags = SDL_WINDOW_RESIZABLE;
    }
    else
    {
        auto scale_w = static_cast<int>(static_cast<double>(display_mode.w * .8 / cfg.logical_width));
        auto scale_h = static_cast<int>(static_cast<double>(display_mode.h * .8 / cfg.logical_height));
        auto scale = std::max(std::min(scale_w, scale_h), 1);
        width = cfg.logical_width * scale;
        height = cfg.logical_height * scale;
        window_flags = SDL_WINDOW_RESIZABLE;
    }
    return sdl::createWindowAndRenderer(width, height, window_flags);
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

void FredApp::mainLoop()
{
    AppState app_state(cfg);
    app_state.set(AppState::SPLASH_SCREEN, *this);
    while (true)
    {
        app_state.render(*this);
        auto event_mask = app_state.event_manager.collectEvents(getWindow());
        if (event_mask.check(GameEvent::QUIT))
            break;
        if (!app_state.eventHandler(*this, event_mask))
            break;
    }
}
