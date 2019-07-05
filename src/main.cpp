/*TODO:
[ ] Profile to see what is causing memory leaks when resizing window
[ ] Make plain function for reading/return 2d array of map
*/
#include "include/display.h"

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
  void update();
  void movePlayer(int newX, int newY);
  void translatePlayer(int dx, int dy);
};

GameBoard::GameBoard(Display &screen, LevelMap &map) : m_screen(screen),
						       m_map(map), m_player(1, 1)
{
  
}

void GameBoard::update()
{
  m_screen.putMap(m_map);
}

void GameBoard::movePlayer(int newX, int newY)
{
  //Verify that it's safe to move to the new location
  if(newX < MapWidth && newX >= 0 && newY < MapHeight && newY >= 0
     && m_screen.isEmpty(newX, newY))
  {
    //Move the player, updating player object, screen buffer, & map
    //Note: screen doesn't visibly change until screen.present() called in main loop
    int oldX = m_player.getX();
    int oldY = m_player.getY();
    m_screen.clearChar(oldX, oldY);
    m_map[oldY][oldX] = 0;
    m_player.move(newX, newY);
    m_screen.putChar(newX, newY, '@');
    m_map[newY][newX] = '@';
  }
}

void GameBoard::translatePlayer(int dx, int dy)
{
  movePlayer(m_player.getX() + dx, m_player.getY() + dy);
}

int main()
{
  LevelMap map = {{'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'},
		  {0,'@',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {'M',0,0,0,0,0,0,0,0,'@',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {'M',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'M',0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		  {'1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'1'}};

  Display screen;
  GameBoard board(screen, map);
  bool running = true;
  //Show initial map
  screen.putMap(map);
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
	board.update();
      }
      screen.present();
      break;
    }
  }

  return 0;
}
