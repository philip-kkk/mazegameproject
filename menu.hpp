#ifndef MENU_HPP
#define MENU_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct VideoBG
{
    string pattern; // "Media/menuFrames/frame_%04d.jpg"
    float fps = 30.f;
    int frame = 0;
    bool loaded = false;

    sf::Clock clock;
    sf::Texture tex;
    sf::Sprite spr;
};

// trả về true nếu load OK
bool video_init(VideoBG &v, const string &pattern, float fps);

// gọi mỗi frame
void video_update(VideoBG &v, int mx, int reset_idx);

// vẽ full window
void video_draw(VideoBG &v, sf::RenderWindow &window, float startX, float startY);

extern sf::Music menu_music;
extern sf::Music fire_sound;
extern sf::Music next_level_sound;
extern sf::Music game_sound;
extern sf::Music game_over_sound;
extern sf::Music game_win_sound;

void start_menu_music();
void stop_menu_music();
void start_game_music();
void stop_game_music();
void start_game_over_music();
void stop_game_over_music();
void start_next_level_music();
void stop_next_level_music();
void start_game_win_music();
void stop_game_win_music();

// BUTTON UTILITIES
bool is_button_clicked(const sf::Event &event, float x, float y, float width, float height);
void draw_button(sf::RenderWindow &window, sf::Font &font, const string &text,
                 float x, float y, float width, float height, sf::Color bgColor, sf::Color textColor);

// HUD BUTTONS CONSTANTS
extern sf::FloatRect HUD_HOME_BOUNDS;
extern sf::FloatRect HUD_UNDO_BOUNDS;
extern sf::FloatRect HUD_RESET_BOUNDS;
extern const float HUD_BUTTON_X;
extern const float HUD_BUTTON_W;
extern const float HUD_BUTTON_H;
extern const float HUD_MENU_BUTTON_Y;
extern const float HUD_UNDO_BUTTON_Y;
extern const float HUD_RESET_BUTTON_Y;

extern sf::Texture bg;
extern sf::Texture sidebar_bg;
extern sf::Texture right_bg;
extern sf::Texture tex_title;
extern sf::Texture tex_new;
extern sf::Texture tex_resume;
extern sf::Texture tex_how;
extern sf::Texture tex_game_over;
extern sf::Texture tex_level_clear;
extern sf::Texture home;
extern sf::Texture reset;
extern sf::Texture undo;

// SPRITE BUTTON FUNCTIONS
sf::FloatRect draw_hud_sprite_button(sf::RenderWindow &window, const string &textureFile,
                                     float x, float y, bool &isHovered,
                                     float targetWidth = 140.f,
                                     float targetHeight = 50.f);

// HUD DRAWING
void draw_hud(sf::RenderWindow &window, int moves, int goblin_alive, int slime_alive);
void load_hud_textures();

// MAIN MENU
void draw_main_menu(sf::RenderWindow &window);
int handle_main_menu_input(sf::RenderWindow &window);

// HOW TO PLAY
void draw_how_to_play(sf::RenderWindow &window);
bool handle_how_to_play_input(sf::RenderWindow &window);
// GAME STATES

void draw_game_over(sf::RenderWindow &window);
bool handle_game_over_input(sf::RenderWindow &window);
void draw_game_win(sf::RenderWindow &window);
bool handle_game_win_input(sf::RenderWindow &window);
void draw_next_level_screen(sf::RenderWindow &window);
bool handle_next_level_input(sf::RenderWindow &window);

#endif