# PROJECT: MAZE OF MALICE

## 1. INTRODUCTION
---------------------------------------------------------------------------------
Maze of Malice is a 2D turn-based strategic puzzle game. The player must 
navigate through dangerous mazes to reach the exit gate while avoiding 
intelligent enemies (Goblins, Slimes) and deadly traps. 

The game features:
- Strategic turn-based movement.
- Intelligent enemy AI using Manhattan distance logic.
- Have "Undo" and "Reset" system to correct tactical mistakes.
- Progressive difficulty through multiple levels.


## 2. PROJECT STRUCTURE
---------------------------------------------------------------------------------
To ensure the game runs correctly, the following folder structure is maintained:

[Source code]
│
├── readme.txt
├── progress.txt
├── _bitmap_font____romulus_by_pix3m-d6aokem.ttf
│
├──main.exe
│
├── main.cpp
├──logic.cpp 
├──levels.cpp 
├──menu.cpp 
├──visualise.cpp
├──logic.hpp 
├──levels.hpp 
├──menu.hpp
├──visualise.hpp 
├──frontend.hpp
├──main.hpp
│
│
├── /SFML
├── /Media                  (Sprites for characters, tiles, and menu frames)
├── /UI                     (Textures for buttons and overlays)
└── /Audio                  (Music and sound effect files)


## 3. COMPILATION AND INSTALLATION
---------------------------------------------------------------------------------
Prerequisites: SFML Library (v2.5+) and a C++17 compiler.

* **For Command Line**:
  g++ main.cpp frontend.cpp levels.cpp logic.cpp visualise.cpp menu.cpp -I SFML/include -L SFML/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -o main.exe

* **For IDE (Visual Studio)**:
  1. Create a new project and add all files from the /source folder.
  2. Configure SFML Include and Library paths in Project Properties.
  3. Ensure the /Media, /UI, /Audio folders and .ttf file are in the same 
     folder as the generated .exe file.


## 4. USER GUIDE (HOW TO PLAY)
---------------------------------------------------------------------------------
* **Movement**: Use W, A, S, D or Arrow Keys to move one tile per turn.
* **Undo**: Press 'R' to reverse your last move or the "Undo" button.
* **Reset**: Press 'M' to restart the current level or the "Reset" button.
* **Exit game**: 'Q' to exit the game immediately
* **Menu Navigation**: Use Mouse Click or Enter to select options.

* **Entities**:
  - **Player**: Your character.
  - **Goblin**: Aggressive, moves 2 tiles per turn towards you.
  - **Slime**: Slow, moves 1 tile per turn towards you.
  - **Trap**: Instant Game Over if stepped on.
  - **Stair**: Your destination to clear the level.


## 5. KNOWN ISSUES
---------------------------------------------------------------------------------
* Collision does not have proper effects.
* Only 4 levels, no random generation.


## 6. FUTURE IDEAS
---------------------------------------------------------------------------------
* Add boss monster (Minotaur).
* Add shop system/power-up items.
* Add more suitable effects.
* Add sign in/sign-up option.
* Add difficulty level (different enemy AI).
* Procedural maze generation.


## 7. CREDITS
---------------------------------------------------------------------------------
Developed by (Name + Student ID):   
* [Dương Trung Anh - 25127012 - 25C11 - University of Science, VNU-HCM]
* [Hà Trần Bội Anh - 25127013 - 25C11 - University of Science, VNU-HCM]
* [Nguyễn Tuấn Nghĩa - 25127101 - 25C11 - University of Science, VNU-HCM]
* [Nguyễn Nhật Quỳnh - 25127131 - 25C11 - University of Science, VNU-HCM]
* [Trần Ngọc Nghĩa - 25127431 - 25C11 - University of Science, VNU-HCM]
Library:      SFML (Simple and Fast Multimedia Library)
