#include <iostream>
#include "include/display.h"
#include "include/actor.h"
#include <algorithm>

//  Map should take up at least min screen space so min-size screen is always full
static_assert(MapWidth >= MinDisplayWidth && MapHeight >= MinDisplayHeight, "Map too small");
static_assert(GUIWidth <= MinDisplayWidth && GUIHeight <= MinDisplayHeight, "GUI too big");
static_assert(MaxLogSize <= MinDisplayHeight && MaxLogSize > 0, "Log too tall");

/* Creates an object that manages access of/content in screen display, starting
   up termbox library, checking for errors/appropriate screen size*/
Display::Display()
  : m_cursorX(-1), m_cursorY(-1), m_screenWidth(0), m_screenHeight(0),
    m_cornerX(0), m_cornerY(0), m_event{0, 0, 0, 0, 0, 0, 0, 0},
    m_textCol(0), m_textX(0), m_textY(0), m_textMaxWidth(0),
    m_log{}, m_logRow(0)
{
  int errorStatus = tb_init();
  if(errorStatus < 0) {
    std::cout << "Error: Couldn't start termbox; code " << errorStatus << "\n";
  }
  if(!largeEnough()) {
    clear();
    printText(1, 1, "Error: screen not large enough");
  }
  //Screen may be smaller than map, so display as much as possible
  //Initially, screen's top left corner is (0, 0) on the map (see member init list)
  m_screenWidth = std::min(boardWidth(), MapWidth);
  m_screenHeight = std::min(boardHeight(), MapHeight);
}

Display::~Display()
{
  //Makes terminal usable after program ends
  tb_shutdown();
}

/* Converts coordinates written in terms of game map (the 2d array maintained
   by the GameBoard) into the row/col coordinates used by termbox onscreen.
   Most member functions use it*/
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

void Display::moveCursor(int x, int y)
{
  m_cursorY = convertCoord(y, false);
  m_cursorX = convertCoord(x, true);
  tb_set_cursor(m_cursorX, m_cursorY);
}

void Display::translateCursor(int dx, int dy)
{
  if(m_cursorX != -1 && m_cursorY != -1) {
    m_cursorY += dy;
    m_cursorX += dx;
    tb_set_cursor(m_cursorX, m_cursorY);
  }
}

void Display::hideCursor()
{
  tb_set_cursor(TB_HIDE_CURSOR, TB_HIDE_CURSOR);
  m_cursorX = -1;
  m_cursorY = -1;
}

/* Replaces the character at a given point with a space character */
void Display::clearChar(int col, int row)
{
  putChar(col, row, EmptySpace);
}

/* Places a character at a given point with foreground/background colors.
   Default fg/bg colors in header */
void Display::putChar(int col, int row, const char letter,
		      const uint16_t fg, const uint16_t bg)
{
  tb_change_cell(col, row, static_cast<uint32_t>(letter), fg, bg);
}

/* Writes a string onscreen, starting at the given coords (in terms of
   screen, not game map), wrapping by character at the screen edge and
   dropping to the same starting x-position on the next line.
   Default fg/bg colors in header  */
void Display::printText(int col, int row, const std::string text,
			const uint16_t fg, const uint16_t bg)
{
  //Validate coordinates
  if(col < 0 || col > tb_width() || row < 0 || row > tb_height()) {
    return;
  }
  int x = col;
  int y = row;
  for(std::string::size_type i=0; i<text.length(); ++i) {
    putChar(x, y, text[i], fg, bg);
    ++x;
    if(x >= tb_width()) {
      x = col;
      ++y;
    }
  }
}

/* Prompt user to enter an integer using given message; -1 is error code,
   not exception-safe*/
int Display::input(const std::string msg, int col, int row)
{
  printText(col, row, msg);
  //Move col over so text typed out
  //printed to right of msg
  col += msg.size();

  std::string num;
  while(getInput()) {
    switch(getEventType())
    {
      //Respond to user key presses
    case TB_EVENT_KEY:
      //First, look at key combos
      switch(getEventKey())
      {
      case TB_KEY_ESC:
	//Exit early
	return -1;
      case TB_KEY_BACKSPACE:
      case TB_KEY_BACKSPACE2:
	//Only backspace if there's something to delete
	if(num.size() > 0) {
	  clearChar(--col, row);
	  num.pop_back();
	}
	break;
      case TB_KEY_ENTER:
	//Give number to caller
	try {
	  return std::stoi(num);
	}
	catch(const std::out_of_range& e) {
	  //Exit early
	  return -1;
	}
      default:
	//If not a key combo, look at individual keys
	char key = getEventChar();
	if(key >= '0' && key <= '9') {
	  num += key;
	  putChar(col++, row, key);
	}
      }
      break;
    }
    //Show any changes
    present();
  }
  return -1;
}

/* Puts text message into stored message log; useful for debugging/showing
   events as they occur*/
void Display::log(const std::string &text)
{
  if(m_logRow >= MaxLogSize) {
    std::rotate(begin(m_log), begin(m_log)+1, end(m_log));
    m_log[MaxLogSize-1] = text;
  }
  else {
    m_log[m_logRow] = text;
    ++m_logRow;
  }
}

/*Prints given text into abstract columns, where each column is
  the width of its widest element; once a higher column is specified,
  the smaller columns cannot be altered/added to. Default fg/bg colors in header*/
void Display::printTextCol(int gridCol, const std::string text,
			   const uint16_t fg, const uint16_t bg)
{
  if(gridCol < m_textCol) {
    return;
  }
  else if(gridCol == m_textCol) {
    printText(m_textX, boardHeight()+m_textY, text, fg, bg);
    if(static_cast<int>(text.size()) > m_textMaxWidth) {
      m_textMaxWidth = text.size();
    }
    ++m_textY;
  }
  else {
    m_textCol = gridCol;
    //Offset next column by some amount of space
    m_textX += (m_textMaxWidth + 2);
    printText(m_textX, boardHeight(), text, fg, bg);
    m_textY = 1;
    m_textMaxWidth = text.size();
  }
}

/* Gets position of top-left corner of screen so that the screen buffer
   (a slice of the game map) stays centered over the player, locking to the
   edges when the player approaches the map edge */
int Display::getCameraCoord(int playerCoord, bool isX)
{
  int screenSize = isX ? m_screenWidth : m_screenHeight;
  int mapSize = isX ? MapWidth : MapHeight;
  if(playerCoord < screenSize / 2) {
    return 0;
  }
  else if(playerCoord >= mapSize - screenSize / 2) {
    return mapSize - screenSize;
  }
  else {
    return playerCoord - screenSize / 2;
  }
}

/* Adds labels/information shown to player in sidebars onscreen to screen
   buffer, respecting the area used to draw the area around the player */
void Display::drawGUI(const Actor &player, const Actor &currActor)
{
  //Draw player/current Actor's info
  printTextCol(0, "Turn:", TB_YELLOW);
  printTextCol(0, " Name: " + currActor.getName());
  printTextCol(0, " Energy: " + std::to_string(currActor.getEnergy()));
  printTextCol(1, "You:", TB_YELLOW);
  printTextCol(1, " Name: " + player.getName());
  printTextCol(1, " Energy: " + std::to_string(player.getEnergy()));

  //Draw event log
  int row = 0;
  while(row < tb_height() && row < m_logRow) {
    printText(m_screenWidth, row, m_log[row]);
    ++row;
  }
  //For printTextCol(); need to be set to 0 after each frame
  //so columns constructed correctly each frame
  m_textCol = 0;
  m_textX = 0;
  m_textY = 0;
  m_textMaxWidth = 0;
}

/* Places all non-empty tiles centered around the player into the screen buffer,
   stopping when there is no more room. Respects area left for GUI */
void Display::draw(const LevelMap &map, const Actor &player, const Actor &currActor)
{
  //Screen may be smaller than map, so display as much as possible
  m_screenWidth = std::min(boardWidth(), MapWidth);
  m_screenHeight = std::min(boardHeight(), MapHeight);
  //Calculate where to start drawing from so player stays centered (if possible)
  m_cornerX = getCameraCoord(player.getX(), true);
  m_cornerY = getCameraCoord(player.getY(), false);
  for(int y=m_cornerY; y<(m_cornerY+m_screenHeight); ++y) {
    for(int x=m_cornerX; x<(m_cornerX+m_screenWidth); ++x) {
      int col = convertCoord(x, true);
      int row = convertCoord(y, false);
      if(map[y][x] != 0) {
	putChar(col, row, map[y][x]);
      }
      else {
	putChar(col, row, EmptySpace);
      }
    }
  }
  drawGUI(player, currActor);
}

/* Checks if window is large enough to adequately display the game */
bool Display::largeEnough()
{
  return tb_width() >= MinDisplayWidth && tb_height() >= MinDisplayHeight;
}
