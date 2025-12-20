/*Ref:
1. https://youtu.be/qigPctCgR_k
2. https://theory.stanford.edu/~amitp/GameProgramming/AStarComparison.html#the-a-star-algorithm
*/

#include "frontend.hpp"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <random>
#include <chrono>

using namespace std;

// Dùng sf::sleep thay vì windows.h
#define sf_sleep(ms) sf::sleep(sf::milliseconds(ms))

mt19937 rd(chrono::steady_clock::now().time_since_epoch().count());

int Rand(int l, int h)
{
    return uniform_int_distribution<int>(l, h)(rd);
}

int load_progress()
{
    ifstream fin("progress.txt");
    int level = 0;
    if (fin.is_open())
        fin >> level;
    return level;
}

void save_progress(int &level)
{
    ofstream fout("progress.txt");
    if (fout.is_open())
        fout << level;
}

void game_over(int state)
{
    if (state == 0)
    {
        cout << "Game over!\nYou got caught";
    }
    else
    {
        cout << "Game over!\nYou got trapped";
    }
}

void game_win()
{
    cout << "Congratulations! You have escaped the maze";
}

int manhattan_dist(int &player_r, int &player_c, int &mob_r, int &mob_c)
{
    return abs(mob_r - player_r) + abs(mob_c - player_c);
}

void player_move(const vector<vector<int>> &wall, char &input, int &player_r, int &player_c, int &nxt_player_r, int &nxt_player_c)
{

    // Đảm bảo input luôn là chữ thường để kiểm tra dễ dàng hơn
    if (input >= 'A' && input <= 'Z')
    {
        input = input + ('a' - 'A');
    }

    if (input == 'w')
    { // Up (Tường trên: 4)
        if (!(wall[player_r][player_c] & 4))
            nxt_player_r -= 1;
        else
            nxt_player_r = -1;
    }
    else if (input == 's')
    { // Down (Tường dưới: 8)
        if (!(wall[player_r][player_c] & 8))
            nxt_player_r += 1;
        else
            nxt_player_r = -1;
    }
    else if (input == 'a')
    { // Left (Tường trái: 1)
        if (!(wall[player_r][player_c] & 1))
            nxt_player_c -= 1;
        else
            nxt_player_c = -1;
    }
    else if (input == 'd')
    { // Right (Tường phải: 2)
        if (!(wall[player_r][player_c] & 2))
            nxt_player_c += 1;
        else
            nxt_player_c = -1;
    }
}

void visualise_game(sf::RenderWindow &window, const vector<vector<int>> &wall, int &player_r, int &player_c,
                    int mummy_r, int mummy_c, int mummy2_r, int mummy2_c, int scorpion_r, int scorpion_c,
                    int trap_r, int trap_c, int win_r, int win_c)
{

    // Chỉ vẽ Maze/Entities
    draw_maze(window, wall);
    draw_trap(window, win_r, win_c, sf::Color::Green); // Exit được vẽ như một Trap màu Xanh

    // Vẽ các Mob và Trap
    if (trap_r != -1)
        draw_trap(window, trap_r, trap_c, sf::Color::Magenta);
    if (mummy_r != -1)
        draw_entity(window, mummy_r, mummy_c, sf::Color::Red);
    if (mummy2_r != -1)
        draw_entity(window, mummy2_r, mummy2_c, sf::Color::Red);
    if (scorpion_r != -1)
        draw_entity(window, scorpion_r, scorpion_c, sf::Color::Yellow);

    // Vẽ Player (cuối cùng để ưu tiên)
    draw_entity(window, player_r, player_c, sf::Color::Blue);
}

void white_mummy_move(const vector<vector<int>> &wall, int &mummy_r, int &mummy_c,
                      bool &caught, int &player_r, int &player_c,
                      int &mummy2_r, int &mummy2_c,
                      int &scorpion_r, int &scorpion_c, int &trap_r, int &trap_c,
                      sf::RenderWindow &window, int win_r, int win_c, int moves)
{

    int white_dr[4] = {0, 0, -1, 1}; // left, right, up, down
    int white_dc[4] = {-1, 1, 0, 0};
    int directions[4] = {1, 2, 4, 8}; // bit mask for walls

    for (int step = 1; step <= 2 && !caught && mummy_r != -1; ++step)
    {
        int nx_dir = -1;
        int cur_dist = manhattan_dist(player_r, player_c, mummy_r, mummy_c);

        // Luôn ưu tiên di chuyển theo hướng gần người chơi
        for (int direction = 0; direction < 4; ++direction)
        {
            int nxt_mummy_r = mummy_r + white_dr[direction];
            int nxt_mummy_c = mummy_c + white_dc[direction];

            if (!(wall[mummy_r][mummy_c] & directions[direction]))
            {
                // Đảm bảo không đi ra ngoài biên
                if (nxt_mummy_r >= 0 && nxt_mummy_r < wall.size() && nxt_mummy_c >= 0 && nxt_mummy_c < wall[0].size())
                {
                    int nxt_dist = manhattan_dist(player_r, player_c, nxt_mummy_r, nxt_mummy_c);
                    if (nxt_dist < cur_dist)
                    {
                        cur_dist = nxt_dist;
                        nx_dir = direction;
                    }
                }
            }
        }

        if (nx_dir != -1)
        {
            mummy_r += white_dr[nx_dir];
            mummy_c += white_dc[nx_dir];

            if (mummy_r == player_r && mummy_c == player_c)
            {
                caught = true;
            }

            // Va chạm giữa các Mob (Mob bị xóa nếu va chạm)
            if (mummy2_r != -1 && mummy_r == mummy2_r && mummy_c == mummy2_c)
            {
                mummy_r = -1; // Mummy va chạm bị xóa (Logic game của bạn)
            }
            else if (scorpion_r != -1 && mummy_r == scorpion_r && mummy_c == scorpion_c)
            {
                scorpion_r = -1; // Scorpion bị xóa (Logic game của bạn)
            }

            // Cập nhật lại màn hình và HUD sau mỗi bước di chuyển
            window.clear();
            visualise_game(window, wall, player_r, player_c, mummy_r, mummy_c, mummy2_r, mummy2_c, scorpion_r, scorpion_c, trap_r, trap_c, win_r, win_c);
            draw_hud(window, moves, (mummy_r != -1) + (mummy2_r != -1), (scorpion_r != -1));
            window.display();
            sf_sleep(120); // Dùng sf::sleep
        }
    }
}

void white_scorpion_move(const vector<vector<int>> &wall, int &scorpion_r, int &scorpion_c,
                         bool &caught, int &player_r, int &player_c,
                         int &mummy_r, int &mummy_c,
                         int &mummy2_r, int &mummy2_c, int &trap_r, int &trap_c,
                         sf::RenderWindow &window, int win_r, int win_c, int moves)
{

    int white_dr[4] = {0, 0, -1, 1}; // left, right, up, down
    int white_dc[4] = {-1, 1, 0, 0};
    int directions[4] = {1, 2, 4, 8}; // bit mask for walls

    int nx_dir = -1;
    int cur_dist = manhattan_dist(player_r, player_c, scorpion_r, scorpion_c);

    // Scorpion chi di chuyen 1 buoc
    for (int direction = 0; direction < 4; ++direction)
    {
        int nxt_scorpion_r = scorpion_r + white_dr[direction];
        int nxt_scorpion_c = scorpion_c + white_dc[direction];

        if (!(wall[scorpion_r][scorpion_c] & directions[direction]))
        {
            // Đảm bảo không đi ra ngoài biên
            if (nxt_scorpion_r >= 0 && nxt_scorpion_r < wall.size() && nxt_scorpion_c >= 0 && nxt_scorpion_c < wall[0].size())
            {
                int nxt_dist = manhattan_dist(player_r, player_c, nxt_scorpion_r, nxt_scorpion_c);
                if (nxt_dist < cur_dist)
                {
                    cur_dist = nxt_dist;
                    nx_dir = direction;
                }
            }
        }
    }

    if (nx_dir != -1)
    {
        scorpion_r += white_dr[nx_dir];
        scorpion_c += white_dc[nx_dir];

        if (scorpion_r == player_r && scorpion_c == player_c)
        {
            caught = true;
        }

        // Va chạm giữa các Mob
        if (mummy_r != -1 && scorpion_r == mummy_r && scorpion_c == mummy_c)
        {
            mummy_r = -1;
        }
        else if (mummy2_r != -1 && scorpion_r == mummy2_r && scorpion_c == mummy2_c)
        {
            mummy2_r = -1;
        }

        // Cập nhật lại màn hình và HUD
        window.clear();
        visualise_game(window, wall, player_r, player_c, mummy_r, mummy_c, mummy2_r, mummy2_c, scorpion_r, scorpion_c, trap_r, trap_c, win_r, win_c);
        draw_hud(window, moves, (mummy_r != -1) + (mummy2_r != -1), (scorpion_r != -1));
        window.display();
        sf_sleep(120); // Dùng sf::sleep
    }
}

// Helper function để kiểm tra click trên sprite
bool is_sprite_clicked(const sf::Event &event, sf::Sprite &sprite)
{
    float mx = (float)event.mouseButton.x;
    float my = (float)event.mouseButton.y;
    return sprite.getGlobalBounds().contains(mx, my);
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
    int mummy_r = -1, mummy_c = -1;
    int win_r = 5, win_c = 0; // Win/Exit position

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
                mummy_r = r;
                mummy_c = c;
            }
        }
    }

    bool caught = false, win = false;

    vector<pair<int, int>> player, mummy;
    player.push_back({player_r, player_c});
    mummy.push_back({mummy_r, mummy_c});

    int mummy2_r = -1, mummy2_c = -1;
    int scorpion_r = -1, scorpion_c = -1;
    int trap_r = -1, trap_c = -1;

    // Kiểm soát tốc độ
    sf::Clock clock;
    const sf::Time move_delay = sf::milliseconds(150);
    sf::Time last_move_time = clock.getElapsedTime() - move_delay;

    // Tạo sprite cho nút HUD
    sf::Texture texUndo, texReset, texHome;
    bool texturesLoaded = true;

    // Kích thước mục tiêu cho các nút HUD
    const float HUD_BTN_TARGET_WIDTH = 140.f;
    const float HUD_BTN_TARGET_HEIGHT = 50.f;

    // Load từng texture
    if (!texUndo.loadFromFile("undo_btn.png"))
    {
        cerr << "Failed to load undo_btn.png" << endl;
        texturesLoaded = false;
    }
    if (!texReset.loadFromFile("reset_btn.png"))
    {
        cerr << "Failed to load reset_btn.png" << endl;
        texturesLoaded = false;
    }
    if (!texHome.loadFromFile("home_btn.png"))
    {
        cerr << "Failed to load home_btn.png" << endl;
        texturesLoaded = false;
    }

    sf::Sprite btnUndo, btnReset, btnHome;
    float spriteButtonX = (200 - HUD_BTN_TARGET_WIDTH) / 2.f; // SIDEBAR_WIDTH = 200

    if (texturesLoaded)
    {
        btnUndo.setTexture(texUndo);
        btnReset.setTexture(texReset);
        btnHome.setTexture(texHome);

        // Tính scale cho từng nút để fit vào kích thước mục tiêu
        auto calculateScaleAndPosition = [&](sf::Sprite &sprite, sf::Texture &texture, float targetY)
        {
            // Tính scale để fit vào target size, giữ aspect ratio
            float texWidth = (float)texture.getSize().x;
            float texHeight = (float)texture.getSize().y;
            float scaleX = HUD_BTN_TARGET_WIDTH / texWidth;
            float scaleY = HUD_BTN_TARGET_HEIGHT / texHeight;
            float uniformScale = min(scaleX, scaleY);

            // Tính kích thước thực tế
            float actualWidth = texWidth * uniformScale;
            float actualHeight = texHeight * uniformScale;

            // Đặt scale và vị trí (căn giữa)
            sprite.setScale(uniformScale, uniformScale);
            sprite.setPosition(spriteButtonX + (HUD_BTN_TARGET_WIDTH - actualWidth) / 2.f,
                               targetY + (HUD_BTN_TARGET_HEIGHT - actualHeight) / 2.f);
        };

        calculateScaleAndPosition(btnUndo, texUndo, HUD_UNDO_BUTTON_Y);
        calculateScaleAndPosition(btnReset, texReset, HUD_RESET_BUTTON_Y);
        calculateScaleAndPosition(btnHome, texHome, HUD_MENU_BUTTON_Y);

        cout << "HUD buttons loaded successfully" << endl;
    }
    else
    {
        cerr << "Some HUD textures failed to load, using fallback" << endl;
    }
    // ------------------------------

    while (window.isOpen())
    {

        // Vẽ
        window.clear();
        visualise_game(window, wall, player_r, player_c, mummy_r, mummy_c, -1, -1, -1, -1, -1, -1, win_r, win_c);
        draw_hud(window, player.size() - 1, (mummy_r != -1), 0);
        window.display();

        // Kiểm tra điều kiện
        if (caught)
        {
            return 0;
        }
        if (win)
        {
            return 1;
        }

        char input = '0';

        // Xử lý input
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                return -1; // -1: Cửa sổ đóng
            }

            // Xử lý nút BACK TO MENU, UNDO, RESET
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                float mx = event.mouseButton.x;
                float my = event.mouseButton.y;

                if (texturesLoaded)
                {
                    // Kiểm tra click bằng sprite bounds
                    if (btnHome.getGlobalBounds().contains(mx, my))
                        return -1;
                    else if (btnUndo.getGlobalBounds().contains(mx, my))
                        input = 'r';
                    else if (btnReset.getGlobalBounds().contains(mx, my))
                        input = 'm';
                }
                else
                {
                    // Fallback: dùng phương pháp cũ nếu không tải được texture
                    float buttonX = (200 - HUD_BUTTON_W) / 2.f;
                    if (is_button_clicked(event, buttonX, HUD_MENU_BUTTON_Y, HUD_BUTTON_W, HUD_BUTTON_H))
                        return -1;
                    if (is_button_clicked(event, buttonX, HUD_UNDO_BUTTON_Y, HUD_BUTTON_W, HUD_BUTTON_H))
                        input = 'r';
                    else if (is_button_clicked(event, buttonX, HUD_RESET_BUTTON_Y, HUD_BUTTON_W, HUD_BUTTON_H))
                        input = 'm';
                }
            }

            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                case sf::Keyboard::W:
                case sf::Keyboard::Up:
                    input = 'w';
                    break;

                case sf::Keyboard::A:
                case sf::Keyboard::Left:
                    input = 'a';
                    break;

                case sf::Keyboard::S:
                case sf::Keyboard::Down:
                    input = 's';
                    break;

                case sf::Keyboard::D:
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
                if (input != '0')
                    break;
            }
        }

        // Logic chuyển dộng

        bool is_move_key = (input == 'w' || input == 'a' || input == 's' || input == 'd');

        if (input == '0')
            continue;

        sf::Time current_time = clock.getElapsedTime();

        // Xử lý khi spam phím
        if (is_move_key)
        {
            if (current_time - last_move_time < move_delay)
            {
                sf::Event flush_event;
                while (window.pollEvent(flush_event))
                {
                }
                continue;
            }
            else
            {
                last_move_time = current_time;
            }
        }

        // Quit
        if (input == 'q' || input == 'Q')
        {
            window.close();
            return -1;
        }

        // redo
        if (input == 'r' || input == 'R')
        {
            if (player.size() <= 1)
                continue;

            // go back
            player.pop_back();
            mummy.pop_back();
            player_r = player.back().first;
            player_c = player.back().second;
            mummy_r = mummy.back().first;
            mummy_c = mummy.back().second;

            continue;
        }

        // reset
        if (input == 'm' || input == 'M')
        {

            // return to first stored location
            player_r = player.front().first;
            player_c = player.front().second;
            mummy_r = mummy.front().first;
            mummy_c = mummy.front().second;

            // Xóa và tái tạo lịch sử
            player.clear();
            mummy.clear();
            player.push_back({player_r, player_c});
            mummy.push_back({mummy_r, mummy_c});
            continue;
        }

        // Player movement
        int nxt_r = player_r, nxt_c = player_c;
        player_move(wall, input, player_r, player_c, nxt_r, nxt_c);
        caught = false;
        int current_moves = player.size(); // Số bước đi trước khi Mob di chuyển

        if (nxt_r != -1 && nxt_c != -1)
        {

            // 1. Kiểm tra va chạm với Mob (Ưu tiên 1)
            if (nxt_r == mummy_r && nxt_c == mummy_c)
            {
                caught = true;
            }

            // 2. Kiểm tra điều kiện Win (Chỉ khi KHÔNG bị bắt)
            else if (nxt_r == win_r && nxt_c == win_c)
            {
                win = true;
            }

            // Cập nhật vị trí của người chơi
            player_r = nxt_r;
            player_c = nxt_c;
            player.push_back({player_r, player_c});
            current_moves = player.size() - 1;

            // Nếu người chơi bị bắt hoặc thắng, bỏ qua di chuyển của Mob
            if (caught || win)
            {
                mummy.push_back({mummy_r, mummy_c});
                continue;
            }

            // mummy move (2 steps random)
            int dr[4] = {0, 0, -1, 1};
            int dc[4] = {-1, 1, 0, 0};
            int directions[4] = {1, 2, 4, 8};
            for (int step = 0; step < 2; step++)
            {
                vector<int> valid;

                for (int d = 0; d < 4; d++)
                {
                    // Kiểm tra tường và không đi ra ngoài biên
                    int temp_r = mummy_r + dr[d];
                    int temp_c = mummy_c + dc[d];
                    if (!(wall[mummy_r][mummy_c] & directions[d]) && temp_r >= 0 && temp_r < n_row && temp_c >= 0 && temp_c < n_col)
                        valid.push_back(d);
                }

                if (valid.empty())
                    break;
                int d = valid[Rand(0, valid.size() - 1)];

                int delta_r = dr[d];
                int delta_c = dc[d];

                mummy_r += delta_r;
                mummy_c += delta_c;

                if (mummy_r == player_r && mummy_c == player_c)
                {
                    caught = true;
                    break;
                }

                // Khắc phục flickering: Cập nhật màn hình sau mỗi bước đi của mob
                window.clear();
                visualise_game(window, wall, player_r, player_c, mummy_r, mummy_c, -1, -1, -1, -1, -1, -1, win_r, win_c);
                draw_hud(window, current_moves, (mummy_r != -1), 0);
                window.display();

                sf_sleep(120);
            }
            mummy.push_back({mummy_r, mummy_c});
        }
    }
    return -1; // Cửa sổ đóng
}

int run_level(vector<string> &maze, vector<vector<int>> &wall, sf::RenderWindow &window, int n_row, int n_col, int win_r, int win_c)
{
    int player_r = -1, player_c = -1;
    int mummy_r = -1, mummy_c = -1;
    int mummy2_r = -1, mummy2_c = -1;
    int trap_r = -1, trap_c = -1;
    int scorpion_r = -1, scorpion_c = -1;

    // Scan maze for initial positions
    for (int row = 0; row < n_row; ++row)
    {
        for (int col = 0; col < n_col; ++col)
        {
            if (maze[row][col] == 'M')
            {
                if (mummy_r == -1)
                    mummy_r = row, mummy_c = col;
                else
                    mummy2_r = row, mummy2_c = col;
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
                scorpion_r = row, scorpion_c = col;
            }
        }
    }

    bool caught = false, win = false, trap = false;
    vector<pair<int, int>> player, mummy, mummy2, scorpion;
    player.push_back({player_r, player_c});
    mummy.push_back({mummy_r, mummy_c});
    mummy2.push_back({mummy2_r, mummy2_c});
    scorpion.push_back({scorpion_r, scorpion_c});

    // Kiểm soát tốc độ
    sf::Clock clock;
    const sf::Time move_delay = sf::milliseconds(150);
    sf::Time last_move_time = clock.getElapsedTime() - move_delay;
    // -----------------------------------------------------

    // Tạo sprite cho nút HUD
    sf::Texture texUndo, texReset, texHome;
    bool texturesLoaded = true;

    // Kích thước mục tiêu cho các nút HUD
    const float HUD_BTN_TARGET_WIDTH = 140.f;
    const float HUD_BTN_TARGET_HEIGHT = 50.f;

    // Load từng texture
    if (!texUndo.loadFromFile("undo_btn.png"))
    {
        cerr << "Failed to load undo_btn.png" << endl;
        texturesLoaded = false;
    }
    if (!texReset.loadFromFile("reset_btn.png"))
    {
        cerr << "Failed to load reset_btn.png" << endl;
        texturesLoaded = false;
    }
    if (!texHome.loadFromFile("home_btn.png"))
    {
        cerr << "Failed to load home_btn.png" << endl;
        texturesLoaded = false;
    }

    sf::Sprite btnUndo, btnReset, btnHome;
    float spriteButtonX = (200 - HUD_BTN_TARGET_WIDTH) / 2.f; // SIDEBAR_WIDTH = 200

    if (texturesLoaded)
    {
        btnUndo.setTexture(texUndo);
        btnReset.setTexture(texReset);
        btnHome.setTexture(texHome);

        // Tính scale cho từng nút để fit vào kích thước mục tiêu
        auto calculateScaleAndPosition = [&](sf::Sprite &sprite, sf::Texture &texture, float targetY)
        {
            // Tính scale để fit vào target size, giữ aspect ratio
            float texWidth = (float)texture.getSize().x;
            float texHeight = (float)texture.getSize().y;
            float scaleX = HUD_BTN_TARGET_WIDTH / texWidth;
            float scaleY = HUD_BTN_TARGET_HEIGHT / texHeight;
            float uniformScale = min(scaleX, scaleY);

            // Tính kích thước thực tế
            float actualWidth = texWidth * uniformScale;
            float actualHeight = texHeight * uniformScale;

            // Đặt scale và vị trí (căn giữa)
            sprite.setScale(uniformScale, uniformScale);
            sprite.setPosition(spriteButtonX + (HUD_BTN_TARGET_WIDTH - actualWidth) / 2.f,
                               targetY + (HUD_BTN_TARGET_HEIGHT - actualHeight) / 2.f);
        };

        calculateScaleAndPosition(btnUndo, texUndo, HUD_UNDO_BUTTON_Y);
        calculateScaleAndPosition(btnReset, texReset, HUD_RESET_BUTTON_Y);
        calculateScaleAndPosition(btnHome, texHome, HUD_MENU_BUTTON_Y);

        cout << "HUD buttons loaded successfully" << endl;
    }
    else
    {
        cerr << "Some HUD textures failed to load, using fallback" << endl;
    }
    // ------------------------------

    while (window.isOpen())
    {
        // Vẽ
        window.clear();
        visualise_game(window, wall, player_r, player_c, mummy_r, mummy_c, mummy2_r, mummy2_c, scorpion_r, scorpion_c, trap_r, trap_c, win_r, win_c);
        draw_hud(window, player.size() - 1, (mummy_r != -1) + (mummy2_r != -1), (scorpion_r != -1));
        window.display();

        // Kiểm tra điều kiện
        if (trap)
        {
            return 0; // Game Over by Trap
        }
        if (caught)
        {
            return 0; // Game Over by Mob
        }
        if (win)
        {
            return 1; // Win
        }

        char input = '0';

        // Xử lí input
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                return -1; // -1: Cửa sổ đóng
            }

            // Xử lý nút BACK TO MENU, UNDO, RESET
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                float mx = event.mouseButton.x;
                float my = event.mouseButton.y;

                if (texturesLoaded)
                {
                    // Kiểm tra click bằng sprite bounds
                    if (btnHome.getGlobalBounds().contains(mx, my))
                        return -1;
                    else if (btnUndo.getGlobalBounds().contains(mx, my))
                        input = 'r';
                    else if (btnReset.getGlobalBounds().contains(mx, my))
                        input = 'm';
                }
                else
                {
                    // Fallback: dùng phương pháp cũ nếu không tải được texture
                    float buttonX = (200 - HUD_BUTTON_W) / 2.f;
                    if (is_button_clicked(event, buttonX, HUD_MENU_BUTTON_Y, HUD_BUTTON_W, HUD_BUTTON_H))
                        return -1;
                    if (is_button_clicked(event, buttonX, HUD_UNDO_BUTTON_Y, HUD_BUTTON_W, HUD_BUTTON_H))
                        input = 'r';
                    else if (is_button_clicked(event, buttonX, HUD_RESET_BUTTON_Y, HUD_BUTTON_W, HUD_BUTTON_H))
                        input = 'm';
                }
            }

            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                case sf::Keyboard::W:
                case sf::Keyboard::Up:
                    input = 'w';
                    break;

                case sf::Keyboard::A:
                case sf::Keyboard::Left:
                    input = 'a';
                    break;

                case sf::Keyboard::S:
                case sf::Keyboard::Down:
                    input = 's';
                    break;

                case sf::Keyboard::D:
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
                if (input != '0')
                    break;
            }
        }

        // --- BƯỚC 4: XỬ LÝ LOGIC CHUYỂN ĐỘNG VÀ THROTTLING ---

        bool is_move_key = (input == 'w' || input == 'a' || input == 's' || input == 'd');

        if (input == '0')
            continue;

        sf::Time current_time = clock.getElapsedTime();

        // --- LOGIC XỬ LÝ SPAM PHÍM ---
        if (is_move_key)
        {
            if (current_time - last_move_time < move_delay)
            {
                sf::Event flush_event;
                while (window.pollEvent(flush_event))
                {
                }
                continue;
            }
            else
            {
                last_move_time = current_time;
            }
        }

        // Quit
        if (input == 'q' || input == 'Q')
        {
            window.close();
            return -1;
        }

        // Redo
        if (input == 'r' || input == 'R')
        {
            if (player.size() <= 1)
                continue;

            // Xóa trạng thái hiện tại
            player.pop_back();
            mummy.pop_back();
            mummy2.pop_back();
            scorpion.pop_back();

            // Quay về trạng thái trước đó
            player_r = player.back().first, player_c = player.back().second;
            mummy_r = mummy.back().first, mummy_c = mummy.back().second;
            mummy2_r = mummy2.back().first, mummy2_c = mummy2.back().second;
            scorpion_r = scorpion.back().first, scorpion_c = scorpion.back().second;

            continue;
        }

        // Reset
        if (input == 'm' || input == 'M')
        {
            // Quay về vị trí ban đầu (phần tử đầu tiên)
            player_r = player.front().first, player_c = player.front().second;
            mummy_r = mummy.front().first, mummy_c = mummy.front().second;
            mummy2_r = mummy2.front().first, mummy2_c = mummy2.front().second;
            scorpion_r = scorpion.front().first, scorpion_c = scorpion.front().second;

            // Xóa lịch sử và khôi phục lại trạng thái ban đầu
            player.clear(), mummy.clear(), mummy2.clear(), scorpion.clear();
            player.push_back({player_r, player_c});
            mummy.push_back({mummy_r, mummy_c});
            mummy2.push_back({mummy2_r, mummy2_c});
            scorpion.push_back({scorpion_r, scorpion_c});
            continue;
        }

        // Player move
        int nxt_player_r = player_r, nxt_player_c = player_c;
        player_move(wall, input, player_r, player_c, nxt_player_r, nxt_player_c);
        caught = false;
        int current_moves = player.size();

        if (nxt_player_r != -1 && nxt_player_c != -1)
        {

            // 1. Kiểm tra Va chạm Mob (Ưu tiên 1)
            if ((nxt_player_r == mummy_r && nxt_player_c == mummy_c) ||
                (nxt_player_r == mummy2_r && nxt_player_c == mummy2_c) ||
                (nxt_player_r == scorpion_r && nxt_player_c == scorpion_c))
            {
                caught = true;
            }

            // 2. Kiểm tra Bẫy (Ưu tiên 1)
            else if (nxt_player_r == trap_r && nxt_player_c == trap_c)
            {
                trap = true;
            }

            // 3. Kiểm tra Win (Ưu tiên 2: CHỈ NẾU KHÔNG bị bắt và KHÔNG mắc bẫy)
            else if (nxt_player_r == win_r && nxt_player_c == win_c)
            {
                win = true;
            }

            // Commit Player Move and History
            player_r = nxt_player_r, player_c = nxt_player_c;
            player.push_back({player_r, player_c});
            current_moves = player.size() - 1;

            if (caught || trap || win)
            {
                mummy.push_back({mummy_r, mummy_c});
                mummy2.push_back({mummy2_r, mummy2_c});
                scorpion.push_back({scorpion_r, scorpion_c});
                continue;
            }

            // --- MOB MOVES ---

            if (mummy_r != -1)
            {
                white_mummy_move(wall, mummy_r, mummy_c, caught, player_r, player_c,
                                 mummy2_r, mummy2_c, scorpion_r, scorpion_c, trap_r, trap_c, window, win_r, win_c, current_moves);
            }
            mummy.push_back({mummy_r, mummy_c});
            if (caught)
                continue;

            if (mummy2_r != -1)
            {
                white_mummy_move(wall, mummy2_r, mummy2_c, caught, player_r, player_c,
                                 mummy_r, mummy_c, scorpion_r, scorpion_c, trap_r, trap_c, window, win_r, win_c, current_moves);
            }
            mummy2.push_back({mummy2_r, mummy2_c});
            if (caught)
                continue;

            if (scorpion_r != -1)
            {
                white_scorpion_move(wall, scorpion_r, scorpion_c, caught, player_r, player_c,
                                    mummy_r, mummy_c, mummy2_r, mummy2_c, trap_r, trap_c, window, win_r, win_c, current_moves);
            }
            scorpion.push_back({scorpion_r, scorpion_c});
            if (caught)
                continue;
        }
    }
    return -1; // Cửa sổ đóng
}

int main()
{
    // ref maze: https://postimg.cc/XGWs1z5c (player voi mummy lech 1 o)
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

    // ref maze: https://postimg.cc/HVp8V5q4
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

    // ref maze (8x8): https://postimg.cc/bGrYr3Qk/b9f376e3
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

    // ref maze (10x10): https://postimg.cc/4nppKXn1
    std::vector<std::string> maze_10_1 = {
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

    std::vector<std::vector<int>> wall_10_1 = {
        {13, 4, 4, 4, 4, 4, 4, 12, 12, 6},
        {5, 10, 3, 1, 0, 8, 2, 13, 12, 10},
        {3, 5, 8, 0, 2, 7, 1, 6, 5, 14},
        {1, 0, 6, 9, 2, 3, 1, 2, 3, 7},
        {1, 6, 1, 6, 3, 9, 8, 0, 0, 2},
        {3, 1, 2, 1, 0, 6, 5, 8, 8, 10},
        {3, 1, 0, 2, 11, 1, 8, 4, 4, 14},
        {1, 0, 8, 0, 6, 11, 7, 1, 8, 14},
        {1, 0, 4, 10, 1, 4, 2, 9, 4, 6},
        {11, 9, 8, 14, 9, 10, 9, 12, 8, 10}};

    sf::RenderWindow window(sf::VideoMode(840, 640), "Mummy Game");

    enum GameState
    {
        MENU,
        HOW_TO_PLAY,
        PLAYING,
        GAME_OVER,
        GAME_WIN,
        NEXT_LEVEL
    };
    GameState state = MENU;
    int cur_level = load_progress(); // Lấy level đã lưu (0-4)

    // Khai báo hằng số cho tọa độ Exit của các level
    const pair<int, int> exits[] = {
        {5, 0}, // Level 0 (Tutorial)
        {5, 3}, // Level 1 (6x6)
        {0, 0}, // Level 2 (6x6)
        {0, 0}, // Level 3 (8x8)
        {0, 9}  // Level 4 (10x10) - Đã sửa để khớp với vị trí 'E' (hàng 9, cột 4)
    };

    while (window.isOpen())
    {

        // --- Xử lý Logic Chuyển trạng thái ---
        if (state == MENU)
        {
            int selection = handle_main_menu_input(window);
            if (selection == 1)
            {                  // NEW GAME
                cur_level = 0; // Bắt đầu ở Level 1
                save_progress(cur_level);
                state = PLAYING;
            }
            else if (selection == 2)
            { // RESUME GAME
                // Nếu chưa chơi bao giờ (cur_level = 0), bắt đầu từ level 1
                if (cur_level == 0 || cur_level > 4)
                    cur_level = 0;
                state = PLAYING;
            }
            else if (selection == 3)
            { // GAME PLOT & HOW TO PLAY
                state = HOW_TO_PLAY;
            }
        }
        else if (state == HOW_TO_PLAY)
        {
            if (handle_how_to_play_input(window))
                state = MENU;
        }
        else if (state == GAME_OVER)
        {
            if (handle_game_over_input(window))
                state = MENU;
        }
        else if (state == GAME_WIN)
        {
            if (handle_game_win_input(window))
                state = MENU;
        }
        else if (state == NEXT_LEVEL)
        {
            if (handle_next_level_input(window))
                state = PLAYING;
        }

        // --- Vẽ ---

        if (state == MENU)
        {
            window.clear();
            draw_main_menu(window);
            window.display();
        }
        else if (state == HOW_TO_PLAY)
        {
            window.clear();
            draw_how_to_play(window);
            window.display();
        }
        else if (state == GAME_OVER)
        {
            window.clear();
            draw_game_over(window);
            window.display();
        }
        else if (state == GAME_WIN)
        {
            window.clear();
            draw_game_win(window);
            window.display();
        }
        else if (state == NEXT_LEVEL)
        {
            window.clear();
            draw_next_level_screen(window);
            window.display();
        }
        else if (state == PLAYING)
        {
            int result;
            int win_r = exits[cur_level].first;
            int win_c = exits[cur_level].second;

            // Chạy level tương ứng
            if (cur_level == 0)
                result = level_0(window);
            else if (cur_level == 1)
                result = run_level(maze_6_1, wall_6_1, window, 6, 6, win_r, win_c);
            else if (cur_level == 2)
                result = run_level(maze_6_2, wall_6_2, window, 6, 6, win_r, win_c);
            else if (cur_level == 3)
                result = run_level(maze_8_1, wall_8_1, window, 8, 8, win_r, win_c);
            else if (cur_level == 4)
                result = run_level(maze_10_1, wall_10_1, window, 10, 10, win_r, win_c);
            else
                result = 1; // all levels completed

            if (result == 1)
            { // Thắng level hiện tại

                cur_level++;              // Tăng level lên level TIẾP THEO (ĐÃ SỬA LỖI)
                save_progress(cur_level); // Lưu level TIẾP THEO

                if (cur_level > 4)
                { // Sau level 4 là Win
                    state = GAME_WIN;
                }
                else
                {
                    state = NEXT_LEVEL;
                }
            }
            else if (result == 0) // Thua (Game Over)
                state = GAME_OVER;
            else if (result == -1)
            {
                // -1: window.close() HOẶC nhấn nút BACK TO MENU
                if (window.isOpen())
                {
                    state = MENU; // Quay về Menu
                }
                else
                {
                    break; // Thoát vòng lặp chính (nếu cửa sổ bị đóng)
                }
            }
        }
        sf_sleep(20);
    }
    return 0;
}