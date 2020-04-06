#ifndef GAMEBOARD_H
#define GAMEBOARD_H
#include "display.h"
#include "actor.h"
#include "template.h"
#include <map>

class GameBoard {
//Purpose: To represent the game map/the actors/pieces on it, as well as to
//    handle user input for controlling the player
private:
    //LevelMap m_map;
    ActorId m_map[MapWidth][MapHeight];
    Display &m_screen;
    ActorTable m_actors;
    ActorTypeTable m_actor_types;
    ActorInventoryTable m_inventories;
    ItemTable m_items;
    ItemTypeTable m_item_types;
    bool changePos(Actor &actor, int newX, int newY);
    bool pickupItem(Actor &actor, int x, int y);
    //bool melee(Actor &attacker, int targetX, int targetY);
public:
    GameBoard(Display &screen, Actor playerCh, const std::string &mapPath);
    //inline Actor& player() { return m_actors[m_player_index]; }
    //inline Actor& currActor() { return m_actors[m_turn_index]; }
    void loadMap(const std::string &path);
    void bindCursorMode(Actor &actor, bool (GameBoard::*action)(Actor&, int, int));
    void updateActors();
    void showInventory(Actor &actor);
    void showStats(Actor &actor);
    void showEquipped(Actor &actor);
    void equipItem(Actor &actor);
    void deequipItem(Actor &actor);
    void log(const std::string &text);
    void redraw();
    void present();
    bool isValid(int x, int y) const;
    bool moveActor(Actor &actor, int newX, int newY);
    //bool rangeAttack(Actor& attacker, int targetX, int targetY);
    bool translateActor(Actor &actor, int dx, int dy);
    void movePlayer(int newX, int newY);
    void translatePlayer(int dx, int dy);
};
#endif
