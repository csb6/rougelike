#include <iostream>
#include "include/display.h"
#include "include/actor.h"

//  Map should take up at least min screen space so min-size screen is always full
static_assert(MapWidth >= MinDisplayWidth && MapHeight >= MinDisplayHeight, "Map too small");

/* Creates an object that manages access of/content in screen display, starting
   up termbox library, checking for errors/appropriate screen size*/
//Display::Display() : m_cursorX(InitCursorX), m_cursorY(InitCursorY),
//		     m_errorStatus(tb_init()), m_event{0, 0, 0, 0, 0, 0, 0, 0}
Display::Display(LevelMap &map)
  : m_map(map), m_errorStatus(tb_init()), m_cornerX(0), m_cornerY(0),
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
  m_screenWidth = std::min(boardWidth(), MapWidth);
  m_screenHeight = std::min(boardHeight(), MapHeight);
  //Initially, screen's top left corner is (0, 0) on the map (see member init list)
}

Display::~Display()
{
  //Makes terminal usable after program ends
  tb_shutdown();
}

/* Converts coordinates written in terms of game map (the 2d array maintained
   by the GameBoard) into the row/col coordinates used by termbox onscreen.
   Most member functions use it, including putChar() */
int Display::convertCoord(int coord, bool isX)
{
  //Converts coord (x- or y-value) from coord on game map
  //to coord onscreen
  return coord - (isX ? m_cornerX : m_cornerY);
}

/* Grab user input if there is any, returning false if any error while
   checking occurs*/
bool Display::getInput()
{
  return tb_peek_event(&m_event, InputTimeout) > -1;
}

/* Checks if any non-space character is currently in the screen buffer at the
   given coordinate*/
bool Display::isEmpty(int x, int y)
{
  int row = convertCoord(y, false);
  int col = convertCoord(x, true);
  tb_cell *buffer = tb_cell_buffer();
  return buffer[(tb_width() * row) + col].ch == EmptySpace;
}

/* Replaces the character at a given point with a space character */
void Display::clearChar(int x, int y)
{
  int row = convertCoord(y, false);
  int col = convertCoord(x, true);
  tb_change_cell(col, row, EmptySpace, TB_WHITE, TB_BLACK);
}

/* Places a character at a given point with foreground/background colors.
   Default fg/bg colors in header */
void Display::putChar(int x, int y, const char letter,
		      const uint16_t fg, const uint16_t bg)
{
  int row = convertCoord(y, false);
  int col = convertCoord(x, true);
  tb_change_cell(col, row, static_cast<uint32_t>(letter), fg, bg);
}

/* Writes a string onscreen, starting at the given coords (in terms of
   screen, not game map), wrapping by character at the screen edge and
   dropping to the same starting x-position on the next line  */
void Display::printText(int col, int row, const std::string text)
{
  //Validate coordinates
  if(col < 0 || col > tb_width() || row < 0 || row > tb_height())
    return;
  int x = col;
  int y = row;
  for(std::string::size_type i=0; i<text.length(); ++i)
  {
    tb_change_cell(x, y, static_cast<uint32_t>(text[i]), TB_WHITE, TB_BLACK);
    ++x;
    if(x >= tb_width())
    {
      x = col;
      ++y;
    }
  }
}

/* Gets position of top-left corner of screen so that the screen buffer
   (a slice of the game map) stays centered over the player, locking to the
   edges when the player approaches the map edge */
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

/* Adds labels/information shown to player in sidebars onscreen to screen
   buffer, respecting the area used to draw the area around the player */
void Display::drawGUI(Actor &player, Actor &currActor)
{
  printText(0, boardHeight(), currActor.getName() + "'s Turn:");
  printText(0, boardHeight()+1, " Energy: " + std::to_string(currActor.getEnergy()));
  printText(0, boardHeight()+2, "You:");
  printText(0, boardHeight()+3, " Name: " + player.getName());
  printText(0, boardHeight()+4, " Energy: " + std::to_string(player.getEnergy()));
}

/* Places all non-empty tiles centered around the player into the screen buffer,
   stopping when there is no more room. Respects area left for GUI */
void Display::draw(Actor &player, Actor &currActor)
{
  //Screen may be smaller than map, so display as much as possible
  m_screenWidth = std::min(boardWidth(), MapWidth);
  m_screenHeight = std::min(boardHeight(), MapHeight);
  //Calculate where to start drawing from so player stays centered (if possible)
  m_cornerX = getCameraCoord(player.getX(), true);
  m_cornerY = getCameraCoord(player.getY(), false);
  for(int y=m_cornerY; y<(m_cornerY+m_screenHeight); ++y)
  {
    for(int x=m_cornerX; x<(m_cornerX+m_screenWidth); ++x)
    {
      if(m_map[y][x])
	putChar(x, y, m_map[y][x]);
    }
  }
  drawGUI(player, currActor);
}

/* Checks if window is large enough to adequately display the game */
bool Display::largeEnough()
{
  return tb_width() >= MinDisplayWidth && tb_height() >= MinDisplayHeight;
}
