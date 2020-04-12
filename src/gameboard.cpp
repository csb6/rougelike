#include "include/gameboard.h"
#include <fstream>
#include <cmath>
#include <functional>

std::string getLocalDir();

constexpr int ItemVecDefaultSize = 5;
constexpr int ActorVecDefaultSize = 10;

/* Distance formula with truncated absolute value result*/
static int distanceFrom(int x1, int y1, int x2, int y2)
{
    return std::abs(std::sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)));
}

/* Creates a new board linking to the termbox screen; opens/loads
   the given map, and sets up in-game GUI*/
GameBoard::GameBoard(Display &screen, const std::string &mapPath,
                     ActorType player_type)
    : m_map{}, m_screen(screen)
{
    m_actor_types.add_tuple(player_type);
    loadMonsterTypes(getLocalDir() + "src/monsters.ini", m_actor_types);
    loadItemTypes(getLocalDir() + "src/items.ini", m_item_types);
    loadMap(mapPath);
    //Show initial map, centered at player's current position
    //m_screen.draw(m_map, player());
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
        const auto player_type = get_index_of(m_actor_types.id, '@');
	for(std::string::size_type pos=0; pos<line.size(); ++pos) {
	    if(col >= MapWidth) {
		break;
	    }
	    if(line[pos] == ',' || line[pos] == '\n' || line[pos] == '\r') {
		continue;
	    } else if(line[pos] == '0') {
		++col;
	    } else {
		//All Actors need to be in m_actors list/have char in m_map
		m_map[row][col] = line[pos];
		if(line[pos] == PlayerTile) {
		    m_actors.move(m_actors.player, {col, row});
		} else {
                    const auto item_it = std::lower_bound(m_item_types.id.begin(),
                                                          m_item_types.id.end(), line[pos]);
                    if(item_it != m_item_types.id.end())
                        m_items.add(line[pos], {col, row});

                    const auto actor_it = std::lower_bound(m_actor_types.id.begin(),
                                                           m_actor_types.id.end(), line[pos]);
                    if(actor_it != m_actor_types.id.end())
                        m_actors.add(line[pos], {col, row}, 3, 10);
		}
		++col;
	    }
	}
	++row;
    }
}

/* Toggles cursor on/off; calls function pointer/disables cursor when called
   and cursor active*/
/*void GameBoard::bindCursorMode(Actor &actor, bool (GameBoard::*action)(Actor&, int, int))
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
	    m_screen.draw(m_map, player()); //FIXME
	}
    }
    }*/

/* Calls update functions on all actors currently on the board,
   working backwards so removing an actor doesn't skip anything*/
/*void GameBoard::updateActors()
{
    //Check if actor with current turn is done;
    //if so, move turn to next actor, update screen
    if(!currActor().isTurn()) {
	if(--m_turn_index == -1) {
	    m_turn_index = m_actors.size() - 1;
	}
	currActor().setTurn(true);
	m_screen.clear();
	m_screen.draw(m_map, player());
    }

    int i = m_turn_index;
    do {
	m_actors[i].update(this);
	if(--i == -1) {
	    i = m_actors.size() - 1;
	}
    } while(i != m_turn_index);
    }*/

/* Displays an actor's current inventory in subscreen; ESC/any redraws closes it*/
void GameBoard::showInventory(ActorId actor)
{
    const auto actor_name = get_name(actor, m_actors, m_actor_types);
    m_screen.printText(0, 0, actor_name + "'s Inventory: (ESC to exit)", TB_YELLOW);
    m_screen.printText(0, 1, "E) Equip item, D) Deequip item", TB_YELLOW);

    const auto index = get_index_of(m_inventories.actor_id, actor);
    if(m_inventories.inventory[index].empty()) {
	m_screen.printText(2, 2, "Empty", TB_CYAN);
	return;
    }

    int i = 3;
    for(const auto item_id : m_inventories.inventory[index]) {
        const auto index = get_index_of(m_item_types.id, item_id);
        const auto name = m_item_types.name[index];
        const auto weight = m_item_types.weight[index];
        const auto armor = m_item_types.armor_value[index];
	m_screen.printText(0, i, " " + std::to_string(i+1) + ". " + name
			   + " - Weight: " + std::to_string(weight)
			   + ", Armor: " + std::to_string(armor), TB_CYAN);
	++i;
    }
}

/*Prints character sheet for an Actor, showing main stats*/
void GameBoard::showStats(ActorId actor)
{
    const auto actor_index = get_index_of(m_actors.id, actor);
    const auto type_index = get_index_of(m_actor_types.id, m_actors.actor_type[actor_index]);
    const auto inv_index = get_index_of(m_inventories.actor_id, actor);

    m_screen.printText(0, 0, m_actor_types.name[type_index]
                       + "'s Character Sheet: (ESC to exit)", TB_YELLOW);
    m_screen.printText(0, 1, "Health: " + std::to_string(m_actors.health[actor_index]),
                                                         TB_CYAN);
    m_screen.printText(0, 2, "Carry Weight: "
                       + std::to_string(m_inventories.carry[inv_index]),
                       TB_CYAN);
    m_screen.printText(0, 3, "Carry Capacity: "
                       + std::to_string(m_actor_types.max_carry[type_index]),
                       TB_CYAN);
    m_screen.printText(0, 6, "Strength: "
                       + std::to_string(m_actor_types.strength[actor_index]),
                       TB_CYAN);
}

/* Show list of equipment slots, showing which items in which slots/which
   slots are empty*/
/*void GameBoard::showEquipped(Actor &actor)
{
    std::string labels[EQUIP_MAX] = {". Head: ", ". Chest: ", ". Legs: ", ". Feet: ",
				     ". Melee: ", ". Ranged: "};
    m_screen.printText(0, 0, actor.getName() + "'s Equipped Items: (ESC to exit)", TB_YELLOW);
    for(int i=0; i<EQUIP_MAX; ++i) {
	Item *item = actor.getEquipped(i);
	if(item == nullptr) {
	    m_screen.printText(0, i+1, std::to_string(i+1) + labels[i] + "Empty", TB_CYAN);
	} else {
	    m_screen.printText(0, i+1, std::to_string(i+1) + labels[i] + item->getName(), TB_CYAN);
	}
    }
    }*/

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
    auto[playerX, playerY] = m_actors.position[m_actors.player_index];
    m_screen.draw(m_map, playerX, playerY);
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

/* Equips item in inventory into an Actor's equip slots*/
/*void GameBoard::equipItem(Actor &actor)
{
    int index = m_screen.input("Enter item #: ") - 1;
    int pos = m_screen.input("Enter equip position [1-"
			     + std::to_string(EQUIP_MAX) + "]: ", 0, 1) - 1;
    actor.equipItem(index, pos);

    m_screen.clear();
    m_screen.draw(m_map, player());
    }*/

/* Deequips item from Actor's armor slot*/
/*void GameBoard::deequipItem(Actor &actor)
{
    int pos = m_screen.input("Enter equip position [1-"
			     + std::to_string(EQUIP_MAX) + "]: ", 0, 1) - 1;
    actor.deequipItem(pos);

    m_screen.clear();
    m_screen.draw(m_map, player());
}*/

/* Moves actor from current position to another, redrawing screen
   buffer to show change. Private function, only to be called by
   moveActor()*/
bool GameBoard::changePos(ActorId actor, int newX, int newY)
{
    //Note: screen doesn't visibly change until screen.present() called in main loop
    const auto index = get_index_of(m_actors.id, actor);
    const auto old_ch = m_map[m_actors.position[index][1]][m_actors.position[index][0]];
    m_map[m_actors.position[index][1]][m_actors.position[index][0]] = 0;
    m_actors.position[index][0] = newX;
    m_actors.position[index][1] = newY;
    m_map[newY][newX] = old_ch;

    m_screen.clear();
    //m_screen.draw(m_map, player()); //FIXME
    return true;
}

/* Picks up an Item at given coords if one can be found at that
   position; private function, only to be called by moveActor()*/
bool GameBoard::pickupItem(ActorId actor, int x, int y)
{
    const auto item_index = get_index_of(m_items.id, item);
    const auto actor_index = get_index_of(m_actors.id, actor);
    const auto inv_index = get_index_of(m_inventories.actor_id, actor);
    const auto itype_index = get_index_of(m_item_types.id, m_items.type[item_index]);
    const auto atype_index = get_index_of(m_actor_types.id, m_actors.actor_type[actor_index]);
    const Weight weight = m_item_types.weight[itype_index];
    const Weight max_carry = m_actor_types.max_carry[atype_index];
    if(m_inventories.carry[inv_index] + weight <= max_carry) {
        // Pick up the item
        m_inventories.inventory[inv_index].push_back(item);
        const auto[x, y] = m_items.position[item_index];
        m_map[y][x] = 0;
        const std::string actor_name = m_actor_types.name[atype_index];
        const std::string item_name = m_item_types.name[itype_index];
        log(actor_name + " picked up " + item_name);

        m_screen.clear();
        //m_screen.draw(m_map, player()); //FIXME
        return true;
    } else {
        log("Can't find item");
        return false;
    }
}

/* Have given Actor attack an Actor at another position. If no Actor
   at that position, do nothing; private function, only to be called
   by moveActor()*/
/*bool GameBoard::melee(Actor &attacker, int targetX, int targetY)
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
	    } else {
		log(each.getName() + " attacked " + attacker.getName());
		log("Damage: " + std::to_string(attacker.getHealth() - attackerHealth));
	    }

	    if(!each.isAlive()) {
		m_map[targetY][targetX] = 0;
		deleteActor(targetX, targetY);
	    }

	    m_screen.clear();
	    m_screen.draw(m_map, player());
	    return true;
	}
    }
    return false;
    }*/

/* Performs action on given position; will move Actor there if possible,
   pickup an Item at that position, or attack a Monster at that position*/
bool GameBoard::moveActor(ActorId actor, int newX, int newY)
{
    //Check to make sure turn is respected/position exists/is within teleport range
    //and not attacking self
    const auto actor_index = get_index_of(m_actors.id, actor);
    const auto[x, y] = m_actors.position[actor_index];
    if(!isValid(newX, newY) || actor_index != m_actors.turn_index
       || distanceFrom(x, y, newX, newY) >= 5
       || (x == newX && y == newY)) {
	return false;
    }
    //If tile is empty, move Actor to it
    if(m_map[newY][newX] == 0) {
	return changePos(actor, newX, newY);
    }
    //If an Item is in that position, try to pick it up
    else if(m_map[newY][newX] == ItemTile) {
	return pickupItem(actor, newX, newY);
        }/* else if(m_map[newY][newX] != WallTile) {
	return melee(actor, newX, newY);
        }*/
    return false;
}

/* Attacks another player over a distance using a ranged weapon if equipped
   (otherwise, throws equipped item if it's not ranged or notifies user
   if no item equipped at all)*/
/*bool GameBoard::rangeAttack(Actor& attacker, int targetX, int targetY)
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
	    } else {
		//Throw item
		log("Item thrown");
	    }

	    if(!each.isAlive()) {
		m_map[targetY][targetX] = 0;
		deleteActor(targetX, targetY);
	    }
	    m_screen.clear();
	    m_screen.draw(m_map, player());
	    return true;
	}
    }
    return false;
    }*/

/*bool GameBoard::translateActor(Actor &actor, int dx, int dy)
{
    return moveActor(actor, actor.getX() + dx, actor.getY() + dy);
}*/

/* If possible, moves the player into a new location, updating the player
   object, map array, screen buffer, and display to show the change*/
void GameBoard::movePlayer(int newX, int newY)
{
    const auto player_id = m_actors.id[m_actors.player_index];
    moveActor(player_id, newX, newY);
}

/* Shortcut for moving player through change in current position*/
/*void GameBoard::translatePlayer(int dx, int dy)
{
    if(!m_screen.hasCursor()) {
	translateActor(player(), dx, dy);
    } else {
	m_screen.translateCursor(dx, dy);
    }
    }*/
