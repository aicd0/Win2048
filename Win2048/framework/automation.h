#pragma once
#include "game.h"

#define MOVE_UP 1
#define MOVE_DOWN 2
#define MOVE_LEFT 3
#define MOVE_RIGHT 4

class Automation2048 {
public:
  Win2048 MainTable;

  int maxTestUp;
  int maxTestDown;
  int maxTestLeft;
  int maxTestRight;

  int minTestUp;
  int minTestDown;
  int minTestLeft;
  int minTestRight;

  int aveTestUp;
  int aveTestDown;
  int aveTestLeft;
  int aveTestRight;

  Automation2048() {
    reset();
  }

  void reset() {
    maxTestUp = 0;
    maxTestDown = 0;
    maxTestLeft = 0;
    maxTestRight = 0;

    minTestUp = 0;
    minTestDown = 0;
    minTestLeft = 0;
    minTestRight = 0;

    aveTestUp = 0;
    aveTestDown = 0;
    aveTestLeft = 0;
    aveTestRight = 0;

    MainTable.newGame();
    ExperimentTable.newGame();
  }

  bool moveMain(int _Move) {
    switch (_Move) {
    case MOVE_UP:
      return MainTable.up();
    case MOVE_DOWN:
      return MainTable.down();
    case MOVE_LEFT:
      return MainTable.left();
    case MOVE_RIGHT:
      return MainTable.right();
    default:
      abort();
    }
    return false;
  }

  void autoPlay() {
    reset();
    for (int i = 0; true; i++) {
      int thisMove = aveTest(100, 300);
      switch (thisMove) {
      case MOVE_UP:
        if (moveMain(MOVE_UP))
          break;
      case MOVE_DOWN:
        if (moveMain(MOVE_DOWN))
          break;
      case MOVE_LEFT:
        if (moveMain(MOVE_LEFT))
          break;
      case MOVE_RIGHT:
        if (moveMain(MOVE_RIGHT))
          break;
        if (moveMain(MOVE_UP))
          break;
        if (moveMain(MOVE_DOWN))
          break;
        if (moveMain(MOVE_LEFT))
          break;
        break;
      default:
        abort();
      }
      if (MainTable.GameOver)
        break;
    }
  }

  void singleMove() {
    if (MainTable.GameOver)
      return;
    int thisMove = aveTest(100, 300);
    switch (thisMove) {
    case MOVE_UP:
      if (moveMain(MOVE_UP))
        break;
    case MOVE_DOWN:
      if (moveMain(MOVE_DOWN))
        break;
    case MOVE_LEFT:
      if (moveMain(MOVE_LEFT))
        break;
    case MOVE_RIGHT:
      if (moveMain(MOVE_RIGHT))
        break;
      if (moveMain(MOVE_UP))
        break;
      if (moveMain(MOVE_DOWN))
        break;
      if (moveMain(MOVE_LEFT))
        break;
      break;
    default:
      abort();
    }
  }

  int maxTest(int _Times, int _MaxSteps) {
    maxTestUp = maxTest(MOVE_UP, _Times, _MaxSteps);
    maxTestDown = maxTest(MOVE_DOWN, _Times, _MaxSteps);
    maxTestLeft = maxTest(MOVE_LEFT, _Times, _MaxSteps);
    maxTestRight = maxTest(MOVE_RIGHT, _Times, _MaxSteps);
    return getMax(maxTestUp, maxTestDown, maxTestLeft, maxTestRight);
  }

  int minTest(int _Times, int _MaxSteps) {
    minTestUp = minTest(MOVE_UP, _Times, _MaxSteps);
    minTestDown = minTest(MOVE_DOWN, _Times, _MaxSteps);
    minTestLeft = minTest(MOVE_LEFT, _Times, _MaxSteps);
    minTestRight = minTest(MOVE_RIGHT, _Times, _MaxSteps);
    return getMax(minTestUp, minTestDown, minTestLeft, minTestRight);
  }

  int aveTest(int _Times, int _MaxSteps) {
    aveTestUp = aveTest(MOVE_UP, _Times, _MaxSteps);
    aveTestDown = aveTest(MOVE_DOWN, _Times, _MaxSteps);
    aveTestLeft = aveTest(MOVE_LEFT, _Times, _MaxSteps);
    aveTestRight = aveTest(MOVE_RIGHT, _Times, _MaxSteps);
    return getMax(aveTestUp, aveTestDown, aveTestLeft, aveTestRight);
  }

private:
  Win2048 ExperimentTable;

  int maxTest(int _Move, int _Times, int _MaxMoves) {
    int currentScore = MainTable.Score;
    int maxScore = MainTable.Score;
    ExperimentTable.copy(MainTable);
    if (!move(_Move))
      return 0;
    for (int i = 0; i < _Times; i++) {
      ExperimentTable.copy(MainTable);
      move(_Move);
      for (int j = 0; j < _MaxMoves; j++) {
        if (ExperimentTable.GameOver)
          break;
        moveRandomly();
      }
      if (ExperimentTable.Score > maxScore)
        maxScore = ExperimentTable.Score;
    }
    return maxScore - currentScore;
  }

  int minTest(int _Move, int _Times, int _MaxMoves) {
    int currentScore = MainTable.Score;
    int minScore = INT_MAX;
    ExperimentTable.copy(MainTable);
    if (!move(_Move))
      return 0;
    for (int i = 0; i < _Times; i++) {
      ExperimentTable.copy(MainTable);
      move(_Move);
      for (int j = 0; j < _MaxMoves; j++) {
        if (ExperimentTable.GameOver)
          break;
        moveRandomly();
      }
      if (ExperimentTable.Score < minScore)
        minScore = ExperimentTable.Score;
    }
    return minScore - currentScore;
  }

  int aveTest(int _Move, int _Times, int _MaxMoves) {
    int currentScore = MainTable.Score;
    int sum = 0;
    ExperimentTable.copy(MainTable);
    if (!move(_Move))
      return 0;
    for (int i = 0; i < _Times; i++) {
      ExperimentTable.copy(MainTable);
      move(_Move);
      for (int j = 0; j < _MaxMoves; j++) {
        if (ExperimentTable.GameOver)
          break;
        moveRandomly();
      }
      sum += ExperimentTable.Score;
    }
    return sum / _Times - currentScore;
  }

  inline void moveRandomly() {
    switch (rand() % 4) {
    case 0:
      if (move(MOVE_UP))
        break;
    case 1:
      if (move(MOVE_DOWN))
        break;
    case 2:
      if (move(MOVE_LEFT))
        break;
    case 3:
      if (move(MOVE_RIGHT))
        break;
      if (move(MOVE_UP))
        break;
      if (move(MOVE_DOWN))
        break;
      if (move(MOVE_LEFT))
        break;
    }
  }

  inline bool move(int _Move) {
    switch (_Move) {
    case MOVE_UP:
      return ExperimentTable.up();
    case MOVE_DOWN:
      return ExperimentTable.down();
    case MOVE_LEFT:
      return ExperimentTable.left();
    case MOVE_RIGHT:
      return ExperimentTable.right();
    default:
      abort();
    }
    return false;
  }

  inline int getMax(int _1, int _2, int _3, int _4) {
    return _1 > _2 ?
      (_3 > _4 ? (_1 > _3 ? 1 : 3) : (_1 > _4 ? 1 : 4)) :
      (_3 > _4 ? (_2 > _3 ? 2 : 3) : (_2 > _4 ? 2 : 4));
  }

  inline int getMin(int _1, int _2, int _3, int _4) {
    return _1 < _2 ?
      (_3 < _4 ? (_1 < _3 ? 1 : 3) : (_1 < _4 ? 1 : 4)) :
      (_3 < _4 ? (_2 < _3 ? 2 : 3) : (_2 < _4 ? 2 : 4));
  }
};