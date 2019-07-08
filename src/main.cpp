/*TODO:
[ ] Profile to see what is causing memory leaks when resizing window
[ ] Make plain function for reading/return 2d array of map
[X] Implement proper scrolling; look at this link: http://www.roguebasin.com/index.php?title=Scrolling_map
*/
#include "include/display.h"
#include <fstream>

class Actor
{
private:
  int m_xPos, m_yPos;
public:
  Actor(int x, int y);
  void move(int newX, int newY);
  //Setters/Getters
  int getX() { return m_xPos; }
  int getY() { return m_yPos; }
};

Actor::Actor(int x, int y) : m_xPos(x), m_yPos(y)
{
  
}

void Actor::move(int newX, int newY)
{
  m_xPos = newX;
  m_yPos = newY;
}

class GameBoard
{
private:
  Display &m_screen;
  LevelMap &m_map;
  Actor m_player;
public:
  GameBoard(Display &screen, LevelMap &map);
  void resize();
  bool canMove(int x, int y);
  void movePlayer(int newX, int newY);
  void translatePlayer(int dx, int dy);
};

GameBoard::GameBoard(Display &screen, LevelMap &map) : m_screen(screen),
						       m_map(map), m_player(1, 1)
{
  
}

void GameBoard::resize()
{
  m_screen.putMap(m_map, m_player.getX(), m_player.getY());
}

bool GameBoard::canMove(int x, int y)
{
  return x < MapWidth && x >= 0 && y < MapHeight && y >= 0
    && m_map[y][x] == 0;
}

void GameBoard::movePlayer(int newX, int newY)
{
  //Verify that it's safe to move to the new location
  if(canMove(newX, newY))
  {
    //Move the player, updating player object, screen buffer, & map
    //Note: screen doesn't visibly change until screen.present() called in main loop
    int oldX = m_player.getX();
    int oldY = m_player.getY();
    m_map[oldY][oldX] = 0;
    m_player.move(newX, newY);
    m_map[newY][newX] = '@';

    m_screen.clear();
    m_screen.putMap(m_map, newX, newY);
  }
}

void GameBoard::translatePlayer(int dx, int dy)
{
  movePlayer(m_player.getX() + dx, m_player.getY() + dy);
}

void loadMapFile(LevelMap &map, const std::string &path)
{
  std::fstream mapFile;
  mapFile.open(path, std::fstream::in);
  char cell;
  int row = 0;
  int col = 0;
  while(mapFile >> cell && row < MapHeight)
  {
    if(cell == ',')
      continue;
    else if(cell == ';' || col >= MapWidth)
    {
      ++row;
      col = 0;
    }
    else
    {
      if(cell == '0')
	cell = 0;
      map[row][col] = cell;
      ++col;
    }
  }
  mapFile.close();
}

int main()
{
  LevelMap map = {{'1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1'},
		  {'1','@',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,'@',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'M',0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,'b',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {'1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1'}};

  /*LevelMap map;
    loadMapFile(map, "test-map1.csv");*/
  Display screen;
  GameBoard board(screen, map);
  bool running = true;
  //Show initial map
  screen.putMap(map, 1, 1);
  screen.present();
  //Start main game loop
  while(running && screen.processInput())
  {
    switch(screen.getEventType())
    {
    case TB_EVENT_KEY:
      switch(screen.getEventKey())
      {
      case TB_KEY_CTRL_C:
	//Stop program immediately
	running = false;
	break;
      case TB_KEY_ARROW_RIGHT:
	board.translatePlayer(1, 0);
	//screen.draw();
	screen.present();
	break;
      case TB_KEY_ARROW_LEFT:
	board.translatePlayer(-1, 0);
	//screen.draw();
	screen.present();
	break;
      case TB_KEY_ARROW_UP:
	board.translatePlayer(0, -1);
	//screen.draw();
	screen.present();
	break;
      case TB_KEY_ARROW_DOWN:
	board.translatePlayer(0, 1);
	//screen.draw();
	screen.present();
	break;
      }
      break;
    case TB_EVENT_RESIZE:
      screen.clear();
      if(!screen.largeEnough())
	screen.printText(1, 1, "Error: screen not large enough");
      else
      {
	//Redraw as much of map as possible
	board.resize();
      }
      screen.present();
      break;
    }
  }

  return 0;
}
