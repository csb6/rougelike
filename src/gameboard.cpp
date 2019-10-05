#include "include/gameboard.h"
#include <fstream>
#include <cmath>

std::string getLocalDir();

const int ItemVecDefaultSize = 5;
const int ActorVecDefaultSize = 10;

static std::string::size_type findSplit(const std::string line)
{
  std::string::size_type split(0);
  for(std::string::size_type i=0; i<line.size(); ++i) {
    if(line[i] == '=') {
      split = i;
      break;
    }
  }
  return split;
}

static char parseChar(std::string value, char defaultVal = 'A')
{
  if(value.size() < 1 || value == "default") {
    return defaultVal;
  }
  return value[0];
}

static std::int_least16_t parseInt(std::string value, std::int_least16_t defaultVal = 0)
{
  if(value.size() < 1 || value == "default") {
    return defaultVal;
  }
  try {
    return std::stoi(value);
  }
  catch(const std::out_of_range& e) {
    return defaultVal;
  }
}

static bool parseBool(std::string value, bool defaultVal = false)
{
  if(value == "true") return true;
  else if(value == "false") return false;
  else return defaultVal;
}

static void applyIniPair(Actor &actor, std::string key, std::string value)
{
  if(key == "char") actor.setCh(parseChar(value));
  else if(key == "name") actor.setName(value);
  else if(key == "energy") actor.setEnergy(parseInt(value));
  else if(key == "health") actor.addHealth(parseInt(value, 100));
  else if(key == "carryWeight") actor.m_carryWeight = parseInt(value);
  else if(key == "carryWeight") actor.m_carryWeight = parseInt(value);
  else if(key == "maxCarryWeight") actor.m_maxCarryWeight = parseInt(value, 10);
  else if(key == "level") actor.m_level = parseInt(value);
  else if(key == "levelProgress") actor.m_levelProgress = parseInt(value);
  else if(key == "strength") actor.m_strength = parseInt(value);
  else if(key == "cunning") actor.m_cunning = parseInt(value);
  else if(key == "agility") actor.m_agility = parseInt(value);
  else if(key == "education") actor.m_education = parseInt(value);
  else if(key == "sidearmSkill") actor.m_sidearmSkill = parseInt(value);
  else if(key == "longarmSkill") actor.m_longarmSkill = parseInt(value);
  else if(key == "meleeSkill") actor.m_meleeSkill = parseInt(value);
  else if(key == "barterSkill") actor.m_barterSkill = parseInt(value);
  else if(key == "negotiateSkill") actor.m_negotiateSkill = parseInt(value);
}

static void applyIniPair(Item &item, std::string key, std::string value)
{
  if(key == "name") item.setName(value);
  else if(key == "weight") item.setWeight(parseInt(value));
  else if(key == "attack") item.setAttack(parseInt(value));
  else if(key == "armor") item.setArmor(parseInt(value));
  else if(key == "isMelee") item.setMelee(parseBool(value));
  else if(key == "isRanged") item.setRanged(parseBool(value));
}

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
  m_items.reserve(ItemVecDefaultSize);
  m_actors.reserve(ActorVecDefaultSize);

  m_actors.push_back(playerCh);
  m_turn_index = m_player_index;

  loadMonsterTemplates(getLocalDir() + "src/monsters.ini");
  //Be careful that no Monsters/Items have same char representing
  //them in .ini files; an Item/Monster will be added in the same location
  loadItemTemplates(getLocalDir() + "src/items.ini");

  loadMap(mapPath);
  //When doing turns, will start iterating through m_actors backward
  //so deletions of Actors are easy/safe; however, 1st turn should be the
  //player's
  player().setTurn(true);
  //Show initial map, centered at player's current position
  m_screen.draw(m_map, player(), currActor());
}

void GameBoard::loadItemTemplates(const std::string &&path)
{
  std::ifstream itemFile(path);
  if(!itemFile) {
    m_screen.printText(0, 0, "Error: could not load item file: " + path + "\n");
    m_screen.input("Press Enter to exit", 0, 1);
    exit(1);
  }

  char ch = 0;
  bool hasCh = false;
  Item newTemplate;
  while(itemFile) {
    std::string line;
    std::getline(itemFile, line);
    //Skip comments
    if(line[0] == '#')
      continue;
    //Finalize/add template when at blank line (end of section)
    else if(line == "" && hasCh) {
      m_itemTemplates[ch] = newTemplate;
      newTemplate = Item();
      hasCh = false;
      continue;
    }
    //Find out where '=' is; it's the division between key/value
    auto split = findSplit(line);
    //Ignore erroneously formatted lines
    if(split == 0 || split == (line.size() - 1)) {
      continue;
    }

    if(line.substr(0, split) == "char") {
      //All items represented by a char in map files, but not onscreen,
      //so char not part of the Item objects themselves
      ch = parseChar(line.substr(split+1), '-');
      hasCh = true;
    }

    applyIniPair(newTemplate, line.substr(0, split), line.substr(split+1));
  }
}

void GameBoard::loadMonsterTemplates(const std::string &&path)
{
  std::ifstream monsterFile(path);
  if(!monsterFile) {
    m_screen.printText(0, 0, "Error: could not load monster file: " + path + "\n");
    m_screen.input("Press Enter to exit", 0, 1);
    exit(1);
  }

  Actor newTemplate;
  while(monsterFile) {
    std::string line;
    std::getline(monsterFile, line);
    //Skip comments
    if(line[0] == '#')
      continue;
    //Finalize/add template when at blank line (end of section)
    else if(line == "") {
      m_templates[newTemplate.getCh()] = newTemplate;
      newTemplate = Actor();
      continue;
    }
    //Find out where '=' is; it's the division between key/value
    auto split = findSplit(line);
    //Ignore erroneously formatted lines
    if(split == 0 || split == (line.size() - 1)) {
      continue;
    }

    applyIniPair(newTemplate, line.substr(0, split), line.substr(split+1));
  }
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
    m_screen.printText(0, 0, "Error: could not load map file: " + path + "\n");
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
      if(col >= MapWidth) {
	break;
      }
      if(line[pos] == ',' || line[pos] == '\n' || line[pos] == '\r') {
	continue;
      }
      else if(line[pos] == '0') {
	++col;
      }
      else {
	//All Actors need to be in m_actors list/have char in m_map
	m_map[row][col] = line[pos];
	if(line[pos] == PlayerTile) {
	  //Need to have accurate positioning for player object
	  player().move(col, row);
	  player().setCh(PlayerTile);
	}
	else if(m_itemTemplates.find(line[pos]) != m_itemTemplates.end()) {
	  Item item = m_itemTemplates[line[pos]];
	  item.move(col, row);
	  //Add Item to Item list
	  m_items.push_back(item);
	}
	else if(m_templates.find(line[pos]) != m_templates.end()) {
	  //If in template list, create monster mapped from given char
	  Actor monster = m_templates[line[pos]];
	  monster.move(col, row);
	  m_actors.push_back(monster);
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
  //Put cursor at player position on first keypress
  if(!m_screen.hasCursor()) {
    m_screen.moveCursor(actor.getX(), actor.getY());
  }
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
    if(--m_turn_index == -1) {
      m_turn_index = m_actors.size() - 1;
    }
    currActor().setTurn(true);
    m_screen.clear();
    m_screen.draw(m_map, player(), currActor());
  }

  int i = m_turn_index;
  do {
    m_actors[i].update(this);
    if(--i == -1) {
      i = m_actors.size() - 1;
    }
  } while(i != m_turn_index);
}

/* Displays an actor's current inventory in subscreen; ESC/any redraws closes it*/
void GameBoard::showInventory(Actor &actor)
{
  m_screen.printText(0, 0, actor.getName() + "'s Inventory: (ESC to exit)", TB_YELLOW);
  m_screen.printText(0, 1, "E) Equip item, D) Deequip item", TB_YELLOW);
  int size = actor.getInventorySize();
  int row = 2;
  if(size <= 0) {
    m_screen.printText(2, row, "Empty", TB_CYAN);
    return;
  }
  for(int i=0; i<size; ++i) {
    Item* item = actor.getItemAt(i);
    if(item == nullptr) continue;
    m_screen.printText(0, row, " " + std::to_string(i+1) + ". " + item->getName()
		       + " - Weight: " + std::to_string(item->getWeight())
		       + ", Armor: " + std::to_string(item->getArmor()), TB_CYAN);
    ++row;
  }
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
  m_screen.printText(0, 13, "Barter: " + std::to_string(actor.m_barterSkill), TB_CYAN);
  m_screen.printText(0, 14, "Negotiate: " + std::to_string(actor.m_negotiateSkill), TB_CYAN);
}

/* Show list of equipment slots, showing which items in which slots/which
   slots are empty*/
void GameBoard::showEquipped(Actor &actor)
{
  std::string labels[EQUIP_MAX] = {". Head: ", ". Chest: ", ". Legs: ", ". Feet: ",
				   ". Melee: ", ". Ranged: "};
  m_screen.printText(0, 0, actor.getName() + "'s Equipped Items: (ESC to exit)", TB_YELLOW);
  for(int i=0; i<EQUIP_MAX; ++i) {
    Item *item = actor.getEquipped(i);
    if(item == nullptr) {
      m_screen.printText(0, i+1, std::to_string(i+1) + labels[i] + "Empty", TB_CYAN);
    }
    else {
      m_screen.printText(0, i+1, std::to_string(i+1) + labels[i] + item->getName(), TB_CYAN);
    }
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

/* Removes given Item from m_items*/
void GameBoard::deleteItem(int x, int y)
{
  if(m_items.size() <= 0) {
    return;
  }

  for(unsigned int i=0; i<m_items.size(); ++i) {
    if(m_items[i].getX() == x && m_items[i].getY() == y) {
      m_items.erase(m_items.begin()+i);
      return;
    }
  }
  log("Error: Couldn't find item");
}

/* Removes given Actor from m_actors*/
void GameBoard::deleteActor(int x, int y)
{
  if(m_actors.size() <= 0) {
    return;
  }
  using vector_t = std::vector<Actor>::size_type;

  bool foundActor = false;
  int pos = 0;
  for(vector_t i=0; i<m_actors.size(); ++i) {
    if(m_actors[i].getX() == x && m_actors[i].getY() == y) {
      log(m_actors[i].getName() + " died");
      m_actors.erase(m_actors.begin()+i);
      foundActor = true;
      pos = i;
      break;
    }
  }
  if(!foundActor) {
    log("Error: actor not found");
    return;
  }
  //Need to update player/turn indexes to account for deletion
  if(pos < m_player_index) {
    --m_player_index;
  }
  else if(pos == m_player_index) {
    log("Player is dead/deleted");
  }

  if(static_cast<vector_t>(m_turn_index) >= m_actors.size()) {
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

/* Moves actor from current position to another, redrawing screen
   buffer to show change. Private function, only to be called by
   moveActor()*/
bool GameBoard::changePos(Actor &actor, int newX, int newY)
{
  //Note: screen doesn't visibly change until screen.present() called in main loop
  int oldX = actor.getX();
  int oldY = actor.getY();
  actor.move(newX, newY);
  m_map[oldY][oldX] = 0;
  m_map[newY][newX] = actor.getCh();

  m_screen.clear();
  m_screen.draw(m_map, player(), currActor());
  return true;
}

/* Picks up an Item at given coords if one can be found at that
   position; private function, only to be called by moveActor()*/
bool GameBoard::pickupItem(Actor &actor, int x, int y)
{
  for(Item& each : m_items) {
    //Find existing Item at the given position
    if(each.getX() == x && each.getY() == y) {
      if(actor.canCarry(each.getWeight())) {
	actor.addItem(each);
	//Item now in Actor inventory, not on map, so stop tracking
	deleteItem(x, y);
	m_map[y][x] = 0;
	log(actor.getName() + " picked up " + each.getName());

	m_screen.clear();
	m_screen.draw(m_map, player(), currActor());
      }
      return true;
    }
  }
  log("Can't find item");
  return false;
}

/* Have given Actor attack an Actor at another position. If no Actor
   at that position, do nothing; private function, only to be called
   by moveActor()*/
bool GameBoard::melee(Actor &attacker, int targetX, int targetY)
{
  for(Actor &each : m_actors) {
    if(each.getX() == targetX && each.getY() == targetY
       && each.getFaction() != attacker.getFaction()) {
      //Attacker attempts to attack; print result (success/fail)
      int eachHealth = each.getHealth();
      int attackerHealth = attacker.getHealth();
      if(attacker.attack(each)) {
	log(attacker.getName() + " attacked " + each.getName());
	log("Damage: " + std::to_string(each.getHealth() - eachHealth));
      }
      else {
	log(each.getName() + " attacked " + attacker.getName());
	log("Damage: " + std::to_string(attacker.getHealth() - attackerHealth));
      }

      if(!each.isAlive()) {
	m_map[targetY][targetX] = 0;
	deleteActor(targetX, targetY);
      }

      m_screen.clear();
      m_screen.draw(m_map, player(), currActor());
      return true;
    }
  }
  return false;
}

/* Performs action on given position; will move Actor there if possible,
   pickup an Item at that position, or attack a Monster at that position*/
bool GameBoard::moveActor(Actor &actor, int newX, int newY)
{
  //Check to make sure turn is respected/position exists/is within teleport range
  //and not attacking self
  if(!actor.isTurn() || !isValid(newX, newY)
     || distanceFrom(actor.getX(), actor.getY(), newX, newY) >= 5
     || (actor.getX() == newX && actor.getY() == newY)) {
    return false;
  }
  //If tile is empty, move Actor to it
  if(m_map[newY][newX] == 0) {
    return changePos(actor, newX, newY);
  }
  //If an Item is in that position, try to pick it up
  else if(m_map[newY][newX] == ItemTile) {
    return pickupItem(actor, newX, newY);
  }
  else if(m_map[newY][newX] != WallTile) {
    return melee(actor, newX, newY);
  }
  return false;
}

/* Attacks another player over a distance using a ranged weapon if equipped
   (otherwise, throws equipped item if it's not ranged or notifies user
   if no item equipped at all)*/
bool GameBoard::rangeAttack(Actor& attacker, int targetX, int targetY)
{
  //Can't attack yourself
  if(attacker.getX() == targetX && attacker.getY() == targetY) {
    return false;
  }

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
	deleteActor(targetX, targetY);
      }
      m_screen.clear();
      m_screen.draw(m_map, player(), currActor());
      return true;
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
  if(!m_screen.hasCursor()) {
    translateActor(player(), dx, dy);
  }
  else {
    m_screen.translateCursor(dx, dy);
  }
}
