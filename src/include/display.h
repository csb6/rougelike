#ifndef DISPLAY_TERMBOX_H
#define DISPLAY_TERMBOX_H
#include <string>
#include "termbox.h"

//Display Constants
const int MinDisplayWidth = 10;
const int MinDisplayHeight = 10;
//GUI dimensions subtracted from tb_width()/height() to get play area screen dimensions
const int GUIHeight = 10;
const int GUIWidth = 10;
const int InputTimeout = 10; //in milliseconds
const int InitCursorX = 1;
const int InitCursorY = 1;
const char EmptySpace = 32;

//Map Constants
//  Map should take up at least min screen space so min-size screen is always full
const int MapWidth = 30;
const int MapHeight = 30;
//  An array containing tiles to display onscreen
typedef char LevelMap[MapWidth][MapHeight];

class Actor;

class Display
//Purpose: Puts/manages content onscreen using termbox library
{
private:
  //m_screenWidth/Height are dimensions of onscreen area to contain tiles
  //int m_cursorX, m_cursorY;
  int m_errorStatus, m_screenWidth, m_screenHeight, m_cornerX, m_cornerY;
  //The latest termbox input event
  tb_event m_event;
  //boardWidth/Height are dimensions of current window bounded on sides by GUI
  int boardWidth() { return tb_width()-GUIWidth; }
  int boardHeight() { return tb_height()-GUIHeight; }
public:
  Display();
  ~Display();
  inline int convertCoord(int coord, bool isX);
  bool processInput();
  //x and y = coords in terms of game map, not display
  //col and row = coords in terms of display, not game map
  bool isEmpty(int x, int y);
  void clearChar(int x, int y);
  void putChar(int x, int y, char letter,
	       const uint16_t fg = TB_WHITE, const uint16_t bg = TB_BLACK);
  void printText(int col, int row, const std::string text);
  int getCameraCoord(int playerCoord, bool isX);
  void draw(const LevelMap &map, Actor &player, Actor &currActor);
  void drawGUI(Actor &player, Actor &currActor);
  //Setters/Getters
  void clear() { tb_clear(); }
  void present() { tb_present(); }
  bool largeEnough();
  int getErrorStatus() { return m_errorStatus; };
  int getEventType() { return m_event.type; };
  int getEventKey() { return m_event.key; }
};
#endif
