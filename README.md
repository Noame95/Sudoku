# Sudoku solver and game generator!
The website has been deployed on https://sudoku-solver-and-game-generator.onrender.com/

<img width="701" height="916" alt="image" src="https://github.com/user-attachments/assets/5ff96734-7068-47bd-834c-587fd90377aa" />

## About:
This is my first full stack like project, combining Python, C++, JS and also hardware controller using ESP32.
The hardware setup includes the ESP32 (can't remember the exact modle name), 4x4 keypad and a joystick!
I started this project with no idea how to write backend with my python skills (even though they are awesome), zero clue on web development, OOP on C++ and ESP32 connection to http.
In order to learn anything here, I used GPT+ as my assistance. How did I do it?
    I told him what I aimed for, he taught me and gave me tasks, one by one each time.
    I forced him to stop acting as a problem solver, but to become my own teacher. I believe it was the most efficent way for me to learn, and the BEST way to use AI in code learning.

## Features:
- Generating Sudoku games in multiple difficulties; easy, medium and hard.
- Solving Sudoku unfilled boards! Giving a solution for example.
- Allows you to play the game by connecting a joystick and a keypad to your own ESP32, after uploading my src code on it :D

## Keypad settings:
`0`–`9` | Enter a number 
`*` | Remove the selected number 
`#` | Clear the board 
`A` | Solve the current puzzle 
`B` | Generate an Easy puzzle 
`C` | Generate a Medium puzzle 
`D` | Generate a Hard puzzle 

```text
┌─────┬─────┬─────┬─────┐
│  1  │  2  │  3  │  A  │
├─────┼─────┼─────┼─────┤
│  4  │  5  │  6  │  B  │
├─────┼─────┼─────┼─────┤
│  7  │  8  │  9  │  C  │
├─────┼─────┼─────┼─────┤
│  *  │  0  │  #  │  D  │
└─────┴─────┴─────┴─────┘
```

## Architecture:
ESP32 (Hardware, C++) -> Backend (Python, FastAPI, Subprocess, Websocket) -> Sudoku Engine (C++) -> Frontend (HTML, JS, CSS)


## Structure

```text
Sudoku/
├── backend/
│   ├── bridge.py
│   └── main.py
│
├── cpp/
│   ├── Sudoku.cpp
│   ├── Sudoku.hpp
│   └── main.cpp
│
├── esp32/
│   └── src/
│       └── main.cpp
│
└── frontend/
    ├── index.html
    ├── app.js
    ├── sounds.js
    └── style.css
```

## Hardware Schematic:
Made in Kicad
<img width="1410" height="736" alt="image" src="https://github.com/user-attachments/assets/e8dbf1c8-c6b8-48ba-ab78-c5829b0e338a" />

###HM:
- The file secrets.h is excluded from the actualy project and is replaced by secrets.example.h, since my personal security data is on the original h file ;-; So change it for your own enviroment.
- The placement marked by yellow and red on the board is used for the controller navigation.
