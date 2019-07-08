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
    clear();
    printText(1, 1, "Error: screen not large enough");
  }
  tb_set_cursor(InitCursorX, InitCursorY);
  //Screen may be smaller than map, so display as much as possible
  m_screenWidth = std::min(tb_width(), MapWidth);
  m_screenHeight = std::min(tb_height(), MapHeight);
  //Initially, screen's top left corner is (0, 0) on the map (see member init list)
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

void Display::printText(int col, int row, const std::string text)
{
  char *textChars = new char[text.length()];
  for(std::string::size_type i=0; i<text.length(); ++i)
  {
    textChars[i] = text[i];
  }
  int x = col;
  int y = row;
  for(std::string::size_type i=0; i<text.length(); ++i)
  {
    putChar(x, y, textChars[i]);
    ++x;
    if(x >= tb_width())
    {
      x = col;
      ++y;
    }
  }
  delete[] textChars;
}

int Display::getCameraCoord(int playerCoord, bool isX)
{
  int screenSize = isX ? m_screenWidth : m_screenHeight;
  int mapSize = isX ? MapWidth : MapHeight;
  if(playerCoord < screenSize / 2)
    return 0;
  else if(playerCoord >= mapSize - screenSize / 2)
    return mapSize - screenSize;
  else
    return playerCoord - screenSize / 2;
}

void Display::putMap(const LevelMap &map, const int playerX, const int playerY)
{
  //Screen may be smaller than map, so display as much as possible
  m_screenWidth = std::min(tb_width(), MapWidth);
  m_screenHeight = std::min(tb_height(), MapHeight);
  //Calculate where to start drawing from so player stays centered (if possible)
  m_cornerX = getCameraCoord(playerX, true);
  m_cornerY = getCameraCoord(playerY, false);
  for(int y=m_cornerY; y<(m_cornerY+m_screenHeight); ++y)
  {
    for(int x=m_cornerX; x<(m_cornerX+m_screenWidth); ++x)
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
