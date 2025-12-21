#include "visualise.hpp"
#include "frontend.hpp"
#include "menu.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

bool textures_loaded = false;
static bool sfx_loaded = false;

sf::SoundBuffer buf_step, buf_slime, buf_goblin;
sf::Sound snd_step_player;
sf::Sound snd_step_goblin1;
sf::Sound snd_step_goblin2;
sf::Sound snd_step_slime;

EntityAnimation player_anim = {0, 0, 0, 0, 1.0f, false, 0.9f, 1, false};
EntityAnimation goblin_anim = {0, 0, 0, 0, 1.0f, false, 1.8f, 1, false};
EntityAnimation goblin2_anim = {0, 0, 0, 0, 1.0f, false, 1.8f, 1, false};
EntityAnimation slime_anim = {0, 0, 0, 0, 1.0f, false, 0.9f, 1, false};

// Player
sf::Texture tex_player_idle_up;
sf::Texture tex_player_idle_down;
sf::Texture tex_player_idle_left;
sf::Texture tex_player_idle_right;

sf::Texture tex_player_motion_up;
sf::Texture tex_player_motion_down;
sf::Texture tex_player_motion_left;
sf::Texture tex_player_motion_right;

sf::Texture tex_player_motion2_up;
sf::Texture tex_player_motion2_down;
sf::Texture tex_player_motion2_left;
sf::Texture tex_player_motion2_right;

// Goblin
sf::Texture tex_goblin_idle_up;
sf::Texture tex_goblin_idle_down;
sf::Texture tex_goblin_idle_left;
sf::Texture tex_goblin_idle_right;

sf::Texture tex_goblin_motion_up;
sf::Texture tex_goblin_motion_down;
sf::Texture tex_goblin_motion_left;
sf::Texture tex_goblin_motion_right;

sf::Texture tex_goblin_motion2_up;
sf::Texture tex_goblin_motion2_down;
sf::Texture tex_goblin_motion2_left;
sf::Texture tex_goblin_motion2_right;

// Slime
sf::Texture tex_slime_idle_up;
sf::Texture tex_slime_idle_down;
sf::Texture tex_slime_idle_left;
sf::Texture tex_slime_idle_right;

const float ANIMATION_SPEED = 0.25f;
sf::Clock animation_clock;
float total_animation_time = 0.f;

void load_anh()
{
    if (textures_loaded)
        return;

    auto load = [](sf::Texture &t, const string &path)
    {
        if (!t.loadFromFile(path))
            cout << "Cannot load " << path << endl;
        t.setSmooth(false);
    };

    // Player
    load(tex_player_idle_up, "Media/Player/player_idle_up.png");
    load(tex_player_idle_down, "Media/Player/player_idle_down.png");
    load(tex_player_idle_left, "Media/Player/player_idle_left.png");
    load(tex_player_idle_right, "Media/Player/player_idle_right.png");

    load(tex_player_motion_up, "Media/Player/player_motion_up.png");
    load(tex_player_motion_down, "Media/Player/player_motion_down.png");
    load(tex_player_motion_left, "Media/Player/player_motion_left.png");
    load(tex_player_motion_right, "Media/Player/player_motion_right.png");

    load(tex_player_motion2_up, "Media/Player/player_motion2_up.png");
    load(tex_player_motion2_down, "Media/Player/player_motion2_down.png");
    load(tex_player_motion2_left, "Media/Player/player_motion2_left.png");
    load(tex_player_motion2_right, "Media/Player/player_motion2_right.png");

    // Goblin
    load(tex_goblin_idle_up, "Media/Goblin/goblin_idle_up.png");
    load(tex_goblin_idle_down, "Media/Goblin/goblin_idle_down.png");
    load(tex_goblin_idle_left, "Media/Goblin/goblin_idle_left.png");
    load(tex_goblin_idle_right, "Media/Goblin/goblin_idle_right.png");

    load(tex_goblin_motion_up, "Media/Goblin/goblin_motion_up.png");
    load(tex_goblin_motion_down, "Media/Goblin/goblin_motion_down.png");
    load(tex_goblin_motion_left, "Media/Goblin/goblin_motion_left.png");
    load(tex_goblin_motion_right, "Media/Goblin/goblin_motion_right.png");

    load(tex_goblin_motion2_up, "Media/Goblin/goblin_motion2_up.png");
    load(tex_goblin_motion2_down, "Media/Goblin/goblin_motion2_down.png");
    load(tex_goblin_motion2_left, "Media/Goblin/goblin_motion2_left.png");
    load(tex_goblin_motion2_right, "Media/Goblin/goblin_motion2_right.png");

    // Slime
    load(tex_slime_idle_up, "Media/Slime/slime_idle_up.png");
    load(tex_slime_idle_down, "Media/Slime/slime_idle_down.png");
    load(tex_slime_idle_left, "Media/Slime/slime_idle_left.png");
    load(tex_slime_idle_right, "Media/Slime/slime_idle_right.png");

    load(tex_trap, "Media/Environment/trap.png");
    load(tex_gate, "Media/Environment/gate.png");
    load(tex_wall_h, "Media/Environment/wall_horizontal.png");
    load(tex_wall_v, "Media/Environment/wall_vertical.png");
    load(tex_tile, "Media/Environment/Maze 1.png");
    load(tex_tile2, "Media/Environment/Maze 2.png");
    load(tex_tile3, "Media/Environment/Maze 3.png");
    load(tex_tile4, "Media/Environment/Maze 4.png");
    load(tex_tile5, "Media/Environment/Maze 5.png");
    load(tex_tile6, "Media/Environment/Maze 6.png");
    load(tex_tile7, "Media/Environment/Maze 7.png");
    load(tex_tile8, "Media/Environment/Maze 8.png");
    load(border_vertical, "Media/Environment/border_vertical.png");
    load(border_horizontal, "Media/Environment/border_horizontal.png");
    tex_wall_h.setRepeated(true);
    tex_wall_v.setRepeated(true);

    textures_loaded = true;
}

static void load_sfx()
{
    if (sfx_loaded)
        return;

    if (!buf_step.loadFromFile("Audio/concrete-footsteps-6752.wav"))
    {
        cout << "Cannot load footstep.wav\n";
    }

    if (!buf_slime.loadFromFile("Audio/slime_sound.wav"))
    {
        cout << "Cannot load slime_sound.wav\n";
    }

    if (!buf_goblin.loadFromFile("Audio/goblin_talk.wav"))
    {
        cout << "Cannot load goblin_talk.wav\n";
    }

    snd_step_player.setBuffer(buf_step);
    snd_step_goblin1.setBuffer(buf_goblin);
    snd_step_goblin2.setBuffer(buf_goblin);
    snd_step_slime.setBuffer(buf_slime);

    snd_step_player.setLoop(true);
    snd_step_goblin1.setLoop(true);
    snd_step_goblin2.setLoop(true);
    snd_step_slime.setLoop(true);

    snd_step_player.setVolume(120.f);
    snd_step_goblin1.setVolume(70.f);
    snd_step_goblin2.setVolume(70.f);
    snd_step_slime.setVolume(70.f);

    sfx_loaded = true;
}

static void update_loop_sound(sf::Sound &s, bool shouldPlay)
{
    auto st = s.getStatus();
    if (shouldPlay)
    {
        if (st != sf::Sound::Playing)
            s.play();
    }
    else
    {
        if (st == sf::Sound::Playing)
            s.stop();
    }
}

static bool gFadeActive = false;
static float gFadeT = 0.f;
static float gFadeDur = 0.6f;
static sf::RectangleShape gFadeRect;

void gameplay_fade_init(sf::RenderWindow &window)
{
    gFadeRect.setSize(sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
    gFadeRect.setPosition(0.f, 0.f);
    gFadeRect.setFillColor(sf::Color(0, 0, 0, 0));
}

void gameplay_fade_start_in(float dur)
{
    gFadeActive = true;
    gFadeDur = dur;
    gFadeT = 0.f;
    gFadeRect.setFillColor(sf::Color(0, 0, 0, 255)); // bắt đầu từ đen
}

void gameplay_fade_update(float dt)
{
    if (!gFadeActive)
        return;

    gFadeT += dt;
    float p = gFadeT / gFadeDur;
    if (p > 1.f)
        p = 1.f;

    // fade-in: alpha 255 -> 0
    int a = (int)(255.f * (1.f - p));
    if (a < 0)
        a = 0;
    gFadeRect.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)a));

    if (p >= 1.f)
    {
        gFadeActive = false;
        gFadeRect.setFillColor(sf::Color(0, 0, 0, 0));
    }
}

void gameplay_fade_draw(sf::RenderWindow &window)
{
    if (gFadeActive)
        window.draw(gFadeRect);
}

static bool frameanim_init(FrameAnim &a, const string &pattern, float fps)
{
    a.pattern = pattern;
    a.fps = fps;
    a.frame = 1;
    a.fps = fps;
    a.acc = 0.f;
    a.loaded = false;

    a.tex.clear();
    a.tex.resize(13);

    for (int i = 1; i < 13; ++i)
    {
        char buf[512];
        snprintf(buf, sizeof(buf), pattern.c_str(), i);
        if (!a.tex[i].loadFromFile(buf))
        {
            a.loaded = false;
            return false;
        }
        a.tex[i].setSmooth(true);
    }
    a.loaded = true;
    return true;
}

static void frameanim_reset(FrameAnim &a)
{
    a.frame = 1;
    a.acc = 0.f;
}

static void frameanim_update(FrameAnim &a, float dt)
{
    if (!a.loaded)
        return;
    a.acc += dt;
    float frame_time = 1.f / a.fps;
    while (a.acc >= frame_time)
    {
        a.acc -= frame_time;
        a.frame = (a.frame + 1) % 12;
        if (a.frame == 0)
            a.frame = 12;
    }
}

static sf::Texture *frameanim_cur_texture(FrameAnim &a)
{
    if (!a.loaded)
        return nullptr;
    return &a.tex[a.frame];
}

FrameAnim slime_run[4], goblin_run[4], goblin2_run[4];

bool move_frames_loaded = false;

void init_move_frames()
{
    if (move_frames_loaded)
        return;
    const int N = 12;
    const float FPS = 12.f;
    frameanim_init(slime_run[0], "Media/Slime/slime_up/frame_%04d.png", FPS);
    frameanim_init(slime_run[1], "Media/Slime/slime_down/frame_%04d.png", FPS);
    frameanim_init(slime_run[2], "Media/Slime/slime_left/frame_%04d.png", FPS);
    frameanim_init(slime_run[3], "Media/Slime/slime_right/frame_%04d.png", FPS);
    move_frames_loaded = true;
}

void reset_animation_states()
{
    player_anim.progress = 1.0f;
    player_anim.is_animating = false;
    player_anim.moving = false;

    goblin_anim.progress = 1.0f;
    goblin_anim.is_animating = false;
    goblin_anim.moving = false;

    goblin2_anim.progress = 1.0f;
    goblin2_anim.is_animating = false;
    goblin2_anim.moving = false;

    slime_anim.progress = 1.0f;
    slime_anim.is_animating = false;
    slime_anim.moving = false;

    snd_step_player.stop();
    snd_step_goblin1.stop();
    snd_step_goblin2.stop();
    snd_step_slime.stop();
}

void update_entity_animation(EntityAnimation &anim, float delta_time)
{
    if (anim.is_animating)
    {
        // Sử dụng tốc độ riêng của từng entity
        anim.progress += delta_time / anim.speed;
        anim.moving = true;

        if (anim.progress >= 1.0f)
        {
            anim.progress = 1.0f;
            anim.is_animating = false;
            anim.moving = false;
        }
    }
}

void start_entity_animation(EntityAnimation &anim, int start_r, int start_c, int target_r, int target_c, int dir)
{
    anim.start_r = start_r;
    anim.start_c = start_c;
    anim.target_r = target_r;
    anim.target_c = target_c;
    anim.dir = dir;
    anim.progress = 0.0f;
    anim.is_animating = true;
    anim.moving = true;
}

void draw_animated_entity_multiframe(sf::RenderWindow &window, EntityAnimation &anim,
                                     sf::Texture &idle_up, sf::Texture &idle_down,
                                     sf::Texture &idle_left, sf::Texture &idle_right,
                                     FrameAnim run[4], float delta_time, sf::Sound &effect)
{
    update_loop_sound(effect, anim.moving);
    float current_r, current_c;
    if (anim.is_animating)
    {
        current_r = anim.start_r + (anim.target_r - anim.start_r) * anim.progress;
        current_c = anim.start_c + (anim.target_c - anim.start_c) * anim.progress;
    }
    else
    {
        current_r = anim.target_r;
        current_c = anim.target_c;
    }

    sf::Texture *texture = nullptr;

    if (anim.moving)
    {
        frameanim_update(run[anim.dir], delta_time);
        texture = frameanim_cur_texture(run[anim.dir]);
        if (!texture)
        {
            if (anim.dir == 0)
                texture = &idle_up;
            else if (anim.dir == 1)
                texture = &idle_down;
            else if (anim.dir == 2)
                texture = &idle_left;
            else
                texture = &idle_right;
        }
    }
    else
    {
        frameanim_reset(run[anim.dir]);
        if (anim.dir == 0)
            texture = &idle_up;
        else if (anim.dir == 1)
            texture = &idle_down;
        else if (anim.dir == 2)
            texture = &idle_left;
        else
            texture = &idle_right;
    }

    if (texture)
    {
        sf::Sprite sprite(*texture);
        float scale = TILE_SIZE / texture->getSize().x;
        sprite.setScale(scale, scale);
        sprite.setPosition(
            GLOBAL_OFFSET_X + current_c * TILE_SIZE,
            GLOBAL_OFFSET_Y + current_r * TILE_SIZE);
        window.draw(sprite);
    }
}

void draw_animated_entity(sf::RenderWindow &window,
                          EntityAnimation &anim,
                          sf::Texture &idle_up, sf::Texture &idle_down,
                          sf::Texture &idle_left, sf::Texture &idle_right,
                          sf::Texture &m1_up, sf::Texture &m1_down,
                          sf::Texture &m1_left, sf::Texture &m1_right,
                          sf::Texture &m2_up, sf::Texture &m2_down,
                          sf::Texture &m2_left, sf::Texture &m2_right)
{
    // Tính vị trí hiện tại bằng nội suy tuyến tính (LERP)
    float current_r, current_c;
    if (anim.is_animating)
    {
        current_r = anim.start_r + (anim.target_r - anim.start_r) * anim.progress;
        current_c = anim.start_c + (anim.target_c - anim.start_c) * anim.progress;
    }
    else
    {
        current_r = anim.target_r;
        current_c = anim.target_c;
    }

    // Lựa chọn texture dựa trên hướng và trạng thái
    sf::Texture *texture = nullptr;

    if (anim.moving)
    {
        // Tạo hiệu ứng bước đi bằng cách đổi texture theo thời gian
        // Chậm lại hiệu ứng bước đi (đổi frame mỗi 0.15s thay vì 0.1s)
        bool use_first_frame = (int(total_animation_time / 0.20f) % 2) == 0; // Đổi frame mỗi ~0.15s

        switch (anim.dir)
        {
        case 0: // UP
            texture = use_first_frame ? &m1_up : &m2_up;
            break;
        case 1: // DOWN
            texture = use_first_frame ? &m1_down : &m2_down;
            break;
        case 2: // LEFT
            texture = use_first_frame ? &m1_left : &m2_left;
            break;
        case 3: // RIGHT
            texture = use_first_frame ? &m1_right : &m2_right;
            break;
        }
    }
    else
    {
        // Trạng thái đứng yên
        switch (anim.dir)
        {
        case 0: // UP
            texture = &idle_up;
            break;
        case 1: // DOWN
            texture = &idle_down;
            break;
        case 2: // LEFT
            texture = &idle_left;
            break;
        case 3: // RIGHT
            texture = &idle_right;
            break;
        }
    }

    if (texture)
    {
        sf::Sprite sprite(*texture);
        float scale = TILE_SIZE / texture->getSize().x;
        sprite.setScale(scale, scale);

        // Vẽ ở vị trí hiện tại
        sprite.setPosition(
            GLOBAL_OFFSET_X + current_c * TILE_SIZE,
            GLOBAL_OFFSET_Y + current_r * TILE_SIZE);

        window.draw(sprite);
    }
}

void visualise_game(sf::RenderWindow &window, const vector<vector<int>> &wall,
                    int &player_r, int &player_c,
                    int goblin_r, int goblin_c,
                    int goblin2_r, int goblin2_c,
                    int slime_r, int slime_c,
                    int trap_r, int trap_c,
                    int win_r, int win_c,
                    int moves, int m_alive, int s_alive)
{
    window.clear();
    load_sfx();
    float delta_time = animation_clock.restart().asSeconds();
    init_move_frames();
    total_animation_time += delta_time;
    if (total_animation_time > 3600.f)
    {
        total_animation_time = fmod(total_animation_time, 3600.f);
    }
    // Cập nhật animation cho player
    if (player_r != -1)
    {
        if (!player_anim.is_animating &&
            (player_anim.target_r != player_r || player_anim.target_c != player_c))
        {
            start_entity_animation(player_anim, player_anim.target_r, player_anim.target_c,
                                   player_r, player_c, player_anim.dir);
        }
        update_entity_animation(player_anim, delta_time);
    }

    // Cập nhật animation cho goblin
    if (goblin_r != -1)
    {
        if (!goblin_anim.is_animating &&
            (goblin_anim.target_r != goblin_r || goblin_anim.target_c != goblin_c))
        {
            start_entity_animation(goblin_anim, goblin_anim.target_r, goblin_anim.target_c,
                                   goblin_r, goblin_c, goblin_anim.dir);
        }
        update_entity_animation(goblin_anim, delta_time);
    }

    // Cập nhật animation cho goblin2
    if (goblin2_r != -1)
    {
        if (!goblin2_anim.is_animating &&
            (goblin2_anim.target_r != goblin2_r || goblin2_anim.target_c != goblin2_c))
        {
            start_entity_animation(goblin2_anim, goblin2_anim.target_r, goblin2_anim.target_c,
                                   goblin2_r, goblin2_c, goblin2_anim.dir);
        }
        update_entity_animation(goblin2_anim, delta_time);
    }

    update_loop_sound(snd_step_player, (player_r != -1) && player_anim.moving);
    update_loop_sound(snd_step_goblin1, (goblin_r != -1) && goblin_anim.moving);
    update_loop_sound(snd_step_goblin2, (goblin2_r != -1) && goblin2_anim.moving);

    draw_hud(window, moves, m_alive, s_alive);
    draw_maze(window, wall, win_r, win_c);

    if (trap_r != -1)
        draw_entity(window, trap_r, trap_c, tex_trap);
    draw_animated_entity(window, player_anim,
                         tex_player_idle_up, tex_player_idle_down, tex_player_idle_left, tex_player_idle_right,
                         tex_player_motion_up, tex_player_motion_down, tex_player_motion_left, tex_player_motion_right,
                         tex_player_motion2_up, tex_player_motion2_down, tex_player_motion2_left, tex_player_motion2_right);

    if (goblin_r != -1)
        draw_animated_entity(window, goblin_anim,
                             tex_goblin_idle_up, tex_goblin_idle_down, tex_goblin_idle_left, tex_goblin_idle_right,
                             tex_goblin_motion_up, tex_goblin_motion_down, tex_goblin_motion_left, tex_goblin_motion_right,
                             tex_goblin_motion2_up, tex_goblin_motion2_down, tex_goblin_motion2_left, tex_goblin_motion2_right);
    if (goblin2_r != -1)
        draw_animated_entity(window, goblin2_anim,
                             tex_goblin_idle_up, tex_goblin_idle_down, tex_goblin_idle_left, tex_goblin_idle_right,
                             tex_goblin_motion_up, tex_goblin_motion_down, tex_goblin_motion_left, tex_goblin_motion_right,
                             tex_goblin_motion2_up, tex_goblin_motion2_down, tex_goblin_motion2_left, tex_goblin_motion2_right);
    if (slime_r != -1)
    {
        draw_animated_entity_multiframe(window, slime_anim, tex_slime_idle_up, tex_slime_idle_down,
                                        tex_slime_idle_left, tex_slime_idle_right, slime_run, delta_time, snd_step_slime);
    }
    gameplay_fade_update(delta_time);
    gameplay_fade_draw(window);
    window.display();
}
