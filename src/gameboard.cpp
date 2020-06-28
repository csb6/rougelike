#include "include/gameboard.h"
#include <fstream>
#include <cmath>

std::string getLocalDir();

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
    // Add player type
    const auto player_type_id = m_actor_types.add(player_type);
    // Add player
    m_actors.add(player_type_id, {0, 0}, {3}, {10});
    m_actors.player_index = m_actors.ids.size() - 1;
    // (player position will be set during loadMap)

    loadMonsterTypes(getLocalDir() + "src/monsters.ini", m_actor_types);
    loadItemTypes(getLocalDir() + "src/items.ini", m_item_types);
    loadMap(mapPath);
    //Show initial map
    m_screen.draw(m_map, m_actors.player_x(), m_actors.player_y());
}

/* Fills 2d array with tiles from given map file, instantiating
   new Actors/other entities as needed in their correct positions*/
void GameBoard::loadMap(const std::string &path)
{
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
	    } else if(line[pos] == '0') {
		++col;
	    } else {
		//All Actors need to be in m_actors list/have char in m_map
		m_map[row][col].ch = line[pos];
		if(line[pos] == PlayerTile) {
                    // Set player position
                    m_map[row][col].actor_id = m_actors.player();
                    m_actors.positions[m_actors.player_index] = {col, row};
		} else if(m_actor_types.contains(line[pos])) {
                    // Add and set position of a non-player actor
                    m_map[row][col].actor_id = m_actors.add(line[pos], {col, row}, {3}, {10});
		}
                // else if a item/other object, then don't need to do anything
		++col;
	    }
	}
	++row;
    }
}

/* Toggles cursor on/off; calls function pointer/disables cursor when called
   and cursor active*/
void GameBoard::bindCursorMode(bool (GameBoard::*action)(ActorId, int, int))
{
    //Put cursor at player position on first keypress
    if(!m_screen.hasCursor()) {
	m_screen.moveCursor(m_actors.player_x(), m_actors.player_y());
    } else {
        //Execute action, passing cursor position, on second keypress
	int cursorX = m_screen.getCursorX();
	int cursorY = m_screen.getCursorY();
	if(isValid(cursorX, cursorY)) {
	    (this->*action)(m_actors.player(), cursorX, cursorY);
	    m_screen.hideCursor();
	    m_screen.clear();
	    m_screen.draw(m_map, m_actors.player_x(), m_actors.player_y());
	}
    }
}

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
void GameBoard::showInventory()
{
    m_screen.printText(0, 0, "Inventory: (ESC to exit)", TB_YELLOW);
    m_screen.printText(0, 1, "E) Equip item, D) Deequip item", TB_YELLOW);

    const auto player_id = m_actors.player();
    std::size_t inv_index = m_inventories.actor_ids.index_of(player_id);
    if(inv_index >= m_inventories.actor_ids.size()
       || m_inventories.actor_ids[inv_index] != player_id) {
	m_screen.printText(0, 2, "Empty", TB_CYAN);
	return;
    }

    int i = 2;
    do {
        const auto item_type_id = m_inventories.items[inv_index];
        const auto amount = m_inventories.amounts[inv_index];
        if(amount <= 0) {
            // Means that the item is currently equipped, so not in
            // regular inventory, so shouldn't be shown
            ++inv_index;
            continue;
        }
        const auto item_type_index = m_item_types.ids.index_of(item_type_id);
	m_screen.printText(0, i, std::to_string(i-1) + ". "
                           + m_item_types.names[item_type_index] + "      Amount: "
                           + std::to_string(amount) + "   ", TB_CYAN);
	++inv_index;
        ++i;
    } while(inv_index < m_inventories.items.size()
            && m_inventories.actor_ids[inv_index] == player_id);
}

/*Prints character sheet for the player, showing main stats*/
void GameBoard::showStats()
{
    const auto player_index = m_actors.player_index;

    m_screen.printText(0, 0, "Character Sheet: (ESC to exit)", TB_YELLOW);
    m_screen.printText(0, 1, "Health: "
                       + std::to_string(m_actors.healths[player_index].v)
                        + "         ", TB_CYAN);
    m_screen.printText(0, 2, "Carry Weight: "
                       + std::to_string(m_actors.carries[player_index].v)
                        + "    ", TB_CYAN);
    m_screen.printText(0, 3, "Carry Capacity: "
                       + std::to_string(m_actor_types.max_carries[player_index].v)
                       + " ", TB_CYAN);
    m_screen.printText(0, 4, "Strength: "
                       + std::to_string(m_actor_types.strengths[player_index].v)
                        + "          ", TB_CYAN);
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
    m_screen.draw(m_map, m_actors.player_x(), m_actors.player_y());
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

/* Show list of equipment slots, showing which items in which slots/which
   slots are empty*/
void GameBoard::showPlayerEquipped()
{
    std::string labels[EquipSlotCount] = {". Head: ", ". Chest: ", ". Legs: ", ". Feet: ",
                                          /*". Melee: ", ". Ranged: "*/};
    m_screen.printText(0, 0, "Equipped Items: (ESC to exit)", TB_YELLOW);
    const auto base_index = m_equipment.actor_ids.index_of(m_actors.player());
    if(base_index < 0 || base_index >= m_equipment.actor_ids.size()) {
        m_screen.printText(0, 1, " Nothing Equipped", TB_CYAN);
        return;
    }
    int count = 0;
    for(size_t i = base_index; i < base_index + EquipSlotCount; ++i) {
        char item_type = m_equipment.equipments[i];
        if(item_type == -1) {
            m_screen.printText(0, count+1, std::to_string(count+1)
                               + labels[count] + "Empty", TB_CYAN);
        } else {
            const auto item_index = m_item_types.ids.index_of(item_type);
            const auto& item_name = m_item_types.names[item_index];
            m_screen.printText(0, count+1, std::to_string(count+1)
                               + labels[count] + item_name, TB_CYAN);
            
        }
        ++count;
    }
}

/* Equips item in inventory into an Actor's equip slots*/
void GameBoard::equipPlayerItem()
{
    int item_num = m_screen.input("Enter item #: ") - 1;
    int equip_index = m_screen.input("Enter equip position [1-"
                                     + std::to_string(EquipSlotCount) + "]: ",
                                     0, 1) - 1;
    if(equip_index < 0 || equip_index >= EquipSlotCount) {
        log("Can't equip");
        m_screen.clear();
        m_screen.draw(m_map, m_actors.player_x(), m_actors.player_y());
        return;
    }
    const auto base_index = m_inventories.actor_ids.index_of(m_actors.player());
    const auto item_index = base_index + item_num;
    if(item_index < 0 || item_index >= m_inventories.items.size()
        || m_inventories.amounts[item_index] <= 0) {
        log("Can't equip");
        m_screen.clear();
        m_screen.draw(m_map, m_actors.player_x(), m_actors.player_y());
        return;
    }
    m_equipment.equip(m_actors.player(), equip_index, m_inventories.items[item_index]);
    // Remove equipped item from player's normal inventory
    m_inventories.amounts[item_index] -= 1;

    m_screen.clear();
    m_screen.draw(m_map, m_actors.player_x(), m_actors.player_y());
}

/* Deequips item from Actor's armor slot*/
void GameBoard::deequipPlayerItem()
{
    int equip_index = m_screen.input("Enter equip position [1-"
                                     + std::to_string(EquipSlotCount) + "]: ",
                                     0, 1) - 1;
    if(equip_index < 0 || equip_index >= EquipSlotCount) {
        log("Can't deequip");
        m_screen.clear();
        m_screen.draw(m_map, m_actors.player_x(), m_actors.player_y());
        return;
    }

    const auto player_id = m_actors.player();
    const char item_type = m_equipment.deequip(player_id, equip_index);
    if(item_type == -1) {
        log("Can't deequip");
        m_screen.clear();
        m_screen.draw(m_map, m_actors.player_x(), m_actors.player_y());
        return;
    }

    const auto base_index = m_inventories.actor_ids.index_of(m_actors.player());
    for(size_t i = base_index; i < m_inventories.items.size(); ++i) {
        if(m_inventories.actor_ids[i] != player_id) {
            log("Can't deequip");
            m_screen.clear();
            m_screen.draw(m_map, m_actors.player_x(), m_actors.player_y());
            return;
        } else if(m_inventories.items[i] == item_type) {
            // Put the item back in the normal inventory
            m_inventories.amounts[i] += 1;
            m_screen.clear();
            m_screen.draw(m_map, m_actors.player_x(), m_actors.player_y());
            return;
        }
    }

    log("Can't deequip");
    m_screen.clear();
    m_screen.draw(m_map, m_actors.player_x(), m_actors.player_y());
}

/* Moves actor from current position to another, redrawing screen
   buffer to show change. Private function, only to be called by
   moveActor()*/
void GameBoard::swapCells(int x, int y, int newX, int newY)
{
    //Note: screen doesn't visibly change until screen.present() called in main loop
    m_map[newY][newX] = m_map[y][x];
    m_map[y][x] = {};

    m_screen.clear();
    m_screen.draw(m_map, m_actors.player_x(), m_actors.player_y());
}

/* Picks up an Item at given coords if one can be found at that
   position; private function, only to be called by moveActor()*/
bool GameBoard::pickupItem(ActorId actor, std::size_t actor_index,
                           int item_x, int item_y)
{
    const char item_type = m_map[item_y][item_x].ch;
    const auto item_type_index = m_item_types.ids.index_of(item_type);
    const Weight item_weight = m_item_types.weights[item_type_index];
    const char actor_type = m_actors.types[actor_index];
    const auto actor_type_index = m_actor_types.ids.index_of(actor_type);
    const Weight max_carry{m_actor_types.max_carries[actor_type_index]};

    if(m_actors.carries[actor_index] + item_weight <= max_carry) {
        // Pick up the item
        m_actors.carries[actor_index] += item_weight;
        m_inventories.add(actor, item_type);
        m_map[item_y][item_x].ch = 0;
        const std::string actor_name = m_actor_types.names[actor_type_index];
        const std::string item_name = m_item_types.names[item_type_index];
        log(actor_name + " picked up " + item_name);

        m_screen.clear();
        m_screen.draw(m_map, m_actors.player_x(), m_actors.player_y());
        return true;
    } else {
        log("Can't carry");
        m_screen.clear();
        m_screen.draw(m_map, m_actors.player_x(), m_actors.player_y());
        return false;
    }
}

/* Have given Actor attack an Actor at another position. If no Actor
   at that position, do nothing; private function, only to be called
   by moveActor()*/
void GameBoard::melee(std::size_t attacker_index, ActorId target)
{
    const std::size_t target_index = m_actors.ids.index_of(target);
    const auto attacker_type = m_actors.types[attacker_index];
    const auto target_type = m_actors.types[target_index];
    const std::size_t attacker_type_index = m_actor_types.ids.index_of(attacker_type);
    const std::size_t target_type_index = m_actor_types.ids.index_of(target_type);
    const auto attacker_name = m_actor_types.names[attacker_type_index];
    const auto target_name = m_actor_types.names[target_type_index];

    if(m_actor_types.successful_attack(attacker_type_index, target_type_index)) {
        if(--m_actors.healths[target_index] > 0) {
            log(attacker_name + " attacked " + target_name);
        } else {
            // TODO: delete/deactivate actors when they die
            log(target_name + " died");
        }
    } else {
        if(--m_actors.healths[attacker_index] > 0) {
            log(target_name + " attacked " + attacker_name);
        } else {
            log(attacker_name + " died");
        }
    }

    m_screen.clear();
    m_screen.draw(m_map, m_actors.player_x(), m_actors.player_y());
}

/* Performs action on given position; will move Actor there if possible,
   pickup an Item at that position, or attack a Monster at that position*/
bool GameBoard::moveActor(ActorId actor, int newX, int newY)
{
    const auto index = m_actors.ids.index_of(actor);
    const auto[x, y] = m_actors.positions[index];
    //Check to make sure position exists/is within teleport range
    //and not attacking self/or doing something to a wall
    if(!isValid(newX, newY) || distanceFrom(x, y, newX, newY) >= 5
       || (x == newX && y == newY) || m_map[newY][newX].ch == WallTile) {
	return false;
    }
    //If tile is empty, move Actor to it
    if(m_map[newY][newX].ch == 0) {
        // Update the player's position in table
        m_actors.positions[index] = {newX, newY};
        // Move the Cell to the new spot
        swapCells(x, y, newX, newY);
        return true;
    } else if(m_map[newY][newX].actor_id != -1) {
        melee(index, m_map[newY][newX].actor_id);
        return true;
    } else {
        //If an Item is in that position, try to pick it up
	return pickupItem(actor, index, newX, newY);
    }
}

/* Attacks another player over a distance using a ranged weapon if equipped
   (otherwise, throws equipped item if it's not ranged or notifies user
   if no item equipped at all)*/
bool GameBoard::rangeAttack(ActorId attacker, int target_x, int target_y)
{
    //Can't attack yourself
    const auto attacker_index = m_actors.ids.index_of(attacker);
    const auto [attacker_x, attacker_y] = m_actors.positions[attacker_index];
    if(target_x == attacker_x && target_y == attacker_y) {
        return false;
    }

    return false;
    /*
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
    return false;*/
}

/*bool GameBoard::translateActor(Actor &actor, int dx, int dy)
{
    return moveActor(actor, actor.getX() + dx, actor.getY() + dy);
}*/

/* If possible, moves the player into a new location, updating the player
   object, map array, screen buffer, and display to show the change*/
void GameBoard::movePlayer(int newX, int newY)
{
    moveActor(m_actors.player(), newX, newY);
}

/* Shortcut for moving player through change in current position*/
void GameBoard::translatePlayer(int dx, int dy)
{
    if(!m_screen.hasCursor()) {
        movePlayer(m_actors.player_x() + dx, m_actors.player_y() + dy);
    } else {
	m_screen.translateCursor(dx, dy);
    }
}
