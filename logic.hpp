#pragma once
#include "visualise.hpp"
#include "frontend.hpp"
#include <SFML/Graphics.hpp>

using namespace std;
extern vector<int> tile_type;

int Rand(int l, int h);
int manhattan_dist(const int &player_r, const int &player_c, const int &goblin_r, const int &goblin_c);
int logicDir_to_visualDir(int logic_dir);
void player_move(const vector<vector<int>> &wall, char &input, int &player_r, int &player_c, int &nxt_player_r, int &nxt_player_c);
void white_goblin_move(const vector<vector<int>> &wall, int &goblin_r, int &goblin_c,
                       bool &caught, int &player_r, int &player_c,
                       int &goblin2_r, int &goblin2_c,
                       int &slime_r, int &slime_c, int &trap_r, int &trap_c,
                       sf::RenderWindow &window, int win_r, int win_c, int moves, EntityAnimation &anim, int type);
void white_slime_move(const vector<vector<int>> &wall, int &slime_r, int &slime_c,
                      bool &caught, int &player_r, int &player_c,
                      int &goblin_r, int &goblin_c,
                      int &goblin2_r, int &goblin2_c, int &trap_r, int &trap_c,
                      sf::RenderWindow &window, int win_r, int win_c, int moves);