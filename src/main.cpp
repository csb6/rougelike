/*TODO:
[ ] Profile to see what is causing memory leaks when resizing window
[X] Make plain function for reading/return 2d array of map
[X] Implement proper scrolling; look at this link: http://www.roguebasin.com/index.php?title=Scrolling_map
[X] Add vector of all actors/entities? in current level, way to cycle through their turns
[X] Add way to have player Actor in vector but also with a reference in GameBoard
[X] Add basic UI for showing who's turn it is, how many energy steps left
[X] Fix bug where turn indicator won't update until player tries to move with 0 energy
[X] Establish better lines of communication between Display and GameBoard so
    references don't have to be passed around so ad-hoc
[X] Call screen.present() in only 1 spot every tick: right after updateActors()
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
  bool &m_running;
  LevelMap m_map;
  Display m_screen;
  //m_player_index is always location of player object in m_actors
  int m_player_index;
  //m_turn_index is always location of object whose turn it is in m_actors
  int m_turn_index;
  std::vector<Actor> m_actors;
  inline Actor& player() { return m_actors[m_player_index]; }
  inline Actor& currActor() { return m_actors[m_turn_index]; }
public:
  GameBoard(bool &running, const std::string &mapPath);
  void loadMapFile(const std::string &path);
  bool processInput();
  void updateActors();
  void present();
  bool canMove(int x, int y);
  void movePlayer(int newX, int newY);
  void translatePlayer(int dx, int dy);
};

/* Creates a new board linking to the termbox screen; opens/loads
   the given map, and sets up in-game GUI*/
GameBoard::GameBoard(bool &running, const std::string &mapPath)
  : m_running(running), m_map{}, m_screen(m_map), m_player_index(0), m_turn_index(0)
{
  loadMapFile(mapPath);
  //When doing turns, will start iterating through m_actors backward
  //so deletions of Actors are easy/safe; however, 1st turn should be the
  //player's
  m_turn_index = m_player_index;
  player().setTurn(true, 3);
  //Show initial map, centered at player's current position
  m_screen.draw(player(), currActor());
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

/* Checks with screen to see if any user input, then changes game state
   based on the event type (e.g. move player, resize screen) */
bool GameBoard::processInput()
{
  if(!m_screen.getInput())
    return false;
  switch(m_screen.getEventType())
  {
  //Respond to user key presses
  case TB_EVENT_KEY:
    switch(m_screen.getEventKey())
    {
    case TB_KEY_CTRL_C:
      //Stop program immediately
      m_running = false;
      break;
      //Basic player movement
    case TB_KEY_ARROW_RIGHT:
      translatePlayer(1, 0);
      break;
    case TB_KEY_ARROW_LEFT:
      translatePlayer(-1, 0);
      break;
    case TB_KEY_ARROW_UP:
      translatePlayer(0, -1);
      break;
    case TB_KEY_ARROW_DOWN:
      translatePlayer(0, 1);
      break;
    }
    break;
  //Adjust screen if window is resized
  case TB_EVENT_RESIZE:
    m_screen.clear();
    if(!m_screen.largeEnough())
      m_screen.printText(1, 1, "Error: screen not large enough");
    else
    {
      //Redraw as much of map as possible
      m_screen.draw(player(), currActor());
    }
    break;
  }
  return true;
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
    m_screen.draw(player(), currActor());
  }

  int i = m_turn_index;
  while(i != m_turn_index)
  {
    m_actors[i].update();
    if(--i == -1)
      i = m_actors.size() - 1;
  }
}

void GameBoard::present()
{
  m_screen.present();
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
    m_screen.draw(player(), currActor());
  }
}

/* Shortcut for moving player through change in current position*/
void GameBoard::translatePlayer(int dx, int dy)
{
  movePlayer(player().getX() + dx, player().getY() + dy);
}

int main()
{
  bool running = true;
  GameBoard board(running, "test-map1.csv");
  //Start main game loop
  while(running && board.processInput())
  {
    board.updateActors();
    board.present();
  }

  return 0;
}
