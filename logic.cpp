#include "logic.hpp"
#include "visualise.hpp"
#include "frontend.hpp"
#include "levels.hpp"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <random>
#include <chrono>

using namespace std;

mt19937 rd(chrono::steady_clock::now().time_since_epoch().count());

int Rand(int l, int h)
{
    return uniform_int_distribution<int>(l, h)(rd);
}

// NOTE: dùng const ref cho an toàn (không cần truyền & để sửa)
int manhattan_dist(const int &player_r, const int &player_c, const int &mob_r, const int &mob_c)
{
    return abs(mob_r - player_r) + abs(mob_c - player_c);
}

// Map hướng trong logic (0..3) -> hướng visual (0..3)
// logic_dir: 0=LEFT, 1=RIGHT, 2=UP, 3=DOWN   (theo dr/dc em đang dùng)
// visual_dir: 0=UP, 1=DOWN, 2=LEFT, 3=RIGHT  (chuẩn anh thống nhất)
int logicDir_to_visualDir(int logic_dir)
{
    if (logic_dir == 0)
        return 2; // LEFT
    if (logic_dir == 1)
        return 3; // RIGHT
    if (logic_dir == 2)
        return 0; // UP
    return 1;     // DOWN
}

void player_move(const vector<vector<int>> &wall, char &input,
                 int &player_r, int &player_c,
                 int &nxt_player_r, int &nxt_player_c)
{

    if (input == 'w')
    {
        if (!(wall[player_r][player_c] & 4))
            nxt_player_r -= 1, player_anim.dir = 0;
        else
            nxt_player_r = -1;
    }
    else if (input == 's')
    {
        if (!(wall[player_r][player_c] & 8))
            nxt_player_r += 1, player_anim.dir = 1;
        else
            nxt_player_r = -1;
    }
    else if (input == 'a')
    {
        if (!(wall[player_r][player_c] & 1))
            nxt_player_c -= 1, player_anim.dir = 2;
        else
            nxt_player_c = -1;
    }
    else if (input == 'd')
    {
        if (!(wall[player_r][player_c] & 2))
            nxt_player_c += 1, player_anim.dir = 3;
        else
            nxt_player_c = -1;
    }
}

// Goblin đi 2 bước / turn
void white_goblin_move(vector<string> &maze,
                       const vector<vector<int>> &wall,
                       int &goblin_r, int &goblin_c,
                       bool &caught,
                       int &player_r, int &player_c,
                       int &goblin2_r, int &goblin2_c,
                       int &slime_r, int &slime_c,
                       int &trap_r, int &trap_c,
                       sf::RenderWindow &window,
                       int win_r, int win_c,
                       int moves, EntityAnimation &anim, int type)
{
    // logic direction: 0=LEFT, 1=RIGHT, 2=UP, 3=DOWN
    const int dr[4] = {0, 0, -1, 1};
    const int dc[4] = {-1, 1, 0, 0};

    // nếu chết rồi thì thôi
    if (goblin_r == -1)
    {
        anim.moving = false;
        return;
    }

    for (int step = 1; step <= 2 && !caught && goblin_r != -1; ++step)
    {
        int best_dir = -1;
        int cur_dist = manhattan_dist(player_r, player_c, goblin_r, goblin_c);

        // tìm hướng làm khoảng cách Manhattan giảm
        for (int dir = 0; dir < 4; ++dir)
        {
            // check tường theo bit: (1<<dir) => 1,2,4,8 đúng với LEFT/RIGHT/UP/DOWN
            if (wall[goblin_r][goblin_c] & (1 << dir))
                continue;

            int nr = goblin_r + dr[dir];
            int nc = goblin_c + dc[dir];

            int nd = manhattan_dist(player_r, player_c, nr, nc);
            if (nd < cur_dist)
            {
                cur_dist = nd;
                best_dir = dir;
            }
        }

        // không tìm được hướng đi => đứng yên
        if (best_dir == -1)
        {
            anim.moving = false;
            continue;
        }

        // set hướng + moving để visual dùng animate
        int old_r = goblin_r, old_c = goblin_c;
        int goblin_dir = logicDir_to_visualDir(best_dir);
        // di chuyển
        goblin_r += dr[best_dir];
        goblin_c += dc[best_dir];
        maze[old_r][old_c] = '.';
        maze[goblin_r][goblin_c] = 'M';
        start_entity_animation(anim, old_r, old_c, goblin_r, goblin_c, goblin_dir);
        anim.moving = true;

        // bắt player
        if (goblin_r == player_r && goblin_c == player_c)
        {
            caught = true;
        }

        // va chạm mob khác
        if (goblin_r == goblin2_r && goblin_c == goblin2_c)
        {
            goblin_r = -1; // goblin chết
            anim.moving = false;
            anim.is_animating = false;
            anim.progress = 1.0f;
        }
        else if (goblin_r == slime_r && goblin_c == slime_c)
        {
            slime_r = -1; // slime chết
            slime_anim.moving = false;
        }

        sf::Clock wait_clock;
        while (anim.is_animating)
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                    return;
                }
            }

            if (type == 1)
            {
                visualise_game(window, wall, player_r, player_c,
                               goblin_r, goblin_c, goblin2_r, goblin2_c,
                               slime_r, slime_c, trap_r, trap_c, win_r, win_c,
                               moves, (goblin_r != -1) + (goblin2_r != -1), (slime_r != -1));
            }
            else
            {
                visualise_game(window, wall, player_r, player_c,
                               goblin2_r, goblin2_c, goblin_r, goblin_c,
                               slime_r, slime_c, trap_r, trap_c, win_r, win_c,
                               moves, (goblin_r != -1) + (goblin2_r != -1), (slime_r != -1));
            }
        }
        anim.moving = false;
        if (caught)
            break;
    }
}

// Slime đi 1 bước / turn
void white_slime_move(vector<string> &maze,
                      const vector<vector<int>> &wall,
                      int &slime_r, int &slime_c,
                      bool &caught,
                      int &player_r, int &player_c,
                      int &goblin_r, int &goblin_c,
                      int &goblin2_r, int &goblin2_c,
                      int &trap_r, int &trap_c,
                      sf::RenderWindow &window,
                      int win_r, int win_c,
                      int moves)
{
    const int dr[4] = {0, 0, -1, 1};
    const int dc[4] = {-1, 1, 0, 0};

    if (slime_r == -1)
    {
        slime_anim.moving = false;
        return;
    }

    slime_anim.moving = false;

    int best_dir = -1;
    int cur_dist = manhattan_dist(player_r, player_c, slime_r, slime_c);

    for (int dir = 0; dir < 4; ++dir)
    {
        if (wall[slime_r][slime_c] & (1 << dir))
            continue;

        int nr = slime_r + dr[dir];
        int nc = slime_c + dc[dir];

        int nd = manhattan_dist(player_r, player_c, nr, nc);
        if (nd < cur_dist)
        {
            cur_dist = nd;
            best_dir = dir;
        }
    }

    // không đi được -> đứng yên
    if (best_dir == -1)
    {
        slime_anim.moving = false;

        visualise_game(window, wall, player_r, player_c,
                       goblin_r, goblin_c, goblin2_r, goblin2_c,
                       slime_r, slime_c, trap_r, trap_c, win_r, win_c,
                       moves, (goblin_r != -1) + (goblin2_r != -1), (slime_r != -1));
        return;
    }
    maze[slime_r][slime_c] = '.';
    maze[slime_r + dr[best_dir]][slime_c + dc[best_dir]] = 'S';
    int old_r = slime_r, old_c = slime_c;
    int slime_dir = logicDir_to_visualDir(best_dir);
    slime_r += dr[best_dir];
    slime_c += dc[best_dir];

    start_entity_animation(slime_anim, old_r, old_c, slime_r, slime_c, slime_dir);
    slime_anim.moving = true;
    if (slime_r == player_r && slime_c == player_c)
    {
        caught = true;
    }

    // va chạm mob khác
    if (slime_r == goblin_r && slime_c == goblin_c)
    {
        slime_r = -1;
        slime_anim.moving = false;
    }
    else if (slime_r == goblin2_r && slime_c == goblin2_c)
    {
        slime_r = -1;
        slime_anim.moving = false;
    }
    sf::Clock wait_clock;
    while (slime_anim.is_animating)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type = sf::Event::Closed)
            {
                window.close();
                return;
            }
        }
        float delta_time = wait_clock.restart().asSeconds();
        update_entity_animation(slime_anim, delta_time);
        visualise_game(window, wall, player_r, player_c,
                       goblin_r, goblin_c, goblin2_r, goblin2_c,
                       slime_r, slime_c, trap_r, trap_c, win_r, win_c,
                       moves, (goblin_r != -1) + (goblin2_r != -1), (slime_r != -1));
    }
    slime_anim.moving = false;
}
