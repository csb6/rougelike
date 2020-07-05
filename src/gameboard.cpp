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
GameBoard::GameBoard(Display &screen, const std::string &mapPath)
    : m_map{}, m_screen(screen)
{
    // Setup player character; player position will be set during loadMap
    const ActorTypeId player_type = m_actor_types.add('@', "Player", 3, {5, 5, 5});
    m_actors.add(player_type, "Player", {0, 0}, 10);

    loadMonsterTypes(getLocalDir() + "src/monsters.ini", m_actor_types);
    loadItemTypes(getLocalDir() + "src/melee-weapons.ini", m_item_types.melee_weapons);
    loadItemTypes(getLocalDir() + "src/ranged-weapons.ini", m_item_types.ranged_weapons);
    loadItemTypes(getLocalDir() + "src/armor.ini", m_item_types.armor);
    loadItemTypes(getLocalDir() + "src/misc-items.ini", m_item_types.misc);
    loadMap(mapPath);

    //Show initial map
    const auto[x, y] = m_actors.player_pos();
    m_screen.draw(m_map, x, y);
    m_screen.present();
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
    short row = 0;
    while(mapFile && row < MapHeight)
    {
	std::string line;
	std::getline(mapFile, line);
	short col = 0;
	for(std::string::size_type pos = 0; pos<line.size(); ++pos) {
	    if(col >= MapWidth) {
		break;
	    }
	    if(line[pos] == ',' || line[pos] == '\n' || line[pos] == '\r') {
		continue;
	    } else if(line[pos] == '0') {
		++col;
	    } else {
		//All Actors need to be in m_actors list/have char in m_map
		m_map[row][col].icon = line[pos];
		if(line[pos] == PlayerTile) {
                    // Place the player on the gameboard
                    m_map[row][col].actor_id = m_actors.player_id;
                    m_actors.position[m_actors.player_id] = {col, row};
		} else if(auto[is_actor, type_id] = m_actor_types.find(line[pos]);
                          is_actor) {
                    // Place an NPC on the gameboard
                    const auto& name = m_actor_types.name[type_id];
                    m_map[row][col].actor_id = m_actors.add(type_id, name, {col, row}, 10);
		} else if(auto[item_cat, type_id] = m_item_types.find(line[pos]);
                          item_cat != ItemCategory::None) {
                    // Place an item on the gameboard
                    m_map[row][col].item_cat = item_cat;
                    m_map[row][col].item_id = type_id;
                }
                // else if a wall/other object, then don't need to do anything
		++col;
	    }
	}
	++row;
    }
}

/* Toggles cursor on/off; calls function pointer/disables cursor when called
   and cursor active*/
void GameBoard::bindCursorMode(bool (GameBoard::*action)(ActorId, short, short))
{
    //Put cursor at player position on first keypress
    if(!m_screen.hasCursor()) {
        const auto[player_x, player_y] = m_actors.player_pos();
	m_screen.moveCursor(player_x, player_y);
    } else {
        //Execute action, passing cursor position, on second keypress
        short cursorX = m_screen.getCursorX();
	short cursorY = m_screen.getCursorY();
	if(isValid(cursorX, cursorY)) {
	    (this->*action)(m_actors.player_id, cursorX, cursorY);
	    m_screen.hideCursor();
	    m_screen.clear();
            const auto[player_x, player_y] = m_actors.player_pos();
	    m_screen.draw(m_map, player_x, player_y);
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

    auto print_item = [this](int i, ItemAmount amount, const std::string &name) {
                          m_screen.printText(0, i, std::to_string(i-1) + ". "
                                             + name + "    Amt: "
                                             + std::to_string(amount) + " ", TB_CYAN);
                      };

    const InventorySet &inventory = m_actors.inventory[m_actors.player_id];
    int i = 2;

    for(auto[item_id, amount] : inventory.melee_weapon) {
        print_item(i, amount, m_item_types.melee_weapons[item_id].name);
        ++i;
    }

    for(auto[item_id, amount] : inventory.ranged_weapon) {
        print_item(i, amount, m_item_types.ranged_weapons[item_id].name);
        ++i;
    }

    for(auto[item_id, amount] : inventory.armor) {
        print_item(i, amount, m_item_types.armor[item_id].name);
        ++i;
    }

    for(auto[item_id, amount] : inventory.misc) {
        print_item(i, amount, m_item_types.misc[item_id].name);
        ++i;
    }
}

/*Prints character sheet for the player, showing main stats*/
 /*void GameBoard::showStats()
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
    const auto[x, y] = m_actors.player_pos();
    m_screen.draw(m_map, x, y);
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
/*void GameBoard::showPlayerEquipped()
{
    std::string labels[EquipSlotCount] = {". Head: ", ". Chest: ", ". Legs: ", ". Feet: ",
                                          ". Melee: ", ". Ranged: "};
    m_screen.printText(0, 0, "Equipped Items: (ESC to exit)", TB_YELLOW);
    const auto index = m_equipment.actor_ids.index_of(m_actors.player());
    if(index < 0 || index >= m_equipment.actor_ids.size()) {
        m_screen.printText(0, 1, " Nothing Equipped", TB_CYAN);
        return;
    }
    const auto flags = m_equipment.to_bits(index);

    for(size_t i = 0; i < EquipSlotCount; ++i) {
        bool is_equipped = (flags & (1 << i)) >> i;
        if(!is_equipped) {
            m_screen.printText(0, i+1, std::to_string(i)
                               + labels[i] + "Empty", TB_CYAN);
        } else {
            //char item_type = m_equipment[index];
            const auto item_index = m_item_types.ids.index_of(item_type);
            const auto& item_name = m_item_types.names[item_index];
            m_screen.printText(0, count+1, std::to_string(count+1)
                               + labels[count] + item_name, TB_CYAN);
            
        }
        ++count;
    }
    }*/

/* Equips item in inventory into an Actor's equip slots*/
/*void GameBoard::equipPlayerItem()
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
    }*/

/* Deequips item from Actor's armor slot*/
/*void GameBoard::deequipPlayerItem()
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
    }*/

/* Moves actor from current position to another, redrawing screen
   buffer to show change. Private function, only to be called by
   moveActor()*/
void GameBoard::swapCells(short x, short y, short newX, short newY)
{
    //Note: screen doesn't visibly change until screen.present() called in main loop
    m_map[newY][newX] = m_map[y][x];
    m_map[y][x] = {};

    m_screen.clear();
    const auto[player_x, player_y] = m_actors.player_pos();
    m_screen.draw(m_map, player_x, player_y);
}

/* Picks up an Item at given coords if one can be found at that
   position; private function, only to be called by moveActor()*/
void GameBoard::pickupItem(ActorId actor, short item_x, short item_y)
{
    const ItemId item_id = m_map[item_y][item_x].item_id;
    const ItemCategory item_cat = m_map[item_y][item_x].item_cat;

    m_map[item_y][item_x] = {};
    const std::string &actor_name = m_actors.name[actor];
    switch(item_cat) {
    case ItemCategory::Melee:
        log(actor_name + " picked up "
            + m_item_types.melee_weapons[item_id].name);
        m_actors.inventory[actor].melee_weapon[item_id] += 1;
        break;
    case ItemCategory::Ranged:
        log(actor_name + " picked up "
            + m_item_types.ranged_weapons[item_id].name);
        m_actors.inventory[actor].ranged_weapon[item_id] += 1;
        break;
    case ItemCategory::Armor:
        log(actor_name + " picked up "
            + m_item_types.armor[item_id].name);
        m_actors.inventory[actor].armor[item_id] += 1;
        break;
    case ItemCategory::Misc:
        log(actor_name + " picked up "
            + m_item_types.misc[item_id].name);
        m_actors.inventory[actor].misc[item_id] += 1;
        break;
    case ItemCategory::None:
        break;
    }

    m_screen.clear();
    const auto[player_x, player_y] = m_actors.player_pos();
    m_screen.draw(m_map, player_x, player_y);
}

/* Have given Actor attack an Actor at another position. If no Actor
   at that position, do nothing; private function, only to be called
   by moveActor()*/
/*void GameBoard::melee(std::size_t attacker_index, ActorId target)
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
    }*/

/* Attacks another player over a distance using a ranged weapon if equipped
   (otherwise, throws equipped item if it's not ranged or notifies user
   if no item equipped at all)*/
  /*bool GameBoard::rangeAttack(ActorId attacker, int target_x, int target_y)
{
    //Can't attack yourself
    const auto attacker_index = m_actors.ids.index_of(attacker);
    const auto [attacker_x, attacker_y] = m_actors.positions[attacker_index];
    

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
    return false;
}*/

/* Performs action on given position; will move Actor there if possible,
   pickup an Item at that position, or attack a Monster at that position*/
bool GameBoard::moveActor(ActorId actor, short newX, short newY)
{
    const auto[x, y] = m_actors.position[actor];
    //Check to make sure position exists/is within teleport range
    //and not attacking self/or doing something to a wall
    if(!isValid(newX, newY) || distanceFrom(x, y, newX, newY) >= 5
       || (x == newX && y == newY) || m_map[newY][newX].icon == WallTile) {
	return false;
    }
    //If tile is empty, move Actor to it
    if(m_map[newY][newX].icon == '.') {
        // Update the player's position in table
        m_actors.position[actor] = {newX, newY};
        // Move the Cell to the new spot
        swapCells(x, y, newX, newY);
        return true;
    } else if(m_map[newY][newX].item_cat != ItemCategory::None) {
        //If an Item is in that position, try to pick it up
        pickupItem(actor, newX, newY);
        return true;
    }
    /*else if(m_map[newY][newX].actor_id != -1) {
        melee(index, m_map[newY][newX].actor_id);
        return true;
    }*/
    return false;
}

bool GameBoard::translateActor(ActorId actor, short dx, short dy)
{
    const auto[currX, currY] = m_actors.position[actor];
    return moveActor(actor, currX + dx, currY + dy);
}

/* If possible, moves the player into a new location, updating the player
   object, map array, screen buffer, and display to show the change*/
void GameBoard::movePlayer(short newX, short newY)
{
    moveActor(m_actors.player_id, newX, newY);
}

/* Shortcut for moving player through change in current position*/
void GameBoard::translatePlayer(short dx, short dy)
{
    if(!m_screen.hasCursor()) {
        translateActor(m_actors.player_id, dx, dy);
    } else {
	m_screen.translateCursor(dx, dy);
    }
}
