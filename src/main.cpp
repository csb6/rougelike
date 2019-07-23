/*TODO:
[ ] Profile to see what is causing memory leaks when resizing window
[X] Add way to log messages to sidebar
[ ] Add way to specify characteristics of monsters in text config files, which can
    then be loaded when the program starts
[ ] Add way to save/load maps
[X] Add Item, Chest classes, related code from old RPG project
[X] Remove Chest class/functions; instead, just have loose Items on map
[X] Add way to pick up map items (add to inventory) by walking on them
[ ] Add way to drop items onto map (remove from inventory)
[X] Add inventory system
[X] Add character sheet
[X] Add basic attack system/related logging
[ ] Remove bool returns for moveActor() when Monster AI in update() no longer
    needs it
[ ] Add basic test suite for key functionality (see old RPG code)
[ ] Add RNG functionality (see old RPG code)
[ ] Factor out input functionality into separate Input class which takes
    references to GameBoard, calls new resize() function on it
[X] Add more comprehensive way to view larger inventory
[ ] Add better, safer, more comprehensive way to draw GUI
[ ] Add better, faster way to get ref to Item from an (x, y) coordinate
[ ] Add way to equip items/armor
[ ] Find way to gracefully exit; use it in loadMap()'s error branch
*/
#include "include/gameboard.h"
#include <fstream>
#include <iostream>

/* Creates a new board linking to the termbox screen; opens/loads
   the given map, and sets up in-game GUI*/
GameBoard::GameBoard(bool &running, const std::string &mapPath)
  : m_running(running), m_map{}, m_screen(m_map), m_player_index(0), m_turn_index(0)
{
  loadMap(mapPath);
  //When doing turns, will start iterating through m_actors backward
  //so deletions of Actors are easy/safe; however, 1st turn should be the
  //player's
  m_turn_index = m_player_index;
  player().setTurn(true);
  //Show initial map, centered at player's current position
  m_screen.draw(player(), currActor());
}

/* Fills 2d array with tiles from given map file, instantiating
   new Actors/other entities as needed in their correct positions*/
void GameBoard::loadMap(const std::string &path)
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
      if(col >= MapWidth)
	break;
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
	  m_actors.push_back(Actor(col, row, "Player", '@'));
	}
	else if(line[pos] == 'M')
	{
	  //Add Monster to Actor list
	  m_actors.push_back(Actor(col, row));
	}
	else if(line[pos] == 'i')
	{
	  //Add Item to Item list
	  m_items.push_back(Item(col, row));
	}
	++col;
      }
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
    //First, look at key combos
    switch(m_screen.getEventKey())
    {
    case TB_KEY_CTRL_C:
      //Stop program immediately
      m_running = false;
      break;
    case TB_KEY_ESC:
      //Redraws whole screen (useful for exiting inventory subscreen, etc.)
      m_screen.clear();
      m_screen.hideCursor();
      m_screen.draw(player(), currActor());
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
    default:
      //If not a key combo, look at individual keys
      switch(m_screen.getEventChar())
      {
      case 'i':
	showInventory(player());
	break;
      case 'c':
	showStats(player());
	break;
	//Controls for showing/moving cursor
      case 't':
      {
	if(!m_screen.hasCursor())
	  m_screen.moveCursor(player().getX(), player().getY());
	else
	{
	  int cursorX = m_screen.getCursorX();
	  int cursorY = m_screen.getCursorY();
	  if(isValid(cursorX, cursorY))
	  {
	    moveActor(player(), cursorX, cursorY);
	    log("Player teleported");
	    m_screen.hideCursor();
	    m_screen.clear();
	    m_screen.draw(player(), currActor());
	  }
	}
      }
	break;
      case 'w':
	m_screen.translateCursor(0, -1);
	break;
      case 's':
	m_screen.translateCursor(0, 1);
	break;
      case 'a':
	m_screen.translateCursor(-1, 0);
	break;
      case 'd':
	m_screen.translateCursor(1, 0);
	break;
      }
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
    if(--m_turn_index == -1)
      m_turn_index = m_actors.size() - 1;
    currActor().setTurn(true);
    m_screen.clear();
    m_screen.draw(player(), currActor());
  }

  int i = m_turn_index;
  do
  {
    m_actors[i].update(this);
    if(--i == -1)
      i = m_actors.size() - 1;
  } while(i != m_turn_index);
}

/* Displays an actor's current inventory in subscreen; ESC/any redraws closes it*/
void GameBoard::showInventory(Actor &actor)
{
  m_screen.printText(0, 0, actor.getName() + "'s Inventory: (ESC to exit)", TB_YELLOW);
  int size = actor.getInventorySize();
  int row = 1;
  if(size > 0)
  {
    for(int i=0; i<size; ++i)
    {
      Item& item = actor.getItemAt(i);
      m_screen.printText(0, row, " " + std::to_string(i+1) + ". " + item.getName()
			 + " - Weight: " + std::to_string(item.getWeight()));
      ++row;
    }
  }
  else
    m_screen.printText(2, row, "empty");
}

/*Prints character sheet for an Actor, showing main stats*/
void GameBoard::showStats(Actor &actor)
{
  m_screen.printText(0, 0, actor.getName() + "'s Character Sheet: (ESC to exit)", TB_YELLOW);
  m_screen.printText(0, 1, "Carry Weight: " + std::to_string(actor.m_carryWeight));
  m_screen.printText(0, 2, "Max Carry Weight: " + std::to_string(actor.m_maxCarryWeight));
  m_screen.printText(0, 3, "Level: " + std::to_string(actor.m_level));
  m_screen.printText(0, 4, "XP: " + std::to_string(actor.m_levelProgress));
  m_screen.printText(0, 5, "Strength: " + std::to_string(actor.m_strength));
  m_screen.printText(0, 6, "Cunning: " + std::to_string(actor.m_cunning));
  m_screen.printText(0, 7, "Agility: " + std::to_string(actor.m_agility));
  m_screen.printText(0, 8, "Education: " + std::to_string(actor.m_education));
  m_screen.printText(0, 9, "Sidearm: " + std::to_string(actor.m_sidearmSkill));
  m_screen.printText(0, 10, "Longarm: " + std::to_string(actor.m_longarmSkill));
  m_screen.printText(0, 11, "Melee: " + std::to_string(actor.m_meleeSkill));
  m_screen.printText(0, 12, "Vehicle: " + std::to_string(actor.m_vehicleSkill));
  m_screen.printText(0, 13, "Barter: " + std::to_string(actor.m_barterSkill));
  m_screen.printText(0, 14, "Negotiate: " + std::to_string(actor.m_negotiateSkill));
  m_screen.printText(0, 15, "Trap: " + std::to_string(actor.m_trapSkill));
}

/* Puts text message into stored message log; useful for debugging/showing
   events as they occur*/
void GameBoard::log(const std::string &text)
{
  m_screen.log(text);
}

void GameBoard::present()
{
  m_screen.present();
}

/* Determines if a position is a valid one for an Actor to move into*/
bool GameBoard::isValid(int x, int y)
{
  return x < MapWidth && x >= 0 && y < MapHeight && y >= 0;
}

/* Moves actor from current position to another, redrawing screen
   buffer to show change. Private function, only to be called by
   moveActor()*/
void GameBoard::changePos(Actor &actor, int newX, int newY)
{
  //Move player, update map then screen buffer
  //Note: screen doesn't visibly change until screen.present() called in main loop
  int oldX = actor.getX();
  int oldY = actor.getY();
  actor.move(newX, newY);
  m_map[oldY][oldX] = 0;
  m_map[newY][newX] = actor.getCh();

  m_screen.clear();
  m_screen.draw(player(), currActor());
}

/* Removes given Item from m_items*/
void GameBoard::deleteItem(Item& item)
{
  if(m_items.size() > 0)
  {
    std::swap(item, m_items.back());
    m_items.pop_back();
  }
}

/* Removes given Actor from m_actors*/
void GameBoard::deleteActor(Actor& actor)
{
  if(m_actors.size() > 0)
  {
    using vector_t = std::vector<Actor>::size_type;
    //Find the Actor's position in m_actors of Actor for erase()
    auto pos = std::find(m_actors.begin(), m_actors.end(), actor) - m_actors.begin();
    if(static_cast<vector_t>(pos) >= m_actors.size())
    {
      log("Error: no Actor at" + std::to_string(actor.getX()) + ", "
	  + std::to_string(actor.getY()));
      return;
    }

    log(actor.getName() + " died");
    m_actors.erase(m_actors.begin()+pos);
    //Need to update player/turn indexes to account for deletion
    if(pos < m_player_index)
      --m_player_index;
    else if(pos == m_player_index)
      log("Player is dead/deleted");
    if(static_cast<vector_t>(m_turn_index) >= m_actors.size())
      m_turn_index = m_actors.size() - 1;
  }
}

/* Picks up an Item at given coords if one can be found at that
   position; private function, only to be called by moveActor()*/
void GameBoard::pickupItem(Actor &actor, int x, int y)
{
  for(Item& each : m_items)
  {
    //Find existing Item at the given position
    if(each.getX() == x && each.getY() == y)
    {
      if(actor.canCarry(each.getWeight()))
      {
	actor.addItem(each);
	//Item now in Actor inventory, not on map, so stop tracking
	deleteItem(each);
	m_map[y][x] = 0;
	log(actor.getName() + " picked up " + each.getName());

	m_screen.clear();
	m_screen.draw(player(), currActor());
      }
      break;
    }
  }
}

/* Have given Actor attack an Actor at another position. If no Actor
   at that position, do nothing; private function, only to be called
   by moveActor()*/
void GameBoard::attack(Actor &attacker, int targetX, int targetY)
{
  for(Actor &each : m_actors)
  {
    if(each.getX() == targetX && each.getY() == targetY)
    {
      log(attacker.getName() + " attacked " + each.getName());
      attacker.attack(each);
      if(!each.isAlive())
      {
	m_map[targetY][targetX] = 0;
	deleteActor(each);
      }
      m_screen.clear();
      m_screen.draw(player(), currActor());
      break;
    }
  }
}

/* Performs action on given position; will move Actor there if possible,
   pickup an Item at that position, or attack a Monster at that position*/
bool GameBoard::moveActor(Actor &actor, int newX, int newY)
{
  //Check to make sure turn is respected/that position exists
  if(!actor.isTurn() || !isValid(newX, newY))
    return false;
  //If tile is empty, move Actor to it
  if(m_map[newY][newX] == 0)
  {
    changePos(actor, newX, newY);
    return true;
  }
  //If an Item is in that position, try to pick it up
  else if(m_map[newY][newX] == 'i')
  {
    pickupItem(actor, newX, newY);
  }
  else if(m_map[newY][newX] == 'M' && actor.getName() == "Player")
  {
    attack(actor, newX, newY);
  }
  return false;
}

bool GameBoard::translateActor(Actor &actor, int dx, int dy)
{
  return moveActor(actor, actor.getX() + dx, actor.getY() + dy);
}

/* If possible, moves the player into a new location, updating the player
    object, map array, screen buffer, and display to show the change*/
void GameBoard::movePlayer(int newX, int newY)
{
  moveActor(player(), newX, newY);
}

/* Shortcut for moving player through change in current position*/
void GameBoard::translatePlayer(int dx, int dy)
{
  translateActor(player(), dx, dy);
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
