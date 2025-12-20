#pragma once
#include "logic.hpp"
#include "visualise.hpp"
#include "frontend.hpp"

using namespace std;

int load_progress();
void reset_progress();
void save_progress(int level);

int level_0(sf::RenderWindow &window);
int run_level(vector<string> &maze, vector<vector<int>> &wall, sf::RenderWindow &window, int n_row, int n_col, int win_r, int win_c);
