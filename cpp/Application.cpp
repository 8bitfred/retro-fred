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
#include "Controller.hpp"
#include <iostream>
#include <cstdio>
#include <fstream>

FredApp::FredApp(Config const &cfg, std::minstd_rand &random_engine)
    : cfg(cfg)
    , random_engine(random_engine)
    , w_and_r(initDisplay(cfg))
    , display_cfg(cfg, w_and_r.first, w_and_r.second)
    , tmgr(cfg, getRenderer())
    , smgr(cfg)
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

void FredApp::splashScreen(StateSplashScreen const &state_data)
{
    display_cfg.setIntroViewport();
    SDL_RenderClear(getRenderer());
    auto texture_id = static_cast<TextureID>(static_cast<int>(TextureID::LOADING1) + state_data.seq);
    SDL_RenderCopy(getRenderer(), tmgr.get(texture_id), nullptr, nullptr);
    SDL_Rect splash_rect{32, 24, 256 - 64, 192 - 48};
    SDL_RenderCopy(getRenderer(), tmgr.get(TextureID::SPLASH_SCREEN), nullptr, &splash_rect);
    SDL_RenderPresent(getRenderer());
}

void FredApp::menu(StateMenu &state_data)
{
    display_cfg.setIntroViewport();
    SDL_RenderClear(getRenderer());
    SDL_Rect logo = {88, 8, 76, 20};
    SDL_RenderCopy(getRenderer(), tmgr.get(TextureID::FRED_LOGO), nullptr, &logo);
    if ((state_data.counter % 2) == 0)
    // TODO: this is not a very sustainable way to customize messages depending on the
    // device type => this should be moved to some xml/json file with all messages, that
    // can support multiple languages, and can be customized depending on device type
#ifdef __ANDROID__
        tmgr.renderText(getRenderer(), "TAP ON THE SCREEN TO START", 24, 56, 206, 206, 206);
#else
        tmgr.renderText(getRenderer(), "PRESS ANY KEY TO START", 40, 56, 206, 206, 206);
#endif
    tmgr.renderText(getRenderer(), "WRITTEN BY FERNANDO RADA,", 0, 104, 206, 206, 206);
    tmgr.renderText(getRenderer(), "PACO MENENDEZ & CARLOS GRANADOS.", 0, 112, 206, 206, 206);
    tmgr.renderText(getRenderer(), "       \x7f INDESCOMP SPAIN", 0, 120, 206, 206, 206);
    tmgr.renderText(getRenderer(), "PRESENTED BY QUICKSILVA", 0, 128, 206, 206, 206);
    tmgr.renderText(getRenderer(), "PRESENTATION SCREEN DESIGN:", 0, 136, 206, 206, 206);
    tmgr.renderText(getRenderer(), "JUAN DELCAN, KIKI & MA", 0, 144, 206, 206, 206);
    tmgr.renderText(getRenderer(), "CHARACTER DESIGN: GAELIC", 0, 152, 206, 206, 206);
    tmgr.renderText(getRenderer(), "PET: SENATOR & DRULY'S DUCK", 0, 160, 206, 206, 206);
    tmgr.renderText(getRenderer(), "MISTAKES: MARTA & PALOMA", 0, 168, 206, 206, 206);
    tmgr.renderText(getRenderer(), "2023 PORT:  MIGUEL CATALINA &", 0, 176, 206, 206, 206);
    tmgr.renderText(getRenderer(), "            ALFREDO CATALINA", 0, 184, 206, 206, 206);
    SDL_RenderPresent(getRenderer());
}

void FredApp::todaysGreatest()
{
    display_cfg.setIntroViewport();
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
}

void FredApp::renderGame(StatePlay const &state_data)
{
    display_cfg.setGameViewport();
    SDL_RenderClear(getRenderer());
    state_data.game.render(tmgr, getRenderer(), state_data.play_window.getGameWindow());
    state_data.play_window.renderFrame(state_data.game, getRenderer(), tmgr);
    if (cfg.virtual_controller)
        Controller::render(getWindow(), getRenderer(), tmgr);
    SDL_RenderPresent(getRenderer());

}

void FredApp::debugMode(StatePlay &state_data, EventMask event_mask)
{
    auto fred = dynamic_cast<Fred *>(state_data.game.getSpriteList(SpriteClass::FRED).front().get());
    if (event_mask.check(GameEvent::DBG_CENTER_FRED))
    {
        if (fred->dbgResetPosition(state_data.play_window.getCenter()))
        {
            state_data.play_window.setWindowPos(state_data.game.getFredCellPos());
            state_data.play_window.resetUserOffset();
        }
    }
    else if (event_mask.check(GameEvent::DBG_MOVE_TO_EXIT))
    {
        if (fred->dbgMoveToHatch())
        {
            state_data.play_window.setWindowPos(state_data.game.getFredCellPos());
            state_data.play_window.resetUserOffset();
        }
    }
    else if (event_mask.check(GameEvent::DBG_DIE))
        fred->dbgDie();
}

void FredApp::updateGame(StatePlay &state_data, EventManager &event_manager, EventMask event_mask)
{
    if (cfg.debug_keys)
        debugMode(state_data, event_mask);
    state_data.play_window.update(event_mask);
    auto level_status = state_data.game.update(event_mask);
    if (level_status == GameBase::LevelStatus::GAME_OVER)
    {
        smgr.play(SoundID::GAME_OVER);
        event_manager.setTimer(500);
        renderGame(state_data);
        return;
    }
    else if (level_status == GameBase::LevelStatus::NEXT_LEVEL)
    {
        transitionToNextLevel(state_data, event_manager);
        return;
    }
    state_data.play_window.setWindowPos(state_data.game.getFredCellPos());
    renderGame(state_data);
    state_data.game.playPendingSounds(smgr);
}

void FredApp::transitionToNextLevel(StatePlay &state_data, EventManager &event_manager)
{
    display_cfg.setIntroViewport();
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
    std::snprintf(buf, sizeof (buf), "%02d", state_data.game.getTreasureCount());
    tmgr.renderText(getRenderer(), buf, 96, 88, 206, 206, 206);
    tmgr.renderText(getRenderer(), "BONUS FOR TREASURES:", 24, 104,
                    206, 206, 206);
    std::snprintf(buf, sizeof (buf), "%05d", state_data.game.getTreasureCount() * 1000);
    tmgr.renderText(getRenderer(), buf, 96, 120, 206, 206, 206);
    SDL_Rect fred_puffing{88, 128, 32, 40};
    SDL_RenderCopy(getRenderer(), tmgr.get(TextureID::FRED_PUFFING),
                   nullptr, &fred_puffing);
    display_cfg.setGameViewport();
    state_data.play_window.renderFrame(state_data.game, getRenderer(), tmgr);
    SDL_RenderPresent(getRenderer());
    smgr.play(SoundID::EXIT_MAZE);
    event_manager.setTimer(7000);
}

void FredApp::updateGameOverSequence(StatePlay &state_data, EventManager &event_manager)
{
    auto fred = dynamic_cast<Fred *>(state_data.game.getSpriteList(SpriteClass::FRED).front().get());
    ++state_data.counter;
    if (state_data.counter < 6)
    {
        fred->updateFred(EventMask());
        event_manager.setTimer(500);
        renderGame(state_data);
    }
    else {
        auto pos = state_data.game.getFredCellPos();
        pos.xadd(-2);
        state_data.game.getSpriteList(SpriteClass::FRED).pop_back();
        state_data.game.getSpriteList(SpriteClass::TOMB).emplace_back(std::make_unique<Tomb>(pos));
        renderGame(state_data);
        event_manager.setTimer(5000);
        state.emplace<StateGameOver>(state_data.game.getScore());
    }
}

void FredApp::renderHighScoreScreen(std::string const &initials)
{
    display_cfg.setIntroViewport();
    SDL_RenderClear(getRenderer());
    sdl::ColorGuard color_guard(getRenderer(), 0, 206, 0, 255);
    if (SDL_RenderFillRect(getRenderer(), nullptr) < 0)
        throw sdl::Error();
    tmgr.renderText(getRenderer(), "CONGRATULATIONS", 8 * 8, 0, 0, 0, 0);
    tmgr.renderText(getRenderer(), "YOU HAVE ONE OF TODAY'S GREATEST",
                    0, 8, 0, 0, 0);
    tmgr.renderText(getRenderer(), "ENTER YOUR INITIALS WITH LEFT,",
                    0, 16, 0, 0, 0);
    tmgr.renderText(getRenderer(), "RIGHT & FIRE",
                    0, 24, 0, 0, 0);
    tmgr.renderText(getRenderer(), initials, 14 * 8, 96, 0, 0, 0);
    if (cfg.virtual_controller)
        Controller::render(getWindow(), getRenderer(), tmgr);
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
            saveHighScores();
            todaysGreatest();
            smgr.play(SoundID::FUNERAL_MARCH);
            event_manager.setTimer(8000);
            state.emplace<StateTodaysGreatest>();
            return;
        }
        else
            initials += "A";
    }
    renderHighScoreScreen(initials);
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

void FredApp::mainLoop()
{
    EventManager event_manager(cfg.ticks_per_frame, cfg.virtual_controller);

    {
        auto &splash_data = state.emplace<StateSplashScreen>();
        splashScreen(splash_data);
        auto const &wav_data = smgr.get(SoundID::LOADING1);
        splash_data.sound_timer = SDL_GetTicks() + wav_data.getLenTicks();
        smgr.play(SoundID::LOADING1);
    }
    event_manager.setTimer(StateSplashScreen::LOADING_FRAME_TICKS);
    while (true)
    {
        auto event_mask = event_manager.collectEvents(getWindow());
        if (event_mask.check(GameEvent::QUIT))
            break;
        if (auto splash_data = std::get_if<StateSplashScreen>(&state); splash_data)
        {
            if (event_mask.check(GameEvent::ANY_KEY) ||
                (event_mask.check(GameEvent::TIMER) && splash_data->counter <= 0))
            {
                smgr.clearQueuedAudio();
                auto &menu_data = state.emplace<StateMenu>();
                menu(menu_data);
                event_manager.setTimer(500);
            }
            else if (event_mask.check(GameEvent::TIMER))
            {
                --splash_data->counter;
                std::uniform_int_distribution<> distrib(0, 8 * 4 - 1);
                if (auto pick = distrib(random_engine); pick > 6*4)
                    splash_data->seq = pick % 4;
                else
                    splash_data->seq = (splash_data->seq + 1) % 4;
                splashScreen(*splash_data);
                if ((splash_data->sound_timer - SDL_GetTicks()) < 100)
                {
                    auto sound_id = static_cast<SoundID>(static_cast<int>(SoundID::LOADING1) + splash_data->seq);
                    auto const &wav_data = smgr.get(sound_id);
                    splash_data->sound_timer += wav_data.getLenTicks();
                    smgr.play(sound_id);
                }
                event_manager.setTimer(StateSplashScreen::LOADING_FRAME_TICKS);
            }
            else if (event_mask.check(GameEvent::BACK))
                break;
            else
                splashScreen(*splash_data);
        }
        else if (auto menu_state = std::get_if<StateMenu>(&state); menu_state)
        {
            if (event_mask.check(GameEvent::BACK))
                break;
            else if (event_mask.check(GameEvent::ANY_KEY))
            {
                auto &play_data = state.emplace<StatePlay>(cfg, display_cfg,
                                                           random_engine,
                                                           high_scores.front().first);
                play_data.game.initializeSprites(random_engine);
                play_data.play_window.setWindowPos(play_data.game.getFredCellPos());
                renderGame(play_data);
            }
            else if (event_mask.check(GameEvent::TIMER))
            {
                ++menu_state->counter;
                if (menu_state->counter == 10)
                {
                    todaysGreatest();
                    smgr.play(SoundID::FUNERAL_MARCH);
                    event_manager.setTimer(8000);
                    state.emplace<StateTodaysGreatest>();
                }
                else
                {
                    menu(*menu_state);
                    event_manager.setTimer(500);
                }
            }
            else
                menu(*menu_state);
        }
        else if (auto todays_greatest = std::get_if<StateTodaysGreatest>(&state); todays_greatest)
        {
            if (event_mask.check(GameEvent::TIMER))
            {
                auto &menu_data = state.emplace<StateMenu>();
                menu(menu_data);
                event_manager.setTimer(500);
            }
            else if (event_mask.check(GameEvent::BACK))
                break;
            else
                todaysGreatest();
        }
        else if (auto play_state = std::get_if<StatePlay>(&state); play_state)
        {
            if (event_mask.check(GameEvent::BACK))
            {
                auto &menu_data = state.emplace<StateMenu>();
                menu(menu_data);
                event_manager.setTimer(500);
            }
            else if (play_state->game.getLevelStatus() == GameBase::LevelStatus::PLAY)
                updateGame(*play_state, event_manager, event_mask);
            else if (play_state->game.getLevelStatus() == GameBase::LevelStatus::NEXT_LEVEL)
            {
                if (event_mask.check(GameEvent::TIMER))
                {
                    play_state->game.addScore(5000 + play_state->game.getTreasureCount() * 1000);
                    play_state->game.nextLevel(random_engine);
                    play_state->game.initializeSprites(random_engine);
                    play_state->play_window.setWindowPos(play_state->game.getFredCellPos());
                    renderGame(*play_state);
                }
            }
            else if (play_state->game.getLevelStatus() == GameBase::LevelStatus::GAME_OVER)
            {
                if (event_mask.check(GameEvent::TIMER))
                    updateGameOverSequence(*play_state, event_manager);
            }
        }
        else if (auto game_over = std::get_if<StateGameOver>(&state); game_over)
        {
            if (event_mask.check(GameEvent::TIMER))
            {
                if (game_over->score > high_scores.back().first)
                {
                    auto &new_state = state.emplace<StateEnterHighScore>(game_over->score);
                    renderHighScoreScreen(new_state.initials);
                }
                else
                {
                    todaysGreatest();
                    smgr.play(SoundID::FUNERAL_MARCH);
                    event_manager.setTimer(8000);
                    state.emplace<StateTodaysGreatest>();
                }
            }
        }
        else if (auto enter_high_score = std::get_if<StateEnterHighScore>(&state); enter_high_score)
        {
            updateHighScore(enter_high_score->initials, enter_high_score->score, event_manager, event_mask);
        }
    }
}
