#ifndef GAMEBOARD_H
#define GAMEBOARD_H
#include "display.h"
#include "actor.h"
#include "template.h"
#include <array>

class GameBoard {
//Purpose: To represent the game map/the actors/pieces on it, as well as to
//    handle user input for controlling the player
private:
    std::array<std::array<Cell, MapWidth>, MapHeight> m_map;
    Display &m_screen;
    ActorTypeTable m_actor_types;
    ItemTypeTable m_item_types;
    ActorTable m_actors;
    ActorInventoryTable m_inventories;
    void swapCell(int x, int y, int newX, int newY);
    bool pickupItem(ActorId actor, std::size_t actor_index, int item_x, int item_y);
    //bool melee(Actor &attacker, int targetX, int targetY);
public:
    GameBoard(Display &screen, const std::string &mapPath,
              ActorType player_type);
    void loadMap(const std::string &path);
    void bindCursorMode(int actor_x, int actor_y, bool (GameBoard::*action)(int, int));
    //void updateActors();
    void showInventory();
    void showStats();
    //void showEquipped(Actor &actor);
    //void equipItem(Actor &actor);
    //void deequipItem(Actor &actor);
    void log(const std::string &text);
    void redraw();
    void present();
    bool isValid(int x, int y) const;
    bool moveActor(ActorId actor, int newX, int newY);
    //bool rangeAttack(Actor& attacker, int targetX, int targetY);
    //bool translateActor(ActorId actor, int dx, int dy);
    void movePlayer(int newX, int newY);
    void translatePlayer(int dx, int dy);
};
#endif
