#ifndef INPUT_GAME_H
#define INPUT_GAME_H
#include "gameboard.h"
#include "display.h"

class Input {
private:
  bool &m_running;
  Display &m_screen;
  GameBoard &m_board;
public:
  Input(bool &running, Display &screen, GameBoard &board);
  bool process();
};
#endif
