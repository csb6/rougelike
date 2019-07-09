/*TODO:
[ ] Profile to see what is causing memory leaks when resizing window
[X] Make plain function for reading/return 2d array of map
[X] Implement proper scrolling; look at this link: http://www.roguebasin.com/index.php?title=Scrolling_map
[X] Add vector of all actors/entities? in current level, way to cycle through their turns
[X] Add way to have player Actor in vector but also with a reference in GameBoard
[X] Add basic UI for showing who's turn it is, how many energy steps left
[ ] Fix bug where turn indicator won't update until player tries to move with 0 energy
[X] Establish better lines of communication between Display and GameBoard so
    references don't have to be passed around so ad-hoc
[ ] Implement way to automatically show name/energy of entity whose turn it is
[ ] Add way to save/load maps
[ ] Add Item, Chest classes, related code from old RPG project
[ ] Add inventory system
[ ] Add basic test suite for key functionality (see old RPG code)
[ ] Add RNG functionality (see old RPG code)
[ ] Find way to gracefully exit; use it in loadMapFile()'s error branch
*/
#include "include/display.h"
#include "include/actor.h"
#include <fstream>
#include <iostream>

class GameBoard
//Purpose: To represent the game map/the actors/pieces on it, as well as to
//    handle user input for controlling the player
{
private:
  Display &m_screen;
  LevelMap m_map;
  //m_player_index is always location of player object in m_actors
  int m_player_index;
  //m_turn_index is always location of object whose turn it is in m_actors
  int m_turn_index;
  std::vector<Actor> m_actors;
  inline Actor& player() { return m_actors[m_player_index]; }
  inline Actor& currActor() { return m_actors[m_turn_index]; }
public:
  GameBoard(Display &screen, const std::string &mapPath);
  void loadMapFile(const std::string &path);
  void updateActors();
  void resize();
  bool canMove(int x, int y);
  void movePlayer(int newX, int newY);
  void translatePlayer(int dx, int dy);
};

/* Creates a new board linking to the termbox screen; opens/loads
   the given map, and sets up in-game GUI*/
GameBoard::GameBoard(Display &screen, const std::string &mapPath)
  : m_screen(screen), m_map{}, m_player_index(0), m_turn_index(0)
{
  loadMapFile(mapPath);
  //When doing turns, will start iterating through m_actors backward
  //so deletions of Actors are easy/safe; however, 1st turn should be the
  //player's
  m_turn_index = m_player_index;
  player().setTurn(true, 3);
  //Show initial map, centered at player's current position
  m_screen.draw(m_map, player(), currActor());
}

/* Fills 2d array with tiles from given map file, instantiating
   new Actors/other entities as needed in their correct positions*/
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

  //Next, populate map/m_actors list with data from map file
  int row = 0;
  while(mapFile && row < MapHeight)
  {
    std::string line;
    std::getline(mapFile, line);
    int col = 0;
    for(std::string::size_type pos=0; pos<line.size(); ++pos)
    {
      if(line[pos] == ',' || line[pos] == '\n' || line[pos] == '\r')
	continue;
      else if(line[pos] == '0')
	++col;
      else
      {
	//All Actors need to be in m_actors list/have char in m_map
	m_map[row][col] = line[pos];
	if(line[pos] == '@')
	{
	  //Need to have accurate index for player object
	  m_player_index = m_actors.size();
	  m_actors.push_back(Actor(col, row, "Player"));
	}
	else
	{
	  //Add Actor to Actor list
	  m_actors.push_back(Actor(col, row));
	}
	++col;
      }
      if(col >= MapWidth)
	break;
    }
    ++row;
  }
}

/* Calls update functions on all actors currently on the board,
   working backwards so removing an actor doesn't skip anything*/
void GameBoard::updateActors()
{
  //Check if actor with current turn is done;
  //if so, move turn to next actor, update screen
  if(!currActor().isTurn())
  {
    m_turn_index--;
    currActor().setTurn(true, 3);
    m_screen.draw(m_map, player(), currActor());
    m_screen.present();
  }

  int i = m_turn_index;
  while(i != m_turn_index)
  {
    m_actors[i].update();
    if(i-- == -1)
      i = m_actors.size() - 1;
  }
}

/* Redraws Display to account for window resizing by user*/
void GameBoard::resize()
{
  m_screen.draw(m_map, player(), currActor());
}

/* Determines if a position is a valid one for an Actor to move into*/
bool GameBoard::canMove(int x, int y)
{
  return x < MapWidth && x >= 0 && y < MapHeight && y >= 0
    && m_map[y][x] == 0;
}

/* If possible, moves the player into a new location, updating the player
    object, map array, screen buffer, and display to show the change*/
void GameBoard::movePlayer(int newX, int newY)
{
  //Verify that it's safe/legal to move to the new location
  if(player().isTurn() && canMove(newX, newY))
  {
    //Move player, update map then screen buffer
    //Note: screen doesn't visibly change until screen.present() called in main loop
    int oldX = player().getX();
    int oldY = player().getY();
    player().move(newX, newY);
    m_map[oldY][oldX] = 0;
    m_map[newY][newX] = '@';

    m_screen.clear();
    m_screen.draw(m_map, player(), currActor());
  }
}

/* Shortcut for moving player through change in current position*/
void GameBoard::translatePlayer(int dx, int dy)
{
  movePlayer(player().getX() + dx, player().getY() + dy);
}

int main()
{
  Display screen;
  GameBoard board(screen, "test-map1.csv");
  bool running = true;
  screen.present();
  //Start main game loop
  while(running && screen.processInput())
  {
    board.updateActors();

    switch(screen.getEventType())
    {
    case TB_EVENT_KEY:
      switch(screen.getEventKey())
      {
      case TB_KEY_CTRL_C:
	//Stop program immediately
	running = false;
	break;
      //Basic player movement
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
    //Adjust screen if window is resized
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
