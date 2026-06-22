# PROJECT: MAZE OF MALICE

---

## 1. INTRODUCTION

Maze of Malice is a 2D turn-based strategic puzzle game. The player must navigate through dangerous mazes to reach the exit gate while avoiding intelligent enemies (Goblins, Slimes) and deadly traps. The game uses both ASCII Graphics on the console and SFML Graphics.

The game features:

* Strategic turn-based movement.
* Intelligent enemy AI using Manhattan distance logic.
* "Undo" and "Reset" systems to correct tactical mistakes.
* Progressive difficulty through multiple levels.

---

## 2. PROJECT STRUCTURE

To ensure the game runs correctly, the following folder structure should be maintained after cloning the repository:

```text
[Project root]
│
├── README.md
├── progress.txt
├── _bitmap_font____romulus_by_pix3m-d6aokem.ttf
│
├── main.cpp
├── logic.cpp
├── levels.cpp
├── menu.cpp
├── visualise.cpp
├── frontend.cpp
│
├── logic.hpp
├── levels.hpp
├── menu.hpp
├── visualise.hpp
├── frontend.hpp
│
├── Media/        (Sprites, animation frames, tiles, and menu frames)
├── UI/           (Textures for buttons, backgrounds, and overlays)
└── Audio/        (Music and sound effect files)
```

Important notes:

* The `Media/`, `UI/`, `Audio/`, `.ttf` font file, and `progress.txt` must stay in the project root when running the game.
* The project loads assets using relative paths such as `Audio/...`, `UI/...`, and `Media/...`.
* SFML is not included in this repository. It should be installed locally on each user's computer.

---

## 3. COMPILATION AND INSTALLATION

### 3.1 Prerequisites

This project was developed with:

* C++17
* SFML Library v2.5.1
* g++.exe 12.1.0
  `(MinGW-W64 x86_64-msvcrt-posix-seh, built by Brecht Sanders)`

Before compiling the project, users need to install SFML locally.

---

### 3.2 Installing SFML Locally

1. Download **SFML 2.5.1** from the official SFML download page.

2. Choose the package that matches your compiler and system architecture.

   For this project, the recommended setup is:

   ```text
   SFML version: 2.5.1
   Architecture: 64-bit
   Compiler: MinGW / g++
   ```

   Important: SFML and the compiler should match. If the downloaded SFML package does not match your MinGW version, the project may fail to compile or run.

3. Extract SFML to a local folder outside the repository, for example:

   ```text
   C:/Libraries/SFML-2.5.1
   ```

   After extracting, the folder should look like this:

   ```text
   C:/Libraries/SFML-2.5.1
   │
   ├── bin/
   ├── include/
   ├── lib/
   └── ...
   ```

4. Do not commit the SFML folder to GitHub. Each user should install SFML locally on their own computer.

---

### 3.3 Compile with Command Line

Open a terminal in the project root folder, then run:

```bash
g++ main.cpp frontend.cpp levels.cpp logic.cpp visualise.cpp menu.cpp ^
  -std=c++17 ^
  -IC:/Libraries/SFML-2.5.1/include ^
  -LC:/Libraries/SFML-2.5.1/lib ^
  -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system ^
  -o main.exe
```

If your SFML folder is installed in a different location, replace this path:

```text
C:/Libraries/SFML-2.5.1
```

with your actual SFML folder path.

For example:

```text
D:/Libraries/SFML-2.5.1
```

or

```text
C:/Users/YourName/Libraries/SFML-2.5.1
```

---

### 3.4 Required DLL Files

Because this project links SFML dynamically, the required SFML `.dll` files must be available when running `main.exe`.

Copy the `.dll` files from:

```text
C:/Libraries/SFML-2.5.1/bin
```

to the same folder as `main.exe`.

The most important DLL files are usually:

```text
sfml-graphics-2.dll
sfml-window-2.dll
sfml-audio-2.dll
sfml-system-2.dll
openal32.dll
```

If these files are missing, Windows may show errors such as:

```text
sfml-graphics-2.dll was not found
```

or

```text
openal32.dll was not found
```

---

### 3.5 Run the Game

After compiling, run:

```bash
./main.exe
```

or on Windows Command Prompt:

```cmd
main.exe
```

Make sure you run the command from the project root folder, where these files and folders are located:

```text
Audio/
Media/
UI/
_bitmap_font____romulus_by_pix3m-d6aokem.ttf
progress.txt
```

If the game opens but some textures, music, or fonts are missing, check that the working directory is set to the project root.

---

### 3.6 For Visual Studio Users

1. Create a new empty C++ project.

2. Add all `.cpp` and `.hpp` files from this repository to the project.

3. Open **Project Properties**.

4. Set the C++ language standard to **C++17**.

5. Add the SFML include path:

   ```text
   C:/Libraries/SFML-2.5.1/include
   ```

   to:

   ```text
   C/C++ > General > Additional Include Directories
   ```

6. Add the SFML library path:

   ```text
   C:/Libraries/SFML-2.5.1/lib
   ```

   to:

   ```text
   Linker > General > Additional Library Directories
   ```

7. Add these libraries to:

   ```text
   Linker > Input > Additional Dependencies
   ```

   ```text
   sfml-graphics.lib
   sfml-window.lib
   sfml-audio.lib
   sfml-system.lib
   ```

8. Copy the required `.dll` files from:

   ```text
   C:/Libraries/SFML-2.5.1/bin
   ```

   to the folder where the generated `.exe` file is located.

9. Make sure the working directory contains:

   ```text
   Audio/
   Media/
   UI/
   _bitmap_font____romulus_by_pix3m-d6aokem.ttf
   progress.txt
   ```

   If assets are not found, set the Visual Studio working directory to the project root.

---

## 4. USER GUIDE (HOW TO PLAY)

* **Movement**: Use Arrow Keys to move one tile per turn.
* **Wait / Do not move**: Press Space.
* **Undo**: Press `R` to reverse your last move or click the "Undo" button.
* **Reset**: Press `M` to restart the current level or click the "Reset" button.
* **Exit game**: Press `Q` to exit the game immediately.
* **Menu Navigation**: Use Mouse Click or Enter to select options.

### Entities

* **Player**: Your character.
* **Goblin**: Aggressive enemy, moves 2 tiles per turn towards you.
* **Slime**: Slow enemy, moves 1 tile per turn towards you.
* **Trap**: Instant Game Over if stepped on.
* **Stair / Portal**: Your destination to clear the level.

---

## 5. COMMON RUNNING PROBLEMS

### Problem 1: SFML header file not found

Example error:

```text
fatal error: SFML/Graphics.hpp: No such file or directory
```

Reason:

* The compiler cannot find the SFML `include` folder.

Fix:

* Check that this path is correct:

```text
C:/Libraries/SFML-2.5.1/include
```

* If SFML is installed somewhere else, update the compile command or IDE settings.

---

### Problem 2: SFML library not found

Example error:

```text
cannot find -lsfml-graphics
```

Reason:

* The linker cannot find the SFML `lib` folder.

Fix:

* Check that this path is correct:

```text
C:/Libraries/SFML-2.5.1/lib
```

* If SFML is installed somewhere else, update the compile command or IDE settings.

---

### Problem 3: Missing DLL file

Example error:

```text
sfml-graphics-2.dll was not found
```

Reason:

* The SFML `.dll` files are not in the same folder as the `.exe`.

Fix:

* Copy all required `.dll` files from:

```text
C:/Libraries/SFML-2.5.1/bin
```

to the folder containing `main.exe`.

---

### Problem 4: Textures, music, or font do not load

Reason:

* The game cannot find the `Audio/`, `Media/`, `UI/`, or font files.

Fix:

* Run the game from the project root folder.
* Make sure these folders and files are next to `main.exe` or available from the current working directory:

```text
Audio/
Media/
UI/
_bitmap_font____romulus_by_pix3m-d6aokem.ttf
progress.txt
```

---

## 6. KNOWN ISSUES

* Collision does not have proper effects.
* Only 4 levels, no random generation.

---

## 7. FUTURE IDEAS

* Add boss monster (Minotaur).
* Add shop system/power-up items.
* Add more suitable effects.
* Add sign in/sign-up option.
* Add difficulty level with different enemy AI.
* Add procedural maze generation.

---

## 8. CREDITS

Developed by:
* Team 05 (Maze of Malice) - University of Science, VNU-HCM - 25C11

Library:

* SFML — Simple and Fast Multimedia Library
