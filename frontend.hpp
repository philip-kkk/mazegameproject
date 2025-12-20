#ifndef FRONTEND_HPP
#define FRONTEND_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

// --- ĐỊNH NGHĨA KÍCH THƯỚC CHUẨN ---
extern float TILE_SIZE;
extern float GLOBAL_OFFSET_X;
extern float GLOBAL_OFFSET_Y;

#define SIDEBAR_WIDTH 500 // Cột bên trái rộng 250px

extern sf::Texture tex_trap;
extern sf::Texture tex_gate;
extern sf::Texture tex_tile;
extern sf::Texture tex_tile2;
extern sf::Texture tex_tile3;
extern sf::Texture tex_tile4;
extern sf::Texture tex_tile5;
extern sf::Texture tex_tile6;
extern sf::Texture tex_tile7;
extern sf::Texture tex_tile8;
extern sf::Texture tex_wall_h;
extern sf::Texture tex_wall_v;
extern sf::Texture border_vertical;
extern sf::Texture border_horizontal;

// Thêm tham số offset_x, offset_y vào các hàm vẽ game
void draw_maze(sf::RenderWindow &window, const std::vector<std::vector<int>> &wall, int win_r, int win_c);
void draw_entity(sf::RenderWindow &window, int r, int c, sf::Texture &texture);
#endif