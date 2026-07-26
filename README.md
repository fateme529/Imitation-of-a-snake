# Imitation of a Snake

A Windows console memory/maze puzzle written in C++, inspired by the classic Snake game.

## How it works

The game builds a rectangular plane made of horizontal walls, each wall punctured with a
few randomly placed gaps ("holes"). It then computes a random valid path through one hole
in every wall, from the top of the plane to the bottom, and briefly animates a snake
moving through that correct path so you can memorize it.

Once the preview ends, the board resets and it's your turn: guide the snake down through
the exact same holes, row by row, using the keys below.

- **You win** when the snake reaches the final hole in the correct sequence.
- **You lose** if the snake hits a wall, or enters a hole that isn't the one from the
  memorized path.

## Controls

| Key | Move  |
|-----|-------|
| W   | Up    |
| A   | Left  |
| S   | Down  |
| D   | Right |

## Project structure

```
Snake/
  game.cpp        # entry point: game loop, input handling, win/lose logic
  GameObjects.h    # Plane, Obstacle, Snake, and Display classes
```

## Building and running

This is a Windows-only console application (it relies on `windows.h` and `conio.h` for
input and rendering). There is no project file checked in, so build it directly from the
sources:

- **Visual Studio**: create a new empty C++ console project, add `Snake/game.cpp` and
  `Snake/GameObjects.h` to it, then build and run.
- **MinGW / g++** (from a Windows terminal):
  ```bash
  g++ Snake/game.cpp -o snake.exe
  ./snake.exe
  ```

Run the resulting executable in a terminal that supports wide/UTF-16 output, since the
board is drawn with box-drawing characters.
