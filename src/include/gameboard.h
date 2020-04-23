#ifndef GAMEBOARD_H
#define GAMEBOARD_H
#include "display.h"
#include "actor.h"
#include "template.h"

struct Cell {
    char ch = 0;
    ActorId actor_id{0};
};

class GameBoard {
//Purpose: To represent the game map/the actors/pieces on it, as well as to
//    handle user input for controlling the player
private:
    Cell m_map[MapHeight][MapWidth];
    Display &m_screen;
    ActorTypeTable m_actor_types;
    ItemTypeTable m_item_types;
    ActorTable m_actors;
    ActorInventoryTable m_inventories;
    void changePos(int x, int y, int newX, int newY);
    //bool pickupItem(ActorId actor, int x, int y);
    //bool melee(Actor &attacker, int targetX, int targetY);
public:
    GameBoard(Display &screen, const std::string &mapPath,
              ActorType player_type);
    void loadMap(const std::string &path);
    //void bindCursorMode(Actor &actor, bool (GameBoard::*action)(Actor&, int, int));
    //void updateActors();
    void showInventory(int x, int y);
    //void showStats(int x, int y);
    //void showEquipped(Actor &actor);
    //void equipItem(Actor &actor);
    //void deequipItem(Actor &actor);
    void log(const std::string &text);
    void redraw();
    void present();
    bool isValid(int x, int y) const;
    bool moveActor(int x, int y, int newX, int newY);
    //bool rangeAttack(Actor& attacker, int targetX, int targetY);
    //bool translateActor(Actor &actor, int dx, int dy);
    void movePlayer(int newX, int newY);
    //void translatePlayer(int dx, int dy);
};
#endif
