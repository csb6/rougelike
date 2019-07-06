#include <iostream>
#include "include/display.h"

//  Map should take up at least min screen space so min-size screen is always full
static_assert(MapWidth >= MinDisplayWidth && MapHeight >= MinDisplayHeight, "Map too small");

//Display::Display() : m_cursorX(InitCursorX), m_cursorY(InitCursorY),
//		     m_errorStatus(tb_init()), m_event{0, 0, 0, 0, 0, 0, 0, 0}
Display::Display() : m_errorStatus(tb_init()), m_cornerX(0), m_cornerY(0),
		     m_event{0, 0, 0, 0, 0, 0, 0, 0}
{
  if(m_errorStatus < 0)
    std::cout << "Error: Couldn't start termbox; code " << m_errorStatus << "\n";
  if(!largeEnough())
  {
    printText(1, 1, "Error: screen not large enough");
  }
  tb_set_cursor(InitCursorX, InitCursorY);
  //Screen may be smaller than map, so display as much as possible
  m_screenWidth = std::min(tb_width(), MapWidth);
  m_screenHeight = std::min(tb_height(), MapHeight);
  //Initially, screen's top left corner is (0, 0) on the map (see member init list)
  //Show buffer onscreen
  present();
}

Display::~Display()
{
  //Makes terminal usable after program ends
  tb_shutdown();
}

int Display::convertCoord(int coord, bool isX)
{
  //Converts coord (x- or y-value) from coord on game map
  //to coord onscreen
  return coord - (isX ? m_cornerX : m_cornerY);
}

bool Display::processInput()
{
  return tb_peek_event(&m_event, InputTimeout) > -1;
}

Edge Display::atEdge(int x, int y)
{
  //"Edge" means edge of part of map displayed onscreen
  int row = convertCoord(y, false);
  int col = convertCoord(x, true);
  if(col == (m_screenWidth-1))
    return Edge::RIGHT;
  else if(x == m_cornerX)
    return Edge::TOP;
  else if(row == (m_screenHeight-1))
    return Edge::BOTTOM;
  else if(y == m_cornerY)
    return Edge::LEFT;
  return Edge::NONE;
  //return col == (m_screenWidth-1) || x == m_cornerX || row == (m_screenHeight-1)
  //  || y == m_cornerY;
}

bool Display::isEmpty(int x, int y)
{
  int row = convertCoord(y, false);
  int col = convertCoord(x, true);
  tb_cell *buffer = tb_cell_buffer();
  return buffer[(tb_width() * row) + col].ch == EmptySpace;
}

void Display::clearChar(int x, int y)
{
  int row = convertCoord(y, false);
  int col = convertCoord(x, true);
  tb_change_cell(col, row, EmptySpace, TB_WHITE, TB_BLACK);
}

void Display::putChar(int x, int y, const char letter,
		      const uint16_t fg, const uint16_t bg)
{
  int row = convertCoord(y, false);
  int col = convertCoord(x, true);
  tb_change_cell(col, row, static_cast<uint32_t>(letter), fg, bg);
}

void Display::printText(int startX, int startY, const std::string text)
{
  char *textChars = new char[text.length()];
  for(std::string::size_type i=0; i<text.length(); ++i)
  {
    textChars[i] = static_cast<char>(text[i]);
  }
  int x = startX;
  int y = startY;
  for(std::string::size_type i=0; i<text.length(); ++i)
  {
    putChar(x, y, textChars[i]);
    ++x;
    if(x >= tb_width())
    {
      x = startX;
      ++y;
    }
  }
  delete[] textChars;
}

void Display::putMap(const LevelMap &map, int newCornerX, int newCornerY)
{
  //Screen may be smaller than map, so display as much as possible
  m_screenWidth = std::min(tb_width(), MapWidth);
  m_screenHeight = std::min(tb_height(), MapHeight);
  m_cornerX = newCornerX;
  m_cornerY = newCornerY;
  //Start placing map cells starting from newly chosen corner
  for(int y=newCornerY; y<m_screenWidth; ++y)
  {
    for(int x=newCornerX; x<m_screenHeight; ++x)
    {
      if(map[y][x])
	putChar(x, y, map[y][x]);
    }
  }
}

bool Display::largeEnough()
{
  return tb_width() >= MinDisplayWidth && tb_height() >= MinDisplayHeight;
}
