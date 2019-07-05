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

class Display
//Purpose: Puts/manages content onscreen using termbox library
{
private:
  int m_cursorX, m_cursorY, m_errorStatus, m_screenWidth, m_screenHeight;
  //The latest termbox input event
  tb_event m_event;
public:
  Display();
  ~Display();
  bool processInput();
  bool isEmpty(const int x, const int y);
  void clearChar(const int x, const int y);
  void putChar(const int x, const int y, const char letter,
	       const uint16_t fg = TB_WHITE, const uint16_t bg = TB_BLACK);
  void printText(const int startX, const int startY, const std::string text);
  void putMap(const LevelMap &map);
  //Setters/Getters
  void clear() { tb_clear(); }
  void present() { tb_present(); }
  bool largeEnough();
  int getErrorStatus() { return m_errorStatus; };
  int getEventType() { return m_event.type; };
  int getEventKey() { return m_event.key; }
};
#endif
