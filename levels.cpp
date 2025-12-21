#include "levels.hpp"
#include "visualise.hpp"
#include "logic.hpp"
#include "frontend.hpp"
#include "menu.hpp"
#include <fstream>
#include <SFML/Graphics.hpp>

using namespace std;

vector<int> tile_type;

int load_progress()
{
    fstream fin("progress.txt", ios::in);
    int level = 0;
    fin >> level;
    return level;
}

void reset_progress()
{
    ofstream fout("progress.txt");
    int level = 0;
    fout << level;
}

void save_progress(int level)
{
    ofstream fout("progress.txt");
    fout << level;
}

int level_0(sf::RenderWindow &window)
{
    // ref: https://postimg.cc/w3fkDvww
    vector<string> maze = {
        "......",
        "....P.",
        "......",
        "......",
        "......",
        ".M....",
    };

    vector<vector<int>> wall = {
        {13, 4, 14, 5, 12, 6},
        {5, 8, 14, 11, 5, 10},
        {3, 5, 14, 5, 2, 7},
        {9, 0, 4, 2, 11, 3},
        {5, 8, 0, 0, 6, 3},
        {9, 12, 8, 8, 8, 10}};

    int n_row = maze.size();
    int n_col = maze[0].size();
    int player_r = -1, player_c = -1;
    int goblin_r = -1, goblin_c = -1;
    int win_r = 5, win_c = 0;

    tile_type.clear();
    for (int row = 0; row < n_row; ++row)
        for (int col = 0; col < n_col; ++col)
            tile_type.push_back(Rand(1, 100));
    // scan map
    for (int r = 0; r < n_row; r++)
    {
        for (int c = 0; c < n_col; c++)
        {
            if (maze[r][c] == 'P')
            {
                player_r = r;
                player_c = c;
            }
            else if (maze[r][c] == 'M')
            {
                goblin_r = r;
                goblin_c = c;
            }
        }
    }

    player_anim.start_r = player_r;
    player_anim.start_c = player_c;
    player_anim.target_r = player_r;
    player_anim.target_c = player_c;
    player_anim.dir = 1;

    goblin_anim.start_r = goblin_r;
    goblin_anim.start_c = goblin_c;
    goblin_anim.target_r = goblin_r;
    goblin_anim.target_c = goblin_c;
    goblin_anim.dir = 1;

    reset_animation_states();

    bool caught = false, win = false;
    bool exist = (goblin_r != -1);

    vector<pair<int, int>> player, goblin;
    player.push_back({player_r, player_c});
    goblin.push_back({goblin_r, goblin_c});

    int goblin2_r = -1, goblin2_c = -1;
    int slime_r = -1, slime_c = -1;
    int trap_r = -1, trap_c = -1;
    visualise_game(window, wall, player_r, player_c, goblin_r, goblin_c, goblin2_r, goblin2_c,
                   slime_r, slime_c, trap_r, trap_c, win_r, win_c,
                   player.size() - 1, (goblin_r != -1) + (goblin2_r != -1), (slime_r != -1));

    while (window.isOpen())
    {
        char input = '0';

        // SFML event & keyboard handling
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                return -1;
            }
            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                case sf::Keyboard::Up:
                    input = 'w';
                    break;
                case sf::Keyboard::Left:
                    input = 'a';
                    break;
                case sf::Keyboard::Down:
                    input = 's';
                    break;
                case sf::Keyboard::Right:
                    input = 'd';
                    break;
                case sf::Keyboard::Space:
                    input = ' ';
                    break;
                case sf::Keyboard::R:
                    input = 'r';
                    break;
                case sf::Keyboard::M:
                    input = 'm';
                    break;
                case sf::Keyboard::Q:
                    input = 'q';
                    break;
                }
            }
            else
            {
                if (event.type == sf::Event::MouseButtonPressed)
                {

                    float mx = event.mouseButton.x;
                    float my = event.mouseButton.y;

                    if (HUD_HOME_BOUNDS.contains(mx, my))
                        return -1;
                    else if (HUD_UNDO_BOUNDS.contains(mx, my))
                        input = 'r';
                    else if (HUD_RESET_BOUNDS.contains(mx, my))
                        input = 'm';
                }
            }
            if (input == '0')
                continue; // no key pressed, skip loop

            // Quit
            if (input == 'q')
            {
                window.close();
                return -1;
            }

            // redo
            if (input == 'r')
            {
                if (player.size() == 1)
                    continue;
                // go back
                player.pop_back();
                goblin.pop_back();
                player_r = player.back().first;
                player_c = player.back().second;
                goblin_r = goblin.back().first;
                goblin_c = goblin.back().second;

                player_anim.target_r = player_r;
                player_anim.target_c = player_c;
                goblin_anim.target_r = goblin_r;
                goblin_anim.target_c = goblin_c;
                reset_animation_states();
                continue;
            }

            // reset
            if (input == 'm')
            {
                // return to first stored location
                player_r = player.front().first;
                player_c = player.front().second;
                goblin_r = goblin.front().first;
                goblin_c = goblin.front().second;
                player.clear();
                goblin.clear();

                // re-create history
                player.push_back({player_r, player_c});
                goblin.push_back({goblin_r, goblin_c});
                player_anim.target_r = player_r;
                player_anim.target_c = player_c;
                goblin_anim.target_r = goblin_r;
                goblin_anim.target_c = goblin_c;
                reset_animation_states();
                continue;
            }

            // Player movement
            int nxt_r = player_r, nxt_c = player_c;
            player_move(wall, input, player_r, player_c, nxt_r, nxt_c);
            caught = false;

            if (nxt_r != -1 && nxt_c != -1)
            {
                // collide with goblin
                if (nxt_r == goblin_r && nxt_c == goblin_c)
                {
                    caught = true;
                    continue;
                }

                // reach exit
                if (nxt_r == win_r && nxt_c == win_c)
                {
                    win = true;
                }

                player_r = nxt_r;
                player_c = nxt_c;
                player.push_back({player_r, player_c});

                // goblin move (2 steps random)
                int dr[4] = {0, 0, -1, 1};
                int dc[4] = {-1, 1, 0, 0};
                for (int step = 0; step < 2; step++)
                {
                    vector<int> valid;
                    for (int d = 0; d < 4; d++)
                    {
                        if (!(wall[goblin_r][goblin_c] & (1 << d)))
                            valid.push_back(d);
                    }
                    if (valid.empty())
                        break;
                    int d = valid[Rand(0, valid.size() - 1)];
                    int old_r = goblin_r, old_c = goblin_c;

                    int goblin_dir = logicDir_to_visualDir(d);
                    goblin_anim.dir = goblin_dir;
                    goblin_r += dr[d];
                    goblin_c += dc[d];
                    start_entity_animation(goblin_anim, old_r, old_c, goblin_r, goblin_c, goblin_dir);
                    goblin_anim.moving = true;

                    if (goblin_r == player_r && goblin_c == player_c)
                    {
                        caught = true;
                        break;
                    }

                    sf::Clock wait_clock;
                    while (goblin_anim.is_animating)
                    {
                        sf::Event event;
                        while (window.pollEvent(event))
                        {
                            if (event.type == sf::Event::Closed)
                            {
                                window.close();
                                return -1;
                            }
                        }
                        float delta_time = wait_clock.restart().asSeconds();
                        update_entity_animation(goblin_anim, delta_time);
                        visualise_game(window, wall, player_r, player_c, goblin_r, goblin_c, goblin2_r, goblin2_c,
                                       slime_r, slime_c, trap_r, trap_c, win_r, win_c,
                                       player.size() - 1, (goblin_r != -1) + (goblin2_r != -1), (slime_r != -1));
                    }
                    goblin_anim.moving = false;
                }
                goblin.push_back({goblin_r, goblin_c});
            }
        }
        visualise_game(window, wall, player_r, player_c, goblin_r, goblin_c, goblin2_r, goblin2_c,
                       slime_r, slime_c, trap_r, trap_c, win_r, win_c,
                       player.size() - 1, (goblin_r != -1) + (goblin2_r != -1), (slime_r != -1));

        if (caught)
        {
            return 0;
        }
        if (win)
        {
            return 1;
        }
    }
    return 1;
}

int run_level(vector<string> &maze, vector<vector<int>> &wall, sf::RenderWindow &window, int n_row, int n_col, int win_r, int win_c)
{
    int player_r = -1, player_c = -1;
    int goblin_r = -1, goblin_c = -1;
    int goblin2_r = -1, goblin2_c = -1;
    int trap_r = -1, trap_c = -1;
    int slime_r = -1, slime_c = -1;

    tile_type.clear();
    for (int row = 0; row < n_row; ++row)
        for (int col = 0; col < n_col; ++col)
            tile_type.push_back(Rand(1, 100));

    // Scan maze for initial positions
    for (int row = 0; row < n_row; ++row)
    {
        for (int col = 0; col < n_col; ++col)
        {
            if (maze[row][col] == 'M')
            {
                if (goblin_r == -1)
                    goblin_r = row, goblin_c = col;
                else
                    goblin2_r = row, goblin2_c = col;
            }
            else if (maze[row][col] == 'P')
            {
                player_r = row, player_c = col;
            }
            else if (maze[row][col] == 'T')
            {
                trap_r = row, trap_c = col;
            }
            else if (maze[row][col] == 'S')
            {
                slime_r = row, slime_c = col;
            }
        }
    }

    bool caught = false, win = false, trap = false;
    vector<pair<int, int>> player, goblin, goblin2, slime;
    player.push_back({player_r, player_c});
    goblin.push_back({goblin_r, goblin_c});
    goblin2.push_back({goblin2_r, goblin2_c});
    slime.push_back({slime_r, slime_c});

    player_anim.start_r = player_r;
    player_anim.start_c = player_c;
    player_anim.target_r = player_r;
    player_anim.target_c = player_c;
    player_anim.dir = 1;

    goblin_anim.start_r = goblin_r;
    goblin_anim.start_c = goblin_c;
    goblin_anim.target_r = goblin_r;
    goblin_anim.target_c = goblin_c;
    goblin_anim.dir = 1;

    goblin2_anim.start_r = goblin2_r;
    goblin2_anim.start_c = goblin2_c;
    goblin2_anim.target_r = goblin2_r;
    goblin2_anim.target_c = goblin2_c;
    goblin2_anim.dir = 1;

    slime_anim.start_r = slime_r;
    slime_anim.start_c = slime_c;
    slime_anim.target_r = slime_r;
    slime_anim.target_c = slime_c;
    slime_anim.dir = 1;

    reset_animation_states();
    visualise_game(window, wall, player_r, player_c, goblin_r, goblin_c, goblin2_r, goblin2_c,
                   slime_r, slime_c, trap_r, trap_c, win_r, win_c,
                   player.size() - 1, (goblin_r != -1) + (goblin2_r != -1), (slime_r != -1));

    while (window.isOpen())
    {
        char input = '0';

        // SFML keyboard input
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                return -1;
            }

            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                case sf::Keyboard::Up:
                    input = 'w';
                    break;
                case sf::Keyboard::Left:
                    input = 'a';
                    break;
                case sf::Keyboard::Down:
                    input = 's';
                    break;
                case sf::Keyboard::Right:
                    input = 'd';
                    break;
                case sf::Keyboard::Space:
                    input = ' ';
                    break;
                case sf::Keyboard::R:
                    input = 'r';
                    break;
                case sf::Keyboard::M:
                    input = 'm';
                    break;
                case sf::Keyboard::Q:
                    input = 'q';
                    break;
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {

                float mx = event.mouseButton.x;
                float my = event.mouseButton.y;

                if (HUD_HOME_BOUNDS.contains(mx, my))
                    return -1;
                else if (HUD_UNDO_BOUNDS.contains(mx, my))
                    input = 'r';
                else if (HUD_RESET_BOUNDS.contains(mx, my))
                    input = 'm';
            }

            if (input == '0')
                continue; // no key pressed, skip loop

            // Quit
            if (input == 'q')
            {
                window.close();
                return -1;
            }

            // Redo
            if (input == 'r')
            {
                if (player.size() == 1)
                    continue;
                player.pop_back();
                goblin.pop_back();
                goblin2.pop_back();
                slime.pop_back();
                player_r = player.back().first, player_c = player.back().second;
                goblin_r = goblin.back().first, goblin_c = goblin.back().second;
                goblin2_r = goblin2.back().first, goblin2_c = goblin2.back().second;
                slime_r = slime.back().first, slime_c = slime.back().second;

                player_anim.target_r = player_r;
                player_anim.target_c = player_c;
                goblin_anim.target_r = goblin_r;
                goblin_anim.target_c = goblin_c;
                goblin2_anim.target_r = goblin2_r;
                goblin2_anim.target_c = goblin2_c;
                slime_anim.target_r = slime_r;
                slime_anim.target_c = slime_c;
                reset_animation_states();
                continue;
            }

            // Reset
            if (input == 'm')
            {
                player_r = player.front().first, player_c = player.front().second;
                goblin_r = goblin.front().first, goblin_c = goblin.front().second;
                goblin2_r = goblin2.front().first, goblin2_c = goblin2.front().second;
                slime_r = slime.front().first, slime_c = slime.front().second;

                player.clear(), goblin.clear(), goblin2.clear(), slime.clear();
                player.push_back({player_r, player_c});
                goblin.push_back({goblin_r, goblin_c});
                goblin2.push_back({goblin2_r, goblin2_c});
                slime.push_back({slime_r, slime_c});

                player_anim.target_r = player_r;
                player_anim.target_c = player_c;
                goblin_anim.target_r = goblin_r;
                goblin_anim.target_c = goblin_c;
                goblin2_anim.target_r = goblin2_r;
                goblin2_anim.target_c = goblin2_c;
                slime_anim.target_r = slime_r;
                slime_anim.target_c = slime_c;
                reset_animation_states();
                continue;
            }

            // Player move
            int nxt_player_r = player_r, nxt_player_c = player_c;
            player_move(wall, input, player_r, player_c, nxt_player_r, nxt_player_c);
            caught = false;

            if (nxt_player_r != -1 && nxt_player_c != -1)
            {
                if (nxt_player_r == trap_r && nxt_player_c == trap_c)
                {
                    trap = true;
                    continue;
                }
                if ((nxt_player_r == goblin_r && nxt_player_c == goblin_c) ||
                    (nxt_player_r == goblin2_r && nxt_player_c == goblin2_c) ||
                    (nxt_player_r == slime_r && nxt_player_c == slime_c))
                {
                    caught = true;
                    continue;
                }
                if (nxt_player_r == win_r && nxt_player_c == win_c)
                {
                    win = true;
                }

                player_r = nxt_player_r, player_c = nxt_player_c;
                player.push_back({player_r, player_c});

                if (goblin_r != -1)
                {
                    white_goblin_move(wall, goblin_r, goblin_c, caught, player_r, player_c,
                                      goblin2_r, goblin2_c, slime_r, slime_c, trap_r, trap_c, window, win_r, win_c, player.size() - 1, goblin_anim, 1);
                }
                goblin.push_back({goblin_r, goblin_c});

                if (goblin2_r != -1)
                {
                    white_goblin_move(wall, goblin2_r, goblin2_c, caught, player_r, player_c,
                                      goblin_r, goblin_c, slime_r, slime_c, trap_r, trap_c, window, win_r, win_c, player.size() - 1, goblin2_anim, 2);
                }
                goblin2.push_back({goblin2_r, goblin2_c});

                if (slime_r != -1)
                {
                    white_slime_move(wall, slime_r, slime_c, caught, player_r, player_c,
                                     goblin_r, goblin_c, goblin2_r, goblin2_c, trap_r, trap_c, window, win_r, win_c, player.size() - 1);
                }
                slime.push_back({slime_r, slime_c});
            }
        }
        visualise_game(window, wall, player_r, player_c, goblin_r, goblin_c, goblin2_r, goblin2_c,
                       slime_r, slime_c, trap_r, trap_c, win_r, win_c,
                       player.size() - 1, (goblin_r != -1) + (goblin2_r != -1), (slime_r != -1));
        if (trap)
        {
            return 0;
        }
        if (caught)
        {
            return 0;
        }
        if (win)
        {
            return 1;
        }
    }
    return 1;
}