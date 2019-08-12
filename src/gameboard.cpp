#include "include/gameboard.h"
#include <fstream>
#include <cmath>

/* Distance formula with truncated absolute value result*/
static int distanceFrom(int x1, int y1, int x2, int y2)
{
  return std::abs(std::sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)));
}

/* Creates a new board linking to the termbox screen; opens/loads
   the given map, and sets up in-game GUI*/
GameBoard::GameBoard(Display &screen, Actor playerCh, const std::string &mapPath)
  : m_map{}, m_screen(screen), m_player_index(0), m_turn_index(0)
{
  m_actors.push_back(playerCh);
  m_player_index = 0;
  m_turn_index = m_player_index;
  loadMap(mapPath);
  //When doing turns, will start iterating through m_actors backward
  //so deletions of Actors are easy/safe; however, 1st turn should be the
  //player's
  player().setTurn(true);
  //Show initial map, centered at player's current position
  m_screen.draw(m_map, player(), currActor());
}

/* Fills 2d array with tiles from given map file, instantiating
   new Actors/other entities as needed in their correct positions*/
void GameBoard::loadMap(const std::string &path)
{
  //First, make all tiles empty tiles
  for(int row=0; row<MapHeight; ++row) {
    for(int col=0; col<MapWidth; ++col) {
      m_map[row][col] = 0;
    }
  }
  std::ifstream mapFile(path);
  if(!mapFile) {
    m_screen.printText(0, 0, "Error: could not load map file\n");
    m_screen.input("Press Enter to exit", 0, 1);
    exit(1);
  }

  //Next, populate map/m_actors list with data from map file
  int row = 0;
  while(mapFile && row < MapHeight)
  {
    std::string line;
    std::getline(mapFile, line);
    int col = 0;
    for(std::string::size_type pos=0; pos<line.size(); ++pos) {
      if(col >= MapWidth)
	break;
      if(line[pos] == ',' || line[pos] == '\n' || line[pos] == '\r')
	continue;
      else if(line[pos] == '0')
	++col;
      else {
	//All Actors need to be in m_actors list/have char in m_map
	m_map[row][col] = line[pos];
	if(line[pos] == '@') {
	  //Need to have accurate positioning for player object
	  player().move(col, row);
	  player().setCh('@');
	}
	else if(line[pos] == 'M') {
	  //Add Monster to Actor list
	  m_actors.push_back(Actor(col, row));
	}
	else if(line[pos] == 'i') {
	  //Add Item to Item list
	  m_items.push_back(Item(col, row));
	}
	++col;
      }
    }
    ++row;
  }
}

/* Toggles cursor on/off; calls function pointer/disables cursor when called
   and cursor active*/
void GameBoard::bindCursorMode(Actor &actor, bool (GameBoard::*action)(Actor&, int, int))
{
  //Set-up action on first keypress
  if(!m_screen.hasCursor())
    m_screen.moveCursor(actor.getX(), actor.getY());
  //Execute action, passing cursor position, on second keypress
  else {
    int cursorX = m_screen.getCursorX();
    int cursorY = m_screen.getCursorY();
    if(isValid(cursorX, cursorY)) {
      (this->*action)(actor, cursorX, cursorY);
      m_screen.hideCursor();
      m_screen.clear();
      m_screen.draw(m_map, player(), currActor());
    }
  }
}

/* Calls update functions on all actors currently on the board,
   working backwards so removing an actor doesn't skip anything*/
void GameBoard::updateActors()
{
  //Check if actor with current turn is done;
  //if so, move turn to next actor, update screen
  if(!currActor().isTurn()) {
    if(--m_turn_index == -1)
      m_turn_index = m_actors.size() - 1;
    currActor().setTurn(true);
    m_screen.clear();
    m_screen.draw(m_map, player(), currActor());
  }

  int i = m_turn_index;
  do {
    m_actors[i].update(this);
    if(--i == -1)
      i = m_actors.size() - 1;
  } while(i != m_turn_index);
}

/* Displays an actor's current inventory in subscreen; ESC/any redraws closes it*/
void GameBoard::showInventory(Actor &actor)
{
  m_screen.printText(0, 0, actor.getName() + "'s Inventory: (ESC to exit)", TB_YELLOW);
  m_screen.printText(0, 1, "E) Equip item, D) Deequip item", TB_YELLOW);
  int size = actor.getInventorySize();
  int row = 2;
  if(size > 0) {
    for(int i=0; i<size; ++i) {
      Item& item = actor.getItemAt(i);
      m_screen.printText(0, row, " " + std::to_string(i+1) + ". " + item.getName()
			 + " - Weight: " + std::to_string(item.getWeight())
			 + ", Armor: " + std::to_string(item.getArmor()), TB_CYAN);
      ++row;
    }
  }
  else
    m_screen.printText(2, row, "Empty", TB_CYAN);
}

/*Prints character sheet for an Actor, showing main stats*/
void GameBoard::showStats(Actor &actor)
{
  m_screen.printText(0, 0, actor.getName() + "'s Character Sheet: (ESC to exit)", TB_YELLOW);
  m_screen.printText(0, 1, "Health: " + std::to_string(actor.getHealth()), TB_CYAN);
  m_screen.printText(0, 2, "Carry Weight: " + std::to_string(actor.m_carryWeight), TB_CYAN);
  m_screen.printText(0, 3, "Carry Capacity: " + std::to_string(actor.m_maxCarryWeight), TB_CYAN);
  m_screen.printText(0, 4, "Level: " + std::to_string(actor.m_level), TB_CYAN);
  m_screen.printText(0, 5, "XP: " + std::to_string(actor.m_levelProgress), TB_CYAN);
  m_screen.printText(0, 6, "Strength: " + std::to_string(actor.m_strength), TB_CYAN);
  m_screen.printText(0, 7, "Cunning: " + std::to_string(actor.m_cunning), TB_CYAN);
  m_screen.printText(0, 8, "Agility: " + std::to_string(actor.m_agility), TB_CYAN);
  m_screen.printText(0, 9, "Education: " + std::to_string(actor.m_education), TB_CYAN);
  m_screen.printText(0, 10, "Sidearm: " + std::to_string(actor.m_sidearmSkill), TB_CYAN);
  m_screen.printText(0, 11, "Longarm: " + std::to_string(actor.m_longarmSkill), TB_CYAN);
  m_screen.printText(0, 12, "Melee: " + std::to_string(actor.m_meleeSkill), TB_CYAN);
  m_screen.printText(0, 13, "Vehicle: " + std::to_string(actor.m_vehicleSkill), TB_CYAN);
  m_screen.printText(0, 14, "Barter: " + std::to_string(actor.m_barterSkill), TB_CYAN);
  m_screen.printText(0, 15, "Negotiate: " + std::to_string(actor.m_negotiateSkill), TB_CYAN);
  m_screen.printText(0, 16, "Trap: " + std::to_string(actor.m_trapSkill), TB_CYAN);
}

void GameBoard::showEquipped(Actor &actor)
{
  std::string labels[EQUIP_MAX] = {". Head: ", ". Chest: ", ". Legs: ", ". Feet: ",
				   ". Melee: ", ". Ranged: "};
  m_screen.printText(0, 0, actor.getName() + "'s Equipped Items: (ESC to exit)", TB_YELLOW);
  for(int i=0; i<EQUIP_MAX; ++i) {
    Item *item = actor.getEquipped(i);
    if(item == nullptr)
      m_screen.printText(0, i+1, std::to_string(i+1) + labels[i] + "Empty", TB_CYAN);
    else
      m_screen.printText(0, i+1, std::to_string(i+1) + labels[i] + item->getName(), TB_CYAN);
  }
}

/* Puts text message into stored message log; useful for debugging/showing
   events as they occur*/
void GameBoard::log(const std::string &text)
{
  m_screen.log(text);
}

void GameBoard::redraw()
{
  //Redraws whole screen (useful for exiting inventory subscreen, etc.)
  m_screen.clear();
  m_screen.hideCursor();
  m_screen.draw(m_map, player(), currActor());
}

void GameBoard::present()
{
  m_screen.present();
}

/* Determines if a position is a valid one for an Actor to move into*/
bool GameBoard::isValid(int x, int y) const
{
  return x < MapWidth && x >= 0 && y < MapHeight && y >= 0;
}

/* Moves actor from current position to another, redrawing screen
   buffer to show change. Private function, only to be called by
   moveActor()*/
void GameBoard::changePos(Actor &actor, int newX, int newY)
{
  //Note: screen doesn't visibly change until screen.present() called in main loop
  int oldX = actor.getX();
  int oldY = actor.getY();
  actor.move(newX, newY);
  m_map[oldY][oldX] = 0;
  m_map[newY][newX] = actor.getCh();

  m_screen.clear();
  m_screen.draw(m_map, player(), currActor());
}

/* Removes given Item from m_items*/
void GameBoard::deleteItem(Item& item)
{
  if(m_items.size() > 0) {
    std::swap(item, m_items.back());
    m_items.pop_back();
  }
}

/* Removes given Actor from m_actors*/
void GameBoard::deleteActor(Actor& actor)
{
  if(m_actors.size() > 0) {
    using vector_t = std::vector<Actor>::size_type;
    //Find the Actor's position in m_actors of Actor for erase()
    auto pos = std::find(m_actors.begin(), m_actors.end(), actor) - m_actors.begin();
    if(static_cast<vector_t>(pos) >= m_actors.size()) {
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
      m_turn_index = 0;
  }
}

/* Equips item in inventory into an Actor's equip slots*/
void GameBoard::equipItem(Actor &actor)
{
  int index = m_screen.input("Enter item #: ") - 1;
  int pos = m_screen.input("Enter equip position [1-"
			   + std::to_string(EQUIP_MAX) + "]: ", 0, 1) - 1;
  actor.equipItem(index, pos);

  m_screen.clear();
  m_screen.draw(m_map, player(), currActor());
}

/* Deequips item from Actor's armor slot*/
void GameBoard::deequipItem(Actor &actor)
{
  int pos = m_screen.input("Enter equip position [1-"
			   + std::to_string(EQUIP_MAX) + "]: ", 0, 1) - 1;
  actor.deequipItem(pos);

  m_screen.clear();
  m_screen.draw(m_map, player(), currActor());
}

/* Picks up an Item at given coords if one can be found at that
   position; private function, only to be called by moveActor()*/
void GameBoard::pickupItem(Actor &actor, int x, int y)
{
  for(Item& each : m_items) {
    //Find existing Item at the given position
    if(each.getX() == x && each.getY() == y) {
      if(actor.canCarry(each.getWeight())) {
	actor.addItem(each);
	//Item now in Actor inventory, not on map, so stop tracking
	deleteItem(each);
	m_map[y][x] = 0;
	log(actor.getName() + " picked up " + each.getName());

	m_screen.clear();
	m_screen.draw(m_map, player(), currActor());
      }
      break;
    }
  }
}

/* Have given Actor attack an Actor at another position. If no Actor
   at that position, do nothing; private function, only to be called
   by moveActor()*/
void GameBoard::melee(Actor &attacker, int targetX, int targetY)
{
  for(Actor &each : m_actors) {
    if(each.getX() == targetX && each.getY() == targetY) {
      //Attacker attempts to attack; print result (success/fail)
      if(attacker.attack(each))
	log(attacker.getName() + " attacked " + each.getName());
      else
	log(each.getName() + " attacked " + attacker.getName());

      if(!each.isAlive()) {
	m_map[targetY][targetX] = 0;
	deleteActor(each);
      }
      m_screen.clear();
      m_screen.draw(m_map, player(), currActor());
      break;
    }
  }
}

/* Performs action on given position; will move Actor there if possible,
   pickup an Item at that position, or attack a Monster at that position*/
bool GameBoard::moveActor(Actor &actor, int newX, int newY)
{
  //Check to make sure turn is respected/position exists/is within teleport range
  if(!actor.isTurn() || !isValid(newX, newY)
     || distanceFrom(actor.getX(), actor.getY(), newX, newY) >= 5)
    return false;
  //If tile is empty, move Actor to it
  if(m_map[newY][newX] == 0) {
    changePos(actor, newX, newY);
  }
  //If an Item is in that position, try to pick it up
  else if(m_map[newY][newX] == 'i') {
    pickupItem(actor, newX, newY);
  }
  else if(m_map[newY][newX] == 'M' && actor == player()) {
    melee(actor, newX, newY);
  }
  else
    return false;
  return true;
}

bool GameBoard::rangeAttack(Actor& attacker, int targetX, int targetY)
{
  for(Actor &each : m_actors) {
    if(each.getX() == targetX && each.getY() == targetY) {
      //Figure out whether to throw/fire projectile
      Item *weapon = attacker.getEquipped(RANGE_WEAPON);
      if(weapon == nullptr) {
	log("No ranged weapon to use");
	return false;
      }
      if(weapon->isRanged()) {
	//Fire projectile
	//Attacker attempts to attack; print result (success/fail)
	if(attacker.attack(each))
	  log(attacker.getName() + " range attacked " + each.getName());
	else
	  log(each.getName() + " range attacked " + attacker.getName());
      }
      else {
	//Throw item
	log("Item thrown");
      }

      if(!each.isAlive()) {
	m_map[targetY][targetX] = 0;
	deleteActor(each);
      }
      m_screen.clear();
      m_screen.draw(m_map, player(), currActor());
      return true;
      break;
    }
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
  if(!m_screen.hasCursor())
    translateActor(player(), dx, dy);
  else
    m_screen.translateCursor(dx, dy);
}
