#include "menu.hpp"
#include "frontend.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <map>

using namespace std;

sf::Music menu_music;
sf::Music fire_sound;
sf::Music clicked_sound;
sf::Music move_sound;
sf::Music game_sound;
sf::Music game_over_sound;
sf::Music game_win_sound;

// Kích thước
const int PADDING = 10;        // Khoảng đệm 10px
const int WINDOW_WIDTH = 1920; // Cửa sổ 840x640
const int WINDOW_HEIGHT = 1080;

// Hud bên trái maze phải
const int HUD_START_X = 0; // HUD bắt đầu từ X=0

// Chỉnh kích thước ảnh khi lấy
const int BTN_IMG_W = 612;
const int BTN_IMG_H = 408;
const int BTN_CROP_W = 271;
const int BTN_CROP_H = 63;
const int BTN_CROP_X = (BTN_IMG_W - BTN_CROP_W) / 2; // 170
const int BTN_CROP_Y = (BTN_IMG_H - BTN_CROP_H) / 2; // 172

const sf::IntRect BUTTON_RECT(BTN_CROP_X, BTN_CROP_Y, BTN_CROP_W, BTN_CROP_H);

const float NEW_CENTER_X = WINDOW_WIDTH / 2.f; // 420
// Vị trí X cho các nút căn giữa (420 - 271/2 = 284.5f)
const float BUTTON_X_CENTERED = NEW_CENTER_X - (BTN_CROP_W / 2.f);

const float BUTTON_W_MAIN_MENU = 914.f;
const float BUTTON_H_MAIN_MENU = 250.f;

// Kích thước menu chính
const float TITLE_IMAGE_SCALE = 0.8f; // Kích thước ảnh tiêu đề: 0.8f
const float TITLE_IMAGE_Y = 200.f;
const float MAIN_MENU_NEW_GAME_Y = 450.f;
const float MAIN_MENU_BUTTON_SPACING = 150.f;

// Kích thước nút về menu
const float HOW_TO_PLAY_BACK_BTN_Y = 800.f;

// Kích thước các nút còn lại trg HUD
const float HUD_BUTTON_W = 160.f;
const float HUD_BUTTON_H = 40.f;
// Vị trí X: Căn giữa trong sidebar (0-200). X = 20.f
const float HUD_BUTTON_X = HUD_START_X + (SIDEBAR_WIDTH - HUD_BUTTON_W) / 2.f;

// Tọa độ Y cho các nút (từ dưới lên)
const float HUD_MENU_BUTTON_Y = 550.f;  // Back to menu
const float HUD_UNDO_BUTTON_Y = 510.f;  // Undo (R)
const float HUD_RESET_BUTTON_Y = 460.f; // Reset (M)

// --- KÍCH THƯỚC MỤC TIÊU CHO NÚT HUD SPRITE ---
const float HUD_BUTTON_TARGET_WIDTH = 320.f;
const float HUD_BUTTON_TARGET_HEIGHT = 84.f;

sf::Texture bg;
sf::Texture right_bg;
sf::Texture sidebar_bg;
sf::Texture tex_continue;
sf::Texture tex_back;
sf::Texture tex_title;
sf::Texture tex_new;
sf::Texture tex_resume;
sf::Texture tex_how;
sf::Texture tex_game_over;
sf::Texture tex_level_clear;
sf::Texture home;
sf::Texture reset;
sf::Texture undo;

sf::FloatRect HUD_HOME_BOUNDS;
sf::FloatRect HUD_UNDO_BOUNDS;
sf::FloatRect HUD_RESET_BOUNDS;

void start_menu_music()
{
    menu_music.openFromFile("Audio/wrong-place-129242.wav");
    fire_sound.openFromFile("Audio/fire.wav");
    fire_sound.setLoop(true);
    fire_sound.setVolume(50.f);
    menu_music.setLoop(true);
    menu_music.setVolume(50.f);
    if (menu_music.getStatus() != sf::Music::Playing)
        menu_music.play();
    if (fire_sound.getStatus() != sf::Music::Playing)
        fire_sound.play();
}

void stop_menu_music()
{
    menu_music.stop();
}

void start_game_music()
{
    game_sound.openFromFile("Audio/adventures-loop-music-226836.wav");
    game_sound.setLoop(true);
    game_sound.setVolume(100.f);
    if (game_sound.getStatus() != sf::Music::Playing)
    {
        game_sound.play();
    }
}

void stop_game_music()
{
    game_sound.stop();
}

void start_footsteps()
{
    move_sound.openFromFile("Audio/concrete-footsteps-6752.wav");
    move_sound.setLoop(true);
    move_sound.setVolume(50.f);
    if (move_sound.getStatus() != sf::Music::Playing)
    {
        move_sound.play();
    }
}

void stop_footsteps()
{
    move_sound.stop();
}

// Hàm load texture cho HUD button
void load_hud_textures()
{
    bg.loadFromFile("UI/menu_bg.png");
    sidebar_bg.loadFromFile("UI/sidebar_bg.png");
    right_bg.loadFromFile("UI/game_bg.png");
    tex_continue.loadFromFile("UI/continue_btn.png");
    tex_back.loadFromFile("UI/back_btn.png");
    tex_title.loadFromFile("UI/game_title.png");
    tex_new.loadFromFile("UI/new_game_btn.png");
    tex_resume.loadFromFile("UI/resume_btn.png");
    tex_how.loadFromFile("UI/howto_btn.png");
    tex_game_over.loadFromFile("UI/game_over_title.png");
    tex_level_clear.loadFromFile("UI/level_cleared_title.png");
    home.loadFromFile("UI/home_btn.png");
    reset.loadFromFile("UI/reset_btn.png");
    undo.loadFromFile("UI/undo_btn.png");
}

// Hàm vẽ sprite button với scale tự động
sf::FloatRect draw_hud_sprite_button(sf::RenderWindow &window, const string &textureFile,
                                     float x, float y, bool &isHovered,
                                     float targetWidth, float targetHeight)
{
    sf::Texture hbt;
    hbt.loadFromFile(textureFile);
    // Tính scale để fit vào target size, giữ aspect ratio
    float scaleX = targetWidth / hbt.getSize().x;
    float scaleY = targetHeight / hbt.getSize().y;
    float uniformScale = min(scaleX, scaleY); // Giữ tỉ lệ, không bị méo

    // Tính kích thước thực tế sau khi scale
    float actualWidth = hbt.getSize().x * uniformScale;
    float actualHeight = hbt.getSize().y * uniformScale;

    // Tạo sprite
    sf::Sprite button(hbt);

    // Kiểm tra hover
    sf::Vector2i mouse = sf::Mouse::getPosition(window);

    // Tính bounds để kiểm tra hover (căn giữa trong target area)
    float drawX = x;
    float drawY = y;
    sf::FloatRect bounds(drawX, drawY, actualWidth, actualHeight);

    isHovered = bounds.contains(mouse.x, mouse.y);

    if (isHovered)
    {
        // Hover: phóng to 10%
        button.setScale(uniformScale * 1.1f, uniformScale * 1.1f);
        float hoverWidth = hbt.getSize().x * uniformScale * 1.1f;
        float hoverHeight = hbt.getSize().y * uniformScale * 1.1f;

        // Căn giữa với kích thước mới
        button.setPosition(x, y);
    }
    else
    {
        button.setScale(uniformScale, uniformScale);
        button.setPosition(drawX, drawY);
    }

    window.draw(button);
    return bounds;
}

// Hàm áp dụng hiệu ứng hover cho sprite button trong Main Menu
void apply_hover_effect_old(sf::RenderWindow &window, sf::Sprite &btn)
{
    sf::Vector2i mouse = sf::Mouse::getPosition(window);
    sf::FloatRect bounds = btn.getGlobalBounds();

    if (bounds.contains(mouse.x, mouse.y))
    {
        btn.setScale(1.1f, 1.1f);
        // Giữ nguyên tâm nút
        btn.setOrigin(BTN_CROP_W / 2.f, BTN_CROP_H / 2.f);
        btn.setPosition(
            bounds.left + bounds.width / 2.f,
            bounds.top + bounds.height / 2.f);
    }
    else
    {
        btn.setScale(1.f, 1.f);
        btn.setOrigin(0.f, 0.f);
    }
}

// Hàm vẽ button dạng hình chữ nhật (fallback)
void draw_button(sf::RenderWindow &window, sf::Font &font, const std::string &text,
                 float x, float y, float width, float height, sf::Color bgColor, sf::Color textColor)
{
    // Vẽ nền nút
    sf::RectangleShape button_bg(sf::Vector2f(width, height));
    button_bg.setFillColor(bgColor);
    button_bg.setPosition(x, y);
    window.draw(button_bg);

    // Vẽ chữ trên nút
    sf::Text button_text;
    button_text.setFont(font);
    button_text.setString(text);
    button_text.setCharacterSize(24);
    button_text.setFillColor(textColor);

    // Căn giữa chữ
    sf::FloatRect textRect = button_text.getLocalBounds();
    button_text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    button_text.setPosition(x + width / 2.0f, y + height / 2.0f);

    window.draw(button_text);
}

// Hàm kiểm tra click button dạng hình chữ nhật
bool is_button_clicked(const sf::Event &event, float x, float y, float width, float height)
{
    float mouseX = (float)event.mouseButton.x;
    float mouseY = (float)event.mouseButton.y;

    if (mouseX >= x && mouseX <= x + width &&
        mouseY >= y && mouseY <= y + height)
    {
        return true;
    }
    return false;
}

static bool load_frame(VideoBG &v, int idx)
{
    char buf[512];
    snprintf(buf, sizeof(buf), v.pattern.c_str(), idx);

    if (!v.tex.loadFromFile(buf))
    {
        return false;
    }
    v.tex.setSmooth(true);
    v.spr.setTexture(v.tex, true);
    return true;
}

bool video_init(VideoBG &v, const std::string &pattern, float fps)
{
    v.pattern = pattern;
    v.fps = fps;
    v.frame = 0;
    v.clock.restart();

    v.loaded = load_frame(v, 0);
    return v.loaded;
}

void video_update(VideoBG &v, int mx)
{
    if (!v.loaded)
        return;

    float frameTime = 1.f / v.fps;
    if (v.clock.getElapsedTime().asSeconds() < frameTime)
        return;
    v.clock.restart();

    int next = v.frame + 1;
    if (next == mx)
    {
        // loop về 0
        next = 0;
        if (!load_frame(v, next))
        {
            v.loaded = false;
            return;
        }
    }
    else
        load_frame(v, next);
    v.frame = next;
}

void video_draw(VideoBG &v, sf::RenderWindow &window, float startX, float startY)
{
    if (!v.loaded)
        return;
    v.spr.setPosition(startX, startY);

    window.draw(v.spr);
}

VideoBG menu_video, game_bg_video;
bool menu_video_loaded = false;
bool game_bg_video_loaded = false;

void draw_hud(sf::RenderWindow &window, int moves, int goblin_alive, int slime_alive)
{
    stop_menu_music();
    sf::Font font;
    if (!font.loadFromFile("_bitmap_font____romulus_by_pix3m-d6aokem.ttf"))
    {
        cerr << "Error loading font" << endl;
        return;
    }

    // Vẽ nền sidebar
    sf::Sprite sidebar_bg_sprite(sidebar_bg);
    sidebar_bg_sprite.setScale(SIDEBAR_WIDTH / float(sidebar_bg.getSize().x),
                               WINDOW_HEIGHT / float(sidebar_bg.getSize().y));
    sidebar_bg_sprite.setPosition(HUD_START_X, 0.f);
    window.draw(sidebar_bg_sprite);

    if (!game_bg_video_loaded)
    {
        game_bg_video_loaded = video_init(game_bg_video, "Media/gameFrames/frame_%04d.jpg", 30.f);
        if (!game_bg_video_loaded)
        {
            std::cout << "FAILED to load menu video frames! Check path.\n";
        }
    }

    video_update(game_bg_video, 122);
    video_draw(game_bg_video, window, 500.f, 0.f);

    sf::Text hud_text;
    hud_text.setFont(font);
    hud_text.setCharacterSize(40);
    hud_text.setFillColor(sf::Color::White);

    // Tựa đề
    sf::Sprite t(tex_title);
    t.setScale(0.5f, 0.5f);
    float START_X = (SIDEBAR_WIDTH / 2.f) - (t.getGlobalBounds().width / 2.f);
    float START_Y = 20.f;
    const float LINE_SPACING = 40.f;
    START_Y += t.getGlobalBounds().height;
    // 1. Lượt đi
    hud_text.setString("Moves: " + std::to_string(moves));
    hud_text.setPosition(SIDEBAR_WIDTH / 6.f, START_Y);
    window.draw(hud_text);

    sf::Text goblin_left("Goblins left: " + to_string(goblin_alive), font, 40);
    goblin_left.setFillColor(sf::Color::Green);
    goblin_left.setPosition(SIDEBAR_WIDTH / 6.f, START_Y + LINE_SPACING);
    window.draw(goblin_left);

    sf::Text slime_left("Slimes left: " + to_string(slime_alive), font, 40);
    slime_left.setFillColor(sf::Color::Magenta);
    slime_left.setPosition(SIDEBAR_WIDTH / 6.f, START_Y + LINE_SPACING * 2);
    window.draw(slime_left);

    // Hướng dẫn
    sf::Text guide_title("Controls:", font, 40);
    guide_title.setFillColor(sf::Color::Cyan);
    guide_title.setPosition(SIDEBAR_WIDTH / 6.f, START_Y + LINE_SPACING * 4);
    window.draw(guide_title);

    sf::Text guide_w("W, A, S, D or Arrow Keys: Move", font, 30);
    guide_w.setFillColor(sf::Color::White);
    guide_w.setPosition(SIDEBAR_WIDTH / 6.f, START_Y + LINE_SPACING * 6 + 10);
    window.draw(guide_w);

    sf::Text guide_m("M: Reset Level", font, 30);
    guide_m.setFillColor(sf::Color::White);
    guide_m.setPosition(SIDEBAR_WIDTH / 6.f, START_Y + LINE_SPACING * 7 + 10);
    window.draw(guide_m);

    sf::Text guide_r("R: Undo Move", font, 30);
    guide_r.setFillColor(sf::Color::White);
    guide_r.setPosition(SIDEBAR_WIDTH / 6.f, START_Y + LINE_SPACING * 8 + 10);
    window.draw(guide_r);

    bool dummyHover = false;

    // Kiểm tra xem có file ảnh không
    bool hasHome = true, hasUndo = true, hasReset = true;

    // Nút HOME (back to menu)
    HUD_HOME_BOUNDS = draw_hud_sprite_button(window, "UI/home_btn.png",
                                             (SIDEBAR_WIDTH / 6.f), START_Y + LINE_SPACING * 9 + 10,
                                             dummyHover, HUD_BUTTON_TARGET_WIDTH, HUD_BUTTON_TARGET_HEIGHT);

    // Nút UNDO (R)
    HUD_UNDO_BOUNDS = draw_hud_sprite_button(window, "UI/undo_btn.png",
                                             (SIDEBAR_WIDTH / 6.f) + 100, START_Y + LINE_SPACING * 9 + 10,
                                             dummyHover, HUD_BUTTON_TARGET_WIDTH, HUD_BUTTON_TARGET_HEIGHT);

    // Nút RESET (M)
    HUD_RESET_BOUNDS = draw_hud_sprite_button(window, "UI/reset_btn.png",
                                              (SIDEBAR_WIDTH / 6.f) + 200, START_Y + LINE_SPACING * 9 + 10,
                                              dummyHover, HUD_BUTTON_TARGET_WIDTH, HUD_BUTTON_TARGET_HEIGHT);
}

// Main menu
void draw_main_menu(sf::RenderWindow &window)
{
    if (!menu_video_loaded)
    {
        menu_video_loaded = video_init(menu_video, "Media/menuFrames/frame_%04d.jpg", 30.f);
        if (!menu_video_loaded)
        {
            std::cout << "FAILED to load menu video frames! Check path.\n";
        }
    }

    video_update(menu_video, 195);
    video_draw(menu_video, window, 0.f, 0.f);

    sf::Sprite t(tex_title);
    t.setScale(TITLE_IMAGE_SCALE, TITLE_IMAGE_SCALE);
    t.setOrigin(tex_title.getSize().x / 2.f, tex_title.getSize().y / 2.f);
    t.setPosition(NEW_CENTER_X, TITLE_IMAGE_Y);
    window.draw(t);

    sf::Sprite btnNew(tex_new, BUTTON_RECT);
    sf::Sprite btnResume(tex_resume, BUTTON_RECT);
    sf::Sprite btnHow(tex_how, BUTTON_RECT);

    // Set positions
    btnNew.setPosition(BUTTON_X_CENTERED, MAIN_MENU_NEW_GAME_Y);
    btnResume.setPosition(BUTTON_X_CENTERED, MAIN_MENU_NEW_GAME_Y + MAIN_MENU_BUTTON_SPACING);
    btnHow.setPosition(BUTTON_X_CENTERED, MAIN_MENU_NEW_GAME_Y + MAIN_MENU_BUTTON_SPACING * 2);

    // Hover effect
    apply_hover_effect_old(window, btnNew);
    apply_hover_effect_old(window, btnResume);
    apply_hover_effect_old(window, btnHow);

    // Draw buttons
    window.draw(btnNew);
    window.draw(btnResume);
    window.draw(btnHow);
}

int handle_main_menu_input(sf::RenderWindow &window)
{
    // Create cropped sprites
    sf::Sprite btnNew(tex_new, BUTTON_RECT);
    sf::Sprite btnResume(tex_resume, BUTTON_RECT);
    sf::Sprite btnHow(tex_how, BUTTON_RECT);

    // Set positions
    btnNew.setPosition(BUTTON_X_CENTERED, MAIN_MENU_NEW_GAME_Y);
    btnResume.setPosition(BUTTON_X_CENTERED, MAIN_MENU_NEW_GAME_Y + MAIN_MENU_BUTTON_SPACING);
    btnHow.setPosition(BUTTON_X_CENTERED, MAIN_MENU_NEW_GAME_Y + MAIN_MENU_BUTTON_SPACING * 2);

    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
            return 0;
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {
            float mx = event.mouseButton.x;
            float my = event.mouseButton.y;

            auto clicked = [&](sf::Sprite &btn)
            {
                return btn.getGlobalBounds().contains(mx, my);
            };

            if (clicked(btnNew))
                return 1;
            if (clicked(btnResume))
                return 2;
            if (clicked(btnHow))
                return 3;
        }
    }

    return 0;
}

void draw_how_to_play(sf::RenderWindow &window)
{
    video_update(menu_video, 195);
    video_draw(menu_video, window, 0.f, 0.f);
    sf::Font font;
    if (!font.loadFromFile("_bitmap_font____romulus_by_pix3m-d6aokem.ttf"))
        return;

    sf::Text title("HOW TO PLAY", font, 50);
    title.setFillColor(sf::Color::Yellow);

    sf::FloatRect textRect = title.getLocalBounds();
    title.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    title.setPosition(NEW_CENTER_X, 100.f);
    window.draw(title);

    sf::Text instruction;
    instruction.setFont(font);
    instruction.setCharacterSize(40);
    instruction.setFillColor(sf::Color::White);

    std::string help_text =
        "Goal: Escape the maze by reaching the Green square (Exit).\n\n"
        "Player: Moves 1 space per turn.\n"
        "Goblin: Moves 2 spaces per turn, following the shortest path.\n"
        "Slime: Moves 1 space per turn, following the shortest path.\n"
        "Trap: Stepping on it results in Game Over.\n"
        "(You still lost if a monster encounters you at the exit)\n\n"
        "Controls:\n"
        "W, A, S, D (remember to turn off Telex) or Arrow Keys: Move\n"
        "R: Undo last move\n"
        "M: Reset level\n\n"
        "Be strategic! Every turn matters.";

    instruction.setString(help_text);
    sf::FloatRect bounds = instruction.getLocalBounds();
    instruction.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    instruction.setPosition(NEW_CENTER_X, 450.f);
    window.draw(instruction);

    sf::Sprite btnBack(tex_back, BUTTON_RECT);

    // Đặt vị trí
    btnBack.setPosition(BUTTON_X_CENTERED, HOW_TO_PLAY_BACK_BTN_Y);

    // Áp dụng Hover
    apply_hover_effect_old(window, btnBack);

    // Vẽ nút
    window.draw(btnBack);
}

bool handle_how_to_play_input(sf::RenderWindow &window)
{
    sf::Sprite btnBack(tex_back, BUTTON_RECT);
    btnBack.setPosition(BUTTON_X_CENTERED, HOW_TO_PLAY_BACK_BTN_Y);

    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
            return true;
        }
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Escape)
            {
                return true;
            }
        }
        if (event.type == sf::Event::MouseButtonPressed)
        {
            float mx = event.mouseButton.x;
            float my = event.mouseButton.y;

            auto clicked = [&](sf::Sprite &btn)
            {
                return btn.getGlobalBounds().contains(mx, my);
            };

            if (clicked(btnBack))
                return true;
        }
    }
    return false;
}

// Game over
void draw_game_over(sf::RenderWindow &window)
{
    video_update(menu_video, 195);
    video_draw(menu_video, window, 0.f, 0.f);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
        return;

    sf::Sprite titleSprite(tex_game_over);
    titleSprite.setOrigin(tex_game_over.getSize().x / 2.f, tex_game_over.getSize().y / 2.f);
    titleSprite.setPosition(NEW_CENTER_X, 200.f);
    window.draw(titleSprite);

    const float BUTTON_Y = 300.f;

    sf::Sprite btnBack(tex_back, BUTTON_RECT);

    // Đặt vị trí
    btnBack.setPosition(BUTTON_X_CENTERED, BUTTON_Y);

    // Áp dụng Hover
    apply_hover_effect_old(window, btnBack);

    // Vẽ nút
    window.draw(btnBack);
}

bool handle_game_over_input(sf::RenderWindow &window)
{
    const float BUTTON_Y = 300.f;

    sf::Sprite btnBack(tex_back, BUTTON_RECT);
    btnBack.setPosition(BUTTON_X_CENTERED, BUTTON_Y);

    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
            return true;
        }
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Enter)
            {
                return true;
            }
        }
        if (event.type == sf::Event::MouseButtonPressed)
        {
            float mx = event.mouseButton.x;
            float my = event.mouseButton.y;

            auto clicked = [&](sf::Sprite &btn)
            {
                return btn.getGlobalBounds().contains(mx, my);
            };

            if (clicked(btnBack))
                return true;
        }
    }
    return false;
}

// Game win
void draw_game_win(sf::RenderWindow &window)
{
    video_update(menu_video, 195);
    video_draw(menu_video, window, 0.f, 0.f);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
        return;

    sf::Text title("CONGRATULATIONS!", font, 48);
    title.setFillColor(sf::Color::Green);

    sf::FloatRect textRect = title.getLocalBounds();
    title.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    title.setPosition(NEW_CENTER_X, 200.f);
    window.draw(title);

    sf::Text subtitle("You have mastered the maze!", font, 24);
    subtitle.setFillColor(sf::Color::White);
    sf::FloatRect subRect = subtitle.getLocalBounds();
    subtitle.setOrigin(subRect.left + subRect.width / 2.0f, subRect.top + subRect.height / 2.0f);
    subtitle.setPosition(NEW_CENTER_X, 270.f);
    window.draw(subtitle);

    const float BUTTON_Y = 350.f;

    sf::Sprite btnBack(tex_back, BUTTON_RECT);

    // Đặt vị trí
    btnBack.setPosition(BUTTON_X_CENTERED, BUTTON_Y);

    // Áp dụng Hover
    apply_hover_effect_old(window, btnBack);

    // Vẽ nút
    window.draw(btnBack);
}

bool handle_game_win_input(sf::RenderWindow &window)
{
    const float BUTTON_Y = 350.f;

    sf::Sprite btnBack(tex_back, BUTTON_RECT);
    btnBack.setPosition(BUTTON_X_CENTERED, BUTTON_Y);

    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
            return true;
        }
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Enter)
            {
                return true;
            }
        }
        if (event.type == sf::Event::MouseButtonPressed)
        {
            float mx = event.mouseButton.x;
            float my = event.mouseButton.y;

            auto clicked = [&](sf::Sprite &btn)
            {
                return btn.getGlobalBounds().contains(mx, my);
            };

            if (clicked(btnBack))
                return true;
        }
    }
    return false;
}

// Next level
void draw_next_level_screen(sf::RenderWindow &window)
{
    video_update(menu_video, 195);
    video_draw(menu_video, window, 0.f, 0.f);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
        return;

    sf::Sprite titleSprite(tex_level_clear);
    titleSprite.setOrigin(tex_level_clear.getSize().x / 2.f, tex_level_clear.getSize().y / 2.f);
    titleSprite.setPosition(NEW_CENTER_X, 200.f);
    window.draw(titleSprite);

    const float BUTTON_Y = 300.f;

    // Nút Continue
    sf::Sprite btnContinue(tex_continue, BUTTON_RECT);

    // Đặt vị trí
    btnContinue.setPosition(BUTTON_X_CENTERED, BUTTON_Y);

    // Áp dụng Hover
    apply_hover_effect_old(window, btnContinue);

    // Vẽ nút
    window.draw(btnContinue);
}

bool handle_next_level_input(sf::RenderWindow &window)
{
    const float BUTTON_Y = 300.f;

    sf::Sprite btnContinue(tex_continue, BUTTON_RECT);
    btnContinue.setPosition(BUTTON_X_CENTERED, BUTTON_Y);

    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
            return true;
        }

        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Enter)
            {
                return true;
            }
        }
        if (event.type == sf::Event::MouseButtonPressed)
        {
            float mx = event.mouseButton.x;
            float my = event.mouseButton.y;

            auto clicked = [&](sf::Sprite &btn)
            {
                return btn.getGlobalBounds().contains(mx, my);
            };

            if (clicked(btnContinue))
                return true;
        }
    }
    return false;
}