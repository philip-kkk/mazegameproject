#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include "frontend.hpp"

using namespace std;
struct EntityAnimation
{
    float start_r, start_c;   // Vị trí bắt đầu animation
    float target_r, target_c; // Vị trí đích (ô)
    float progress;           // Tiến trình animation (0-1)
    bool is_animating;        // Đang animation?
    float speed;              // Tốc độ animation
    int dir;                  // Hướng di chuyển
    bool moving;              // Đang di chuyển?
};
const int maxframe = 13;
struct FrameAnim
{
    string pattern;
    float fps = 12.f;
    int frame = 1;
    float acc = 0.f;

    bool loaded = false;
    vector<sf::Texture> tex;
};
extern float total_animation_time;
// Player textures (4 hướng × 3 trạng thái)
extern sf::Texture tex_player_idle_up;
extern sf::Texture tex_player_idle_down;
extern sf::Texture tex_player_idle_left;
extern sf::Texture tex_player_idle_right;

extern sf::Texture tex_player_motion_up;
extern sf::Texture tex_player_motion_down;
extern sf::Texture tex_player_motion_left;
extern sf::Texture tex_player_motion_right;

extern sf::Texture tex_player_motion2_up;
extern sf::Texture tex_player_motion2_down;
extern sf::Texture tex_player_motion2_left;
extern sf::Texture tex_player_motion2_right;

extern sf::Texture tex_goblin_idle_up;
extern sf::Texture tex_goblin_idle_down;
extern sf::Texture tex_goblin_idle_left;
extern sf::Texture tex_goblin_idle_right;

extern sf::Texture tex_goblin_motion_up;
extern sf::Texture tex_goblin_motion_down;
extern sf::Texture tex_goblin_motion_left;
extern sf::Texture tex_goblin_motion_right;

extern sf::Texture tex_goblin_motion2_up;
extern sf::Texture tex_goblin_motion2_down;
extern sf::Texture tex_goblin_motion2_left;
extern sf::Texture tex_goblin_motion2_right;

extern sf::Texture tex_slime_idle_up;
extern sf::Texture tex_slime_idle_down;
extern sf::Texture tex_slime_idle_left;
extern sf::Texture tex_slime_idle_right;

// Animation states
extern EntityAnimation player_anim;
extern EntityAnimation goblin_anim;
extern EntityAnimation goblin2_anim;
extern EntityAnimation slime_anim;

extern bool textures_loaded;
extern const float ANIMATION_SPEED;
void init_move_frames();
void draw_animated_entity_multiframe(sf::RenderWindow &window, EntityAnimation &anim,
                                     sf::Texture &idle_up, sf::Texture &idle_down,
                                     sf::Texture &idle_left, sf::Texture &idle_right,
                                     FrameAnim run[4], float delta_time);
void load_anh();
void gameplay_fade_init(sf::RenderWindow &window);
void gameplay_fade_start_in(float dur = 0.6f);
void gameplay_fade_update(float dt);
void gameplay_fade_draw(sf::RenderWindow &window);

void reset_animation_states();
void update_entity_animation(EntityAnimation &anim, float delta_time);
void start_entity_animation(EntityAnimation &anim, int start_r, int start_c, int target_r, int target_c, int dir);
void draw_animated_entity(sf::RenderWindow &window,
                          EntityAnimation &anim,
                          sf::Texture &idle_up, sf::Texture &idle_down,
                          sf::Texture &idle_left, sf::Texture &idle_right,
                          sf::Texture &m1_up, sf::Texture &m1_down,
                          sf::Texture &m1_left, sf::Texture &m1_right,
                          sf::Texture &m2_up, sf::Texture &m2_down,
                          sf::Texture &m2_left, sf::Texture &m2_right);
void visualise_game(sf::RenderWindow &window, const vector<vector<int>> &wall, int &player_r, int &player_c,
                    int goblin_r, int goblin_c, int goblin2_r, int goblin2_c, int slime_r, int slime_c,
                    int trap_r, int trap_c, int win_r, int win_c,
                    int moves, int m_alive, int s_alive);
// Thêm tham số thống kê
// m_alive: số goblin còn sống, s_alive: số slime còn sống
