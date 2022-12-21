#pragma once

#include <utility>
#include <vector>
#include <iostream>
#include <fcntl.h>
#include <windows.h>

using namespace std;

#define DEFAULT_HOLE_SIZE 3
#define RANDOM_GEN(a, b) ((rand() % ((b) - (a))) + (a))

namespace GameObjects {
  class Obstacle {
   public:
    struct Hole {
      int x;
      int size;
    };

   public:
    int y;
    int width;
    vector<Hole> holes;

   public:
    explicit Obstacle(int y, int width) : y(y), width(width) {
    }

    Obstacle &generate() {
      Hole hole{0, DEFAULT_HOLE_SIZE};
      // generate holes randomly
      do {
        hole.x += RANDOM_GEN(2, 10);
        if (hole.x + hole.size >= width - 2)
          break;
        // add current hole to all holes
        holes.push_back(hole);
        // move to next hole
        hole.x += hole.size;
      } while (true);
      // return result
      return *this;
    }

    Obstacle &addHole(Hole hole) {
      holes.push_back(hole);
      // return result
      return *this;
    }
  };

  class Plane {
   public:
    int width;
    int height;
    vector<Obstacle> obstacles;

   public:
    explicit Plane(int width, int height) : width(width), height(height) {
    }

    Plane &generate() {
      int y = 2;
      //
      obstacles.push_back(Obstacle(0, width)
                          .addHole({width / 2, 1}));
      do {
        Obstacle obstacle(y, width);
                 obstacle.generate();
        obstacles.push_back(obstacle);
        // move to next obstacle
        y += 2;
      } while (y < height - 2);
      obstacles.push_back(Obstacle(height - 1, width)
                          .addHole({(width - DEFAULT_HOLE_SIZE) / 2, DEFAULT_HOLE_SIZE}));
      // return result
      return *this;
    }
  };

  class Snake {
   public:
    struct Pos {
      int x;
      int y;
    };

   public:
    int x;
    int y;
    //
    Plane plane;
    //
    vector<int> indices;
    vector<Pos> movements;

   public:
    Snake(int x, int y, Plane plane) : x(x), y(y), plane(std::move(plane)) {
      movements.push_back({x, y});
    }

    void findWay() {
      int lastX = 0;
      for (int i = 0; i < plane.obstacles.size(); ++i) {
        auto index = RANDOM_GEN(0, (int) plane.obstacles[i].holes.size());
        indices.push_back(index);
        // get hole
        auto &hole = plane.obstacles[i].holes[index];
        //
        int nextX = hole.x + hole.size / 2;
        if (i != 0) {
          movements.push_back({lastX, 2 * (i - 1)});
          for (int path = lastX; (lastX < nextX ? path <= nextX : path >= nextX);
                                 (lastX < nextX ? ++path        : --path))
            movements.push_back({path, 2 * i - 1});
        }
        lastX = nextX;
      }
      movements.push_back({lastX, (int) (2 * (plane.obstacles.size() - 1))});
    }

    int moveCheck(int oX, int oY) {
      int tempX = x + oX;
      int tempY = y + oY;

      bool holeFind = false;
      // check for hole index
      int index = 0;
      for (auto const& obstacle : plane.obstacles) {
        if (obstacle.y == tempY) {
          int holeIndex = 0;
          for (auto const& hole : obstacle.holes) {
            if (hole.x <= tempX and tempX < hole.x + hole.size) {
              holeFind = true;
              // check if current hole index is same as answer index
              if (indices[index] == holeIndex)
                return index;
            }
            // move to next hole
            ++holeIndex;
          }
        }
        // move to next obstacle
        ++index;
      }
      // return result
      if (holeFind)
        return -1; // -1 means snake isn't in correct hole
      return -2;   // -2 means snake isn't in hole
    }

    bool move(int oX, int oY, bool canGo) {
      x += oX;
      y += oY;
      if (not movements.empty()) {
        auto &lastMove = movements[movements.size() - 2];
        if (x == lastMove.x and y == lastMove.y) {
          movements.pop_back();
          return true;
        }
      }
      if (canGo)
        movements.push_back({x, y});
      else {
        x -= oX;
        y -= oY;
      }
      return canGo;
    }

    void clearMovements() {
      movements.erase(movements.begin() + 1, movements.end());
    }
  };

  class Display {
   public:
    Plane &plane;
    Snake &snake;
    //
    vector<wstring> buffer;

   public:
    Display(Plane &plane, Snake &snake) : plane(plane), snake(snake) {
      // create buffer lines
      buffer.resize(plane.height + 2);
    }

    void renderObstacles() {
      clearBuffer();
      for (auto const& obstacle : plane.obstacles) {
        int x = 0;
        auto &line = buffer[obstacle.y + 1];
        //
        line.clear();
        for (auto hole : obstacle.holes) {
          line += repeatText(L"═", hole.x - x) + wstring(hole.size, ' ');
          // move to next hole
          x = hole.x + hole.size;
        }
        line += repeatText(L"═", plane.width - x);
      }
      // draw borders
      buffer[0]                = L"┌" + repeatText(L"─", plane.width) + L"┐";
      buffer[plane.height + 1] = L"└" + repeatText(L"─", plane.width) + L"┘";
      for (int i = 1 ; i < plane.height + 1; ++i) {
        auto &line = buffer[i];
        if (line.empty()) {
          line += L"│" + wstring(plane.width, ' ') + L"│";
        } else {
          line.append(L"╡");
          line.insert(0, L"╞");
        }
      }
    };

    void renderSnake(int sleep = 0) {
      renderObstacles();
      for (auto const& pos : snake.movements) {
        buffer[pos.y + 1][pos.x + 1] = L'#';
        // delay after print rendered result if needed
        if (sleep) {
          printAll();
          Sleep(sleep);
        }
      }
      // print rendered result
      printAll();
    }

    void printAll(int delay = 0, bool clear = true) const {
      // clear if is needed
      if (clear)
        clearAll();
      // print buffer lines
      for (auto const& line : buffer)
        wcout << line << endl;
      // delay if is needed
      if (delay)
        Sleep(delay);
    }

    static void printMessage(wstring const& text, int width, int sleep) {
      int length = (int) text.length();
      int padding = (width - length) / 2;
      // show message
      wcout << L"┌" + repeatText(L"─", width) + L"┐" << endl
            << L"│" + repeatText(L" ", padding) + text + repeatText(L" ", padding + (length + 1) % 2)
             + L"│" << endl
            << L"└" + repeatText(L"─", width) + L"┘" << endl;
      // sleep if needed
      Sleep(sleep);
    }

    void clearBuffer() {
      for (auto &line : buffer)
        line = L"";
    }

    static void clearAll() {
      system("cls");
    }

    bool isEmpty(int x, int y) const {
      return buffer[y][x] == ' ';
    }

   private:
    static wstring repeatText(wstring const& text, int n) {
      wstring result;
      for (int i = 0; i < n; ++i)
        result += text;
      // return repeated text
      return result;
    }
  };
}