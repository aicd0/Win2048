#pragma once
#include <stdlib.h>
#include <time.h>
#include "easing_animation.h"

#define GAME_TABLE_WIDTH 4
#define GAME_TABLE_HEIGHT 4

class Block {
public:
  int Num;

  //  painting
  bool BlockMoved = false;
  EasingAnimation PositionX;
  EasingAnimation PositionY;
  EasingAnimation Zoom;

  Block() {
    Num = rand() % 10 ? 2 : 4;
    Zoom.startEasing(0.0, 1.0, EaseOutQuad, 0.3, 0.3);
  }

  void multiply() {
    Num *= 2;
    Zoom.startEasing(1.2, EaseOutQuad, 0.2, 0.0);
    Zoom.startEasing(1.0, EaseOutQuad, 0.2, 0.2);
  }

  Block& operator=(int _Num) {
    Num = _Num;
    return *this;
  }

  bool operator==(int _Num) {
    return Num == _Num;
  }

  bool operator==(Block& _Block) {
    return Num == _Block.Num;
  }

  bool operator!=(int _Num) {
    return Num != _Num;
  }

  bool operator!=(Block& _Block) {
    return Num != _Block.Num;
  }
};

class Win2048 {
public:
  Block* GameTable[GAME_TABLE_WIDTH][GAME_TABLE_HEIGHT];

  int Score = 0;
  bool GameOver = false;

  Win2048() {
    for (int x = 0; x < GAME_TABLE_HEIGHT; x++) {
      for (int y = 0; y < GAME_TABLE_HEIGHT; y++) {
        GameTable[x][y] = nullptr;
      }
    }
    srand((unsigned int)time(NULL));
    newGame();
  }

  ~Win2048() {
    clearTable();
  }

  void copy(Win2048& src) {
    clearTable();
    Score = src.Score;
    GameOver = src.GameOver;
    for (int x = 0; x < GAME_TABLE_HEIGHT; x++) {
      for (int y = 0; y < GAME_TABLE_HEIGHT; y++) {
        if (src.GameTable[x][y])
        {
          GameTable[x][y] = new Block;
          GameTable[x][y]->Num = src.GameTable[x][y]->Num;
        }
      }
    }
  }

  void newGame() {
    Score = 0;
    GameOver = false;
    clearTable();
    generateNewBlock();
  }

  bool up() {
    if (GameOver)
      return false;
    bool blockMoved = false;
    for (int x = 0; x < GAME_TABLE_WIDTH; x++) {
      int pointer = 0;
      Block* lastBlock = nullptr;
      bool blankInserted = false;
      for (int y = 0; y < GAME_TABLE_HEIGHT; y++) {
        if (!GameTable[x][y]) {
          blankInserted = true;
          continue;
        }
        if (blankInserted)
          blockMoved = true;
        if (!lastBlock)
          lastBlock = GameTable[x][y];
        else if (*GameTable[x][y] != *lastBlock) {
          GameTable[x][pointer++] = lastBlock;
          lastBlock->BlockMoved = true;
          lastBlock = GameTable[x][y];
        }
        else {
          deleteBlock(GameTable[x][y]);
          lastBlock->multiply();
          GameTable[x][pointer++] = lastBlock;
          lastBlock->BlockMoved = true;
          Score += lastBlock->Num;
          lastBlock = nullptr;
          blockMoved = true;
        }
      }
      if (lastBlock) {
        GameTable[x][pointer++] = lastBlock;
        lastBlock->BlockMoved = true;
      }
      for (; pointer < GAME_TABLE_HEIGHT; pointer++)
        GameTable[x][pointer] = nullptr;
    }
    if (blockMoved)
      generateNewBlock();
    return blockMoved;
  }

  bool down() {
    if (GameOver)
      return false;
    bool blockMoved = false;
    for (int x = 0; x < GAME_TABLE_WIDTH; x++) {
      int pointer = GAME_TABLE_HEIGHT - 1;
      Block* lastBlock = nullptr;
      bool blankInserted = false;
      for (int y = GAME_TABLE_HEIGHT - 1; y >= 0; y--) {
        if (!GameTable[x][y]) {
          blankInserted = true;
          continue;
        }
        if (blankInserted)
          blockMoved = true;
        if (!lastBlock)
          lastBlock = GameTable[x][y];
        else if (*GameTable[x][y] != *lastBlock) {
          GameTable[x][pointer--] = lastBlock;
          lastBlock->BlockMoved = true;
          lastBlock = GameTable[x][y];
        }
        else {
          deleteBlock(GameTable[x][y]);
          lastBlock->multiply();
          GameTable[x][pointer--] = lastBlock;
          lastBlock->BlockMoved = true;
          Score += lastBlock->Num;
          lastBlock = nullptr;
          blockMoved = true;
        }
      }
      if (lastBlock) {
        GameTable[x][pointer--] = lastBlock;
        lastBlock->BlockMoved = true;
      }
      for (; pointer >= 0; pointer--)
        GameTable[x][pointer] = nullptr;
    }
    if (blockMoved)
      generateNewBlock();
    return blockMoved;
  }

  bool left() {
    if (GameOver)
      return false;
    bool blockMoved = false;
    for (int y = 0; y < GAME_TABLE_HEIGHT; y++) {
      int pointer = 0;
      Block* lastBlock = nullptr;
      bool blankInserted = false;
      for (int x = 0; x < GAME_TABLE_WIDTH; x++) {
        if (!GameTable[x][y]) {
          blankInserted = true;
          continue;
        }
        if (blankInserted)
          blockMoved = true;
        if (!lastBlock)
          lastBlock = GameTable[x][y];
        else if (*GameTable[x][y] != *lastBlock) {
          GameTable[pointer++][y] = lastBlock;
          lastBlock->BlockMoved = true;
          lastBlock = GameTable[x][y];
        }
        else {
          deleteBlock(GameTable[x][y]);
          lastBlock->multiply();
          GameTable[pointer++][y] = lastBlock;
          lastBlock->BlockMoved = true;
          Score += lastBlock->Num;
          lastBlock = nullptr;
          blockMoved = true;
        }
      }
      if (lastBlock) {
        GameTable[pointer++][y] = lastBlock;
        lastBlock->BlockMoved = true;
      }
      for (; pointer < GAME_TABLE_WIDTH; pointer++)
        GameTable[pointer][y] = nullptr;
    }
    if (blockMoved)
      generateNewBlock();
    return blockMoved;
  }

  bool right() {
    if (GameOver)
      return false;
    bool blockMoved = false;
    for (int y = 0; y < GAME_TABLE_HEIGHT; y++) {
      int pointer = GAME_TABLE_WIDTH - 1;
      Block* lastBlock = nullptr;
      bool blankInserted = false;
      for (int x = GAME_TABLE_WIDTH - 1; x >= 0; x--) {
        if (!GameTable[x][y]) {
          blankInserted = true;
          continue;
        }
        if (blankInserted)
          blockMoved = true;
        if (!lastBlock)
          lastBlock = GameTable[x][y];
        else if (*GameTable[x][y] != *lastBlock) {
          GameTable[pointer--][y] = lastBlock;
          lastBlock->BlockMoved = true;
          lastBlock = GameTable[x][y];
        }
        else {
          deleteBlock(GameTable[x][y]);
          lastBlock->multiply();
          GameTable[pointer--][y] = lastBlock;
          lastBlock->BlockMoved = true;
          Score += lastBlock->Num;
          lastBlock = nullptr;
          blockMoved = true;
        }
      }
      if (lastBlock) {
        GameTable[pointer--][y] = lastBlock;
        lastBlock->BlockMoved = true;
      }
      for (; pointer >= 0; pointer--)
        GameTable[pointer][y] = nullptr;
    }
    if (blockMoved)
      generateNewBlock();
    return blockMoved;
  }

private:
  void generateNewBlock() {
    int max = -1;
    int max_x = 0;
    int max_y = 0;
    for (int x = 0; x < GAME_TABLE_WIDTH; x++) {
      for (int y = 0; y < GAME_TABLE_HEIGHT; y++) {
        if (!GameTable[x][y]) {
          int sum = 0;
          int count = 0;
          int pt[4][2] = { {x - 1, y}, {x + 1, y}, {x, y - 1}, {x, y + 1} };
          for (int i = 0; i < 4; i++) {
            if (pt[i][0] >= 0 && pt[i][0] < GAME_TABLE_WIDTH && pt[i][1] >= 0 && pt[i][1] < GAME_TABLE_HEIGHT) {
              if (GameTable[pt[i][0]][pt[i][1]]) {
                sum += GameTable[pt[i][0]][pt[i][1]]->Num;
              }
              count++;
            }
          }
          int ave = sum / count;
          if (ave > max) {
            max = ave;
            max_x = x;
            max_y = y;
          }
        }
      }
    }
    if (max >= 0) {
      GameTable[max_x][max_y] = new Block;
    }
  }

  void deleteBlock(Block*& _Block) {
    if (_Block) {
      delete _Block;
      _Block = nullptr;
    }
  }

  void clearTable() {
    for (int x = 0; x < GAME_TABLE_HEIGHT; x++) {
      for (int y = 0; y < GAME_TABLE_HEIGHT; y++) {
        deleteBlock(GameTable[x][y]);
      }
    }
  }

  void checkGameOver() {
    GameOver = false;
    for (int x = 0; x < GAME_TABLE_WIDTH; x++) {
      Block* lastBlock = nullptr;
      for (int y = 0; y < GAME_TABLE_HEIGHT; y++) {
        if (!GameTable[x][y])
          return;
        if (!lastBlock || *lastBlock != *GameTable[x][y])
          lastBlock = GameTable[x][y];
        else return;
      }
    }
    for (int y = 0; y < GAME_TABLE_HEIGHT; y++) {
      Block* lastBlock = nullptr;
      for (int x = 0; x < GAME_TABLE_WIDTH; x++) {
        if (!lastBlock || *lastBlock != *GameTable[x][y])
          lastBlock = GameTable[x][y];
        else return;
      }
    }
    GameOver = true;
  }
};