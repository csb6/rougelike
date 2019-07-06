#ifndef DISPLAY_TERMBOX_H
#define DISPLAY_TERMBOX_H
#include <string>
#include "termbox.h"

//Display Constants
const int MinDisplayWidth = 10;
const int MinDisplayHeight = 10;
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

enum class Edge
{
 TOP,
 BOTTOM,
 LEFT,
 RIGHT,
 NONE
};

class Display
//Purpose: Puts/manages content onscreen using termbox library
{
private:
  //int m_cursorX, m_cursorY;
  int m_errorStatus, m_screenWidth, m_screenHeight, m_cornerX, m_cornerY;
  //The latest termbox input event
  tb_event m_event;
public:
  Display();
  ~Display();
  inline int convertCoord(int coord, bool isX);
  bool processInput();
  //x and y in terms of game map, not display
  Edge atEdge(int x, int y);
  bool isEmpty(int x, int y);
  void clearChar(int x, int y);
  void putChar(int x, int y, char letter,
	       const uint16_t fg = TB_WHITE, const uint16_t bg = TB_BLACK);
  //startX & startY are in terms of display, not the game map
  void printText(int startX, int startY, const std::string text);
  //newCornerX and newCornerY in terms of game map, not display
  void putMap(const LevelMap &map, const int newCornerX = 0, const int newCornerY = 0);
  //Setters/Getters
  void clear() { tb_clear(); }
  void present() { tb_present(); }
  bool largeEnough();
  int getErrorStatus() { return m_errorStatus; };
  int getEventType() { return m_event.type; };
  int getEventKey() { return m_event.key; }
};
#endif
