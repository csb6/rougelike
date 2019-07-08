/*TODO:
[ ] Profile to see what is causing memory leaks when resizing window
[X] Make plain function for reading/return 2d array of map
[X] Implement proper scrolling; look at this link: http://www.roguebasin.com/index.php?title=Scrolling_map
[ ] Add vector of all actors/entities? in current level, way to cycle through their turns
[ ] Add way to have player Actor in vector but also with a reference in GameBoard
[ ] Add basic UI for showing who's turn it is, how many energy steps left
[ ] Add Item, Chest classes, related code from old RPG project
[ ] Add inventory system
[ ] Add basic test suite for key functionality (see old RPG code)
[ ] Add RNG functionality (see olf RPG code)
[ ] Find way to gracefully exit; use it in loadMapFile()'s error branch
*/
#include "include/display.h"
#include <fstream>
#include <iostream>
#include <vector>

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
  LevelMap m_map;
  //m_player_index always is location of player object in m_actors
  int m_player_index;
  std::vector<Actor> m_actors;
  inline Actor& player() { return m_actors[m_player_index]; }
public:
  GameBoard(Display &screen, const std::string &mapPath);
  void resize();
  bool canMove(int x, int y);
  void movePlayer(int newX, int newY);
  void translatePlayer(int dx, int dy);
  void loadMapFile(const std::string &path);
};

GameBoard::GameBoard(Display &screen, const std::string &mapPath)
  : m_screen(screen), m_map{}, m_player_index(0)
{
  //Show initial map
  loadMapFile(mapPath);
  m_screen.putMap(m_map, 1, 1);
}

void GameBoard::resize()
{
  m_screen.putMap(m_map, player().getX(), player().getY());
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
    int oldX = player().getX();
    int oldY = player().getY();
    m_map[oldY][oldX] = 0;
    player().move(newX, newY);
    m_map[newY][newX] = '@';

    m_screen.clear();
    m_screen.putMap(m_map, newX, newY);
  }
}

void GameBoard::translatePlayer(int dx, int dy)
{
  movePlayer(player().getX() + dx, player().getY() + dy);
}

void GameBoard::loadMapFile(const std::string &path)
{
  //First, make all tiles empty tiles
  for(int row=0; row<MapHeight; ++row)
  {
    for(int col=0; col<MapWidth; ++col)
    {
      m_map[row][col] = 0;
    }
  }
  std::ifstream mapFile(path);
  if(!mapFile)
  {
    std::cerr << "Error: could not load map file\n";
    exit(1);
  }

  //Next, populate map with data from map file
  int row = 0;
  while(mapFile && row < MapHeight)
  {
    std::string line;
    std::getline(mapFile, line);
    int col = 0;
    for(std::string::size_type pos=0; pos<line.size(); ++pos)
    {
      if(line[pos] == ',' || line[pos] == '\n')
	continue;
      else if(line[pos] == '0')
	++col;
      else
      {
	m_map[row][col] = line[pos];
	//Add Actor to Actor list
	if(line[pos] == '@')
	{
	  //Need to have accurate index for player object
	  m_player_index = m_actors.size();
	}
	m_actors.push_back(Actor(col, row));
	++col;
      }
      if(col >= MapWidth)
	break;
    }
    ++row;
  }
}

int main()
{
  //LevelMap map;
  //loadMapFile(map, "test-map1.csv");
  Display screen;
  GameBoard board(screen, "test-map1.csv");
  bool running = true;
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
	screen.present();
	break;
      case TB_KEY_ARROW_LEFT:
	board.translatePlayer(-1, 0);
	screen.present();
	break;
      case TB_KEY_ARROW_UP:
	board.translatePlayer(0, -1);
	screen.present();
	break;
      case TB_KEY_ARROW_DOWN:
	board.translatePlayer(0, 1);
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
