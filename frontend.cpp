#include "frontend.hpp"
#include "logic.hpp"

#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <string>

using namespace std;

float TILE_SIZE;
float GLOBAL_OFFSET_X = 0;
float GLOBAL_OFFSET_Y = 0;

sf::Texture tex_trap;
sf::Texture tex_gate;
sf::Texture tex_tile;
sf::Texture tex_wall_h;
sf::Texture tex_wall_v;
sf::Texture tex_tile2;
sf::Texture tex_tile3;
sf::Texture tex_tile4;
sf::Texture tex_tile5;
sf::Texture tex_tile6;
sf::Texture tex_tile7;
sf::Texture tex_tile8;
sf::Texture border_vertical;
sf::Texture border_horizontal;

void draw_maze(sf::RenderWindow &window, const vector<vector<int>> &wall, int win_r, int win_c)
{
    int rows = wall.size();
    int cols = rows;

    const float MARGIN = 20.f;
    const float SIDEBAR = SIDEBAR_WIDTH;

    float usableW = window.getSize().x - SIDEBAR - MARGIN;
    float usableH = window.getSize().y - MARGIN - 250;

    float tile = min(usableW / cols, usableH / rows);

    // Offset căn giữa maze
    float offsetX = SIDEBAR + (usableW - cols * tile) / 2.f + 50;
    float offsetY = (window.getSize().y - rows * tile) / 2.f - 25;

    TILE_SIZE = tile;
    GLOBAL_OFFSET_X = offsetX;
    GLOBAL_OFFSET_Y = offsetY;

    tex_tile.setSmooth(false);
    tex_tile2.setSmooth(false);
    tex_tile3.setSmooth(false);
    tex_tile4.setSmooth(false);
    tex_tile5.setSmooth(false);
    tex_tile6.setSmooth(false);
    tex_tile7.setSmooth(false);
    tex_tile8.setSmooth(false);
    border_horizontal.setSmooth(false);
    border_vertical.setSmooth(false);
    tex_wall_h.setSmooth(false);
    tex_wall_v.setSmooth(false);
    tex_gate.setSmooth(false);

    // Title spirte
    sf::Sprite tileSprite(tex_tile);
    sf::Sprite tile2Sprite(tex_tile2);
    sf::Sprite tile3Sprite(tex_tile3);
    sf::Sprite tile4Sprite(tex_tile4);
    sf::Sprite tile5Sprite(tex_tile5);
    sf::Sprite tile6Sprite(tex_tile6);
    sf::Sprite tile7Sprite(tex_tile7);
    sf::Sprite tile8Sprite(tex_tile8);
    sf::Sprite gate(tex_gate);
    tileSprite.setScale(tile / tex_tile.getSize().x,
                        tile / tex_tile.getSize().y);
    tile2Sprite.setScale(tile / tex_tile2.getSize().x,
                         tile / tex_tile2.getSize().y);
    tile3Sprite.setScale(tile / tex_tile3.getSize().x,
                         tile / tex_tile3.getSize().y);
    tile4Sprite.setScale(tile / tex_tile4.getSize().x,
                         tile / tex_tile4.getSize().y);
    tile5Sprite.setScale(tile / tex_tile5.getSize().x,
                         tile / tex_tile5.getSize().y);
    tile6Sprite.setScale(tile / tex_tile6.getSize().x,
                         tile / tex_tile6.getSize().y);
    tile7Sprite.setScale(tile / tex_tile7.getSize().x,
                         tile / tex_tile7.getSize().y);
    tile8Sprite.setScale(tile / tex_tile8.getSize().x,
                         tile / tex_tile8.getSize().y);
    gate.setScale(tile / tex_gate.getSize().x,
                  tile / tex_gate.getSize().y);

    float overlap = 20 * tex_wall_h.getSize().x / tex_tile.getSize().x;

    sf::Sprite borderH(border_horizontal);
    borderH.setScale(
        tile / tex_tile.getSize().x,
        tile / tex_tile.getSize().x);
    sf::Sprite borderV(border_vertical);
    borderV.setScale(
        tile / tex_tile.getSize().x,
        tile / tex_tile.getSize().x);
    sf::Sprite wallH(tex_wall_h);
    wallH.setScale(
        tile / tex_tile.getSize().x,
        tile / tex_tile.getSize().x);

    sf::Sprite wallV(tex_wall_v);
    wallV.setScale(
        tile / tex_tile.getSize().x,
        tile / tex_tile.getSize().x);
    float borderV_width = border_vertical.getSize().x * borderV.getScale().x;
    float borderV_height = border_vertical.getSize().y * borderV.getScale().y;
    float borderH_width = border_horizontal.getSize().x * borderH.getScale().x;
    float borderH_height = border_horizontal.getSize().y * borderH.getScale().y;
    int cnt = 0;
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            float x = offsetX + c * tile;
            float y = offsetY + r * tile;
            if (r == win_r && c == win_c)
            {
                gate.setPosition(x, y), window.draw(gate);
            }
            else
            {
                int t = tile_type[cnt++];
                if (t % 8 == 0)
                    tileSprite.setPosition(x, y), window.draw(tileSprite);
                else if (t % 8 == 1)
                    tile2Sprite.setPosition(x, y), window.draw(tile2Sprite);
                else if (t % 8 == 2)
                    tile3Sprite.setPosition(x, y), window.draw(tile3Sprite);
                else if (t % 8 == 3)
                    tile4Sprite.setPosition(x, y), window.draw(tile4Sprite);
                else if (t % 8 == 4)
                    tile5Sprite.setPosition(x, y), window.draw(tile5Sprite);
                else if (t % 8 == 5)
                    tile6Sprite.setPosition(x, y), window.draw(tile6Sprite);
                else if (t % 8 == 6)
                    tile7Sprite.setPosition(x, y), window.draw(tile7Sprite);
                else
                    tile8Sprite.setPosition(x, y), window.draw(tile8Sprite);
            }
            if (r == rows - 1)
            {
                // Bottom border
                borderH.setPosition(x, y + tile);
                window.draw(borderH);
            }
            if (r == 0)
            {
                // Top
                borderH.setPosition(x, y - borderH_height);
                window.draw(borderH);
            }
            if (c == 0)
            {
                // Left
                borderV.setPosition(x - borderV_width, y - borderH_height);
                window.draw(borderV);
                borderV.setPosition(x - borderV_width, y + tile + borderH_height - borderV_height);
                window.draw(borderV);
            }
            if (c == cols - 1)
            {
                // Right border
                borderV.setPosition(x + tile, y - borderH_height);
                window.draw(borderV);
                borderV.setPosition(x + tile, y + tile + borderH_height - borderV_height);
                window.draw(borderV);
            }
        }
    }

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            float x = offsetX + c * tile;
            float y = offsetY + r * tile;

            int m = wall[r][c];
            // Right
            if (m & 2)
            {
                if (c != cols - 1)
                {
                    wallV.setPosition(x + tile - overlap / 2, y - overlap / 2);
                    window.draw(wallV);
                }
            }
            // Bottom
            if (m & 8)
            {
                if (r != rows - 1)
                {
                    wallH.setPosition(x - overlap / 2, y + tile - overlap / 2);
                    window.draw(wallH);
                }
            }
        }
    }
}

void draw_entity(sf::RenderWindow &window, int r, int c, sf::Texture &texture)
{
    sf::Sprite sprite(texture);

    texture.setSmooth(false);

    float texW = texture.getSize().x;
    float texH = texture.getSize().y;
    float scale = TILE_SIZE / max(texW, texH);

    sprite.setScale(scale, scale);

    sprite.setPosition(
        GLOBAL_OFFSET_X + c * TILE_SIZE + (TILE_SIZE - texW * scale) / 2,
        GLOBAL_OFFSET_Y + r * TILE_SIZE + (TILE_SIZE - texH * scale) / 2);

    window.draw(sprite);
}
