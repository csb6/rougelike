#ifndef GAMEBOARD_H
#define GAMEBOARD_H
#include "display.h"
#include "actor.h"
#include "template.h"
#include <array>

class GameBoard {
// Purpose: To represent the game map/the actors/pieces on it
private:
    std::array<std::array<Cell, MapWidth>, MapHeight> m_map;
    Display &m_screen;
    ActorTypeTable m_actor_types;
    ItemTypeTable m_item_types;
    ActorTable m_actors;
    //ActorInventoryTable m_inventories;
    //ActorEquipmentTable m_equipment;
    void swapCells(short x, short y, short newX, short newY);
    //bool pickupItem(ActorId actor, std::size_t actor_index, int item_x, int item_y);
    //void melee(std::size_t attacker_index, ActorId target);
public:
    GameBoard(Display &screen, const std::string &mapPath);
    void loadMap(const std::string &path);
    //void bindCursorMode(bool (GameBoard::*action)(ActorId, int, int));
    //void updateActors();
    /*void showInventory();
    void showStats();
    void showPlayerEquipped();
    void equipPlayerItem();
    void deequipPlayerItem();*/
    void log(const std::string &);
    void redraw();
    void present();
    static bool isValid(int x, int y);
    //bool rangeAttack(ActorId attacker, int target_x, int target_y);
    bool moveActor(ActorId actor, short newX, short newY);
    bool translateActor(ActorId actor, short dx, short dy);
    void movePlayer(short newX, short newY);
    void translatePlayer(short dx, short dy);
};
#endif
