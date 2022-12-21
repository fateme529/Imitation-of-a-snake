#include "GameObjects.h"

#include <ctime>
#include <conio.h>

#define DEFAULT_GAME_WIDTH  31 // must be odd
#define DEFAULT_GAME_HEIGHT 27 // must be odd

int main() {
  _setmode(_fileno(stdout), 0x20000);
  srand(time(nullptr));

  auto plane = GameObjects::Plane(DEFAULT_GAME_WIDTH, DEFAULT_GAME_HEIGHT);
       plane.generate();

  auto snake = GameObjects::Snake(DEFAULT_GAME_WIDTH / 2, 0, plane);
       snake.findWay();

  auto display = GameObjects::Display(plane, snake);

  // render automatic generated snake path
  display.renderObstacles();
  display.renderSnake(45);
  Sleep(2000);
  // clear snake movements
  snake.clearMovements();

  bool wins = false;

  bool hitWall = false;
  bool wrongHole = false;
  while (not wins) {
    display.renderSnake();
    char key = getch();
    // process movement
    int oX = 0, oY = 0;
    switch (std::tolower(key)) {
      case 'w': --oY; break;
      case 'd': ++oX; break;
      case 's': ++oY; break;
      case 'a': --oX; break;
      default: continue;
    }
    // calculate snake position after click
    int x = snake.x + oX;
    int y = snake.y + oY;
    // check if movement is ok
    int result = snake.moveCheck(oX, oY);
    if (result == plane.obstacles.size() - 1)
      wins = true;
    //
    if (result == -1) {
      wrongHole = true;
      break;
    }
    if (not snake.move(oX, oY, display.isEmpty(x+1, y+1))) {
      hitWall = true;
      break;
    }
  }
  // show result
  if (wins) {
    display.renderSnake();
    Sleep(1000);
    GameObjects::Display::clearAll();
    GameObjects::Display::printMessage(L"You WON the game", DEFAULT_GAME_WIDTH + 2, 5000);
  }
  GameObjects::Display::clearAll();
  if (hitWall)
    GameObjects::Display::printMessage(L"LOOSE: Hit the wall", DEFAULT_GAME_WIDTH + 2, 5000);
  else if (wrongHole)
    GameObjects::Display::printMessage(L"LOOSE: Wrong hole", DEFAULT_GAME_WIDTH + 2, 5000);
}