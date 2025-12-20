#include "frontend.hpp"
#include "visualise.hpp"
#include "logic.hpp"
#include "levels.hpp"
#include "menu.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <random>
#include <chrono>

using namespace std;

struct FadeTransition
{
    bool active = false;
    bool fadingOut = true;  // true: 0->255, false: 255->0
    float duration = 0.35f; // thời gian 1 pha
    float t = 0.f;

    sf::RectangleShape overlay;

    void init(const sf::Vector2u &winSize)
    {
        overlay.setSize(sf::Vector2f((float)winSize.x, (float)winSize.y));
        overlay.setPosition(0.f, 0.f);
        overlay.setFillColor(sf::Color(0, 0, 0, 0));
    }

    void startFadeOut(float dur = 0.35f)
    {
        active = true;
        fadingOut = true;
        duration = dur;
        t = 0.f;
        overlay.setFillColor(sf::Color(0, 0, 0, 0));
    }

    void startFadeIn(float dur = 0.35f)
    {
        active = true;
        fadingOut = false;
        duration = dur;
        t = 0.f;
        overlay.setFillColor(sf::Color(0, 0, 0, 255));
    }

    // trả về true nếu pha hiện tại đã xong
    bool update(float dt)
    {
        if (!active)
            return false;

        t += dt;
        float p = t / duration;
        if (p > 1.f)
            p = 1.f;

        if (fadingOut)
        {
            int a = (int)(255.f * p);
            overlay.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)a));
        }
        else
        {
            int a = (int)(255.f * (1.f - p));
            overlay.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)a));
        }

        if (p >= 1.f)
        {
            if (fadingOut)
            {
                // Giữ màn hình đen để chuẩn bị switch state
                overlay.setFillColor(sf::Color(0, 0, 0, 255));
                // active giữ nguyên = true
                return true; // báo "fade out done"
            }
            else
            {
                // fade in done -> tắt overlay
                overlay.setFillColor(sf::Color(0, 0, 0, 0));
                active = false;
                return true;
            }
        }
        return false;
    }

    void draw(sf::RenderWindow &window)
    {
        if (active)
            window.draw(overlay);
    }

    // tiện: kiểm tra đã đen hoàn toàn chưa (alpha=255)
    bool isFullBlack() const
    {
        return overlay.getFillColor().a == 255;
    }
};

int main()
{
    vector<string> maze_6_1 = {
        "......",
        "......",
        ".....T",
        ".....P",
        ".....M",
        "......",
    };
    vector<vector<int>> wall_6_1 = {
        {5, 4, 12, 12, 4, 6},
        {11, 11, 5, 4, 8, 2},
        {5, 14, 1, 0, 4, 2},
        {1, 12, 0, 0, 8, 2},
        {1, 6, 1, 2, 5, 2},
        {11, 9, 8, 8, 8, 10}};

    vector<string> maze_6_2 = {
        "......",
        "......",
        "......",
        "M....P",
        "......",
        "......",
    };
    vector<vector<int>> wall_6_2 = {
        {7, 13, 4, 12, 4, 6},
        {1, 14, 1, 14, 1, 2},
        {1, 6, 1, 12, 2, 3},
        {1, 8, 8, 6, 9, 2},
        {1, 4, 14, 1, 14, 3},
        {9, 8, 14, 9, 12, 10}};

    vector<string> maze_8_1 = {
        "..S.....",
        "........",
        "........",
        "........",
        ".......M",
        "........",
        "........",
        "......P.",
    };
    vector<vector<int>> wall_8_1 = {
        {5, 4, 4, 4, 4, 4, 4, 14},
        {9, 8, 0, 8, 2, 1, 0, 6},
        {7, 13, 0, 4, 0, 2, 1, 2},
        {3, 7, 1, 2, 1, 0, 8, 2},
        {9, 2, 1, 2, 1, 0, 4, 2},
        {13, 0, 0, 2, 9, 0, 2, 11},
        {5, 0, 10, 9, 6, 3, 11, 7},
        {11, 9, 12, 12, 8, 8, 12, 10}};

    vector<string> maze_10_1 = {
        ".T........",
        "...M......",
        "..........",
        "..........",
        ".....M....",
        "..........",
        "..........",
        "..........",
        "....P.....",
        ".........."};

    vector<vector<int>> wall_10_1 = {
        {13, 4, 4, 4, 4, 4, 4, 12, 12, 6},
        {5, 10, 3, 1, 0, 8, 2, 13, 12, 10},
        {3, 5, 8, 0, 2, 7, 1, 6, 5, 14},
        {1, 8, 6, 9, 2, 3, 1, 2, 3, 7},
        {1, 6, 1, 6, 3, 9, 8, 0, 0, 2},
        {3, 1, 2, 1, 0, 6, 5, 8, 8, 10},
        {3, 1, 0, 2, 11, 1, 8, 4, 4, 14},
        {1, 0, 8, 0, 6, 11, 7, 1, 8, 14},
        {1, 0, 4, 10, 1, 4, 2, 9, 4, 6},
        {11, 9, 8, 14, 9, 10, 9, 12, 8, 10}};

    load_anh();
    load_hud_textures();
    reset_progress();

    sf::RenderWindow window(
        sf::VideoMode(1920, 1080),
        "Maze of Malice",
        sf::Style::Titlebar | sf::Style::Close);
    gameplay_fade_init(window);
    enum GameState
    {
        MENU,
        PLAYING,
        HOW_TO_PLAY,
        GAME_OVER,
        GAME_WIN,
        NEXT_LEVEL
    };
    GameState state = MENU;
    GameState pre_state = MENU;

    start_menu_music();

    // ===== Fade setup =====
    FadeTransition fade;
    fade.init(window.getSize());
    sf::Clock dtClock;

    // pending state change (dùng cho các state menu-style)
    bool pending = false;
    GameState pendingState = MENU;

    auto requestState = [&](GameState target)
    {
        if (fade.active)
            return;
        pending = true;
        pendingState = target;
        fade.startFadeOut(0.7f);
    };

    int cur_level = 0;

    while (window.isOpen())
    {
        float dt = dtClock.restart().asSeconds();

        // update fade trước
        bool fadeDone = fade.update(dt);

        // nếu đang pending và fadeOut đã xong -> đổi state ngay
        if (pending && fade.fadingOut && fadeDone)
        {
            state = pendingState;
            pending = false;

            if (state != PLAYING)
            {
                fade.startFadeIn(0.70f);
            }
            else
            {
                fade.active = false;
            }
        }

        if (state != pre_state)
        {
            if (state == MENU)
                start_menu_music();
            if (pre_state == MENU && state == PLAYING)
            {
                stop_menu_music();
                start_game_music();
            }
            if (pre_state == PLAYING && state != PLAYING)
            {
                stop_game_music();
                gameplay_fade_start_in(0.6f);
            }
            pre_state = state;
        }

        window.clear();

        if (state == MENU)
        {
            draw_main_menu(window);

            if (!fade.active)
            {
                int button = handle_main_menu_input(window);
                if (button == 1)
                {
                    cur_level = 0;
                    requestState(PLAYING);
                }
                else if (button == 2)
                {
                    cur_level = load_progress();
                    requestState(PLAYING);
                }
                else if (button == 3)
                {
                    requestState(HOW_TO_PLAY);
                }
            }
        }
        else if (state == HOW_TO_PLAY)
        {
            draw_how_to_play(window);
            if (!fade.active && handle_how_to_play_input(window))
                requestState(MENU);
        }
        else if (state == GAME_OVER)
        {
            draw_game_over(window);
            if (!fade.active && handle_game_over_input(window))
                requestState(MENU);
        }
        else if (state == GAME_WIN)
        {
            draw_game_win(window);
            if (!fade.active && handle_game_win_input(window))
                requestState(MENU);
        }
        else if (state == NEXT_LEVEL)
        {
            draw_next_level_screen(window);
            if (!fade.active && handle_next_level_input(window))
                requestState(PLAYING);
        }
        else if (state == PLAYING)
        {
            int result;
            if (cur_level == 0)
                result = level_0(window);
            else if (cur_level == 1)
                result = run_level(maze_6_1, wall_6_1, window, 6, 6, 5, 3);
            else if (cur_level == 2)
                result = run_level(maze_6_2, wall_6_2, window, 6, 6, 0, 0);
            else if (cur_level == 3)
                result = run_level(maze_8_1, wall_8_1, window, 8, 8, 0, 0);
            else if (cur_level == 4)
                result = run_level(maze_10_1, wall_10_1, window, 10, 10, 0, 8);
            else
                result = 1;

            if (result == -1)
            {
                state = MENU;
                continue;
            }
            else if (result == 1)
            {
                save_progress(cur_level + 1);
                if (cur_level == 4)
                    state = GAME_WIN;
                else
                {
                    cur_level++;
                    state = NEXT_LEVEL;
                }
            }
            else if (result == 0)
            {
                state = GAME_OVER;
            }
        }
        fade.draw(window);

        window.display();
    }

    return 0;
}