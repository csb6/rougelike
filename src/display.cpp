#include <iostream>
#include "include/display.h"

//  Map should take up at least min screen space so min-size screen is always full
static_assert(MapWidth >= MinDisplayWidth && MapHeight >= MinDisplayHeight, "Map too small");

Display::Display() : m_cursorX(InitCursorX), m_cursorY(InitCursorY),
		     m_errorStatus(tb_init()), m_event{0, 0, 0, 0, 0, 0, 0, 0}
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
  //Show buffer onscreen
  present();
}

Display::~Display()
{
  //Makes terminal usable after program ends
  tb_shutdown();
}

bool Display::processInput()
{
  return tb_peek_event(&m_event, InputTimeout) > -1;
}

bool Display::isEmpty(const int x, const int y)
{
  tb_cell *buffer = tb_cell_buffer();
  return buffer[(tb_width() * y) + x].ch == EmptySpace;
}

void Display::clearChar(const int x, const int y)
{
  tb_change_cell(x, y, EmptySpace, TB_WHITE, TB_BLACK);
}

void Display::putChar(const int x, const int y, const char letter,
		      const uint16_t fg, const uint16_t bg)
{
  tb_change_cell(x, y, static_cast<uint32_t>(letter), fg, bg);
}

void Display::printText(const int startX, const int startY, const std::string text)
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

void Display::putMap(const LevelMap &map)
{
  //Screen may be smaller than map, so display as much as possible
  m_screenWidth = std::min(tb_width(), MapWidth);
  m_screenHeight = std::min(tb_height(), MapHeight);
  for(int row=0; row<m_screenWidth; ++row)
  {
    for(int col=0; col<m_screenHeight; ++col)
    {
      if(map[row][col])
	putChar(col, row, map[row][col]);
    }
  }
}

bool Display::largeEnough()
{
  return tb_width() >= MinDisplayWidth && tb_height() >= MinDisplayHeight;
}
