#ifndef GAMEBOARD_H
#define GAMEBOARD_H
#include "display.h"
#include "actor.h"
#include <map>

class GameBoard {
//Purpose: To represent the game map/the actors/pieces on it, as well as to
//    handle user input for controlling the player
private:
  LevelMap m_map;
  Display &m_screen;
  //m_player_index is always location of player object in m_actors
  int m_player_index;
  //m_turn_index is always location of object whose turn it is in m_actors
  int m_turn_index;
  std::vector<Item> m_items;
  std::vector<Actor> m_actors;
  std::map<char,Actor> m_templates;
  void deleteItem(Item &item);
  void deleteActor(Actor &actor);
  void changePos(Actor &actor, int newX, int newY);
  void pickupItem(Actor &actor, int x, int y);
  void melee(Actor &attacker, int targetX, int targetY);
  void loadMonsterTemplates(const std::string &&path);
public:
  GameBoard(Display &screen, Actor playerCh, const std::string &mapPath);
  inline Actor& player() { return m_actors[m_player_index]; }
  inline Actor& currActor() { return m_actors[m_turn_index]; }
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
  bool rangeAttack(Actor& attacker, int targetX, int targetY);
  bool translateActor(Actor &actor, int dx, int dy);
  void movePlayer(int newX, int newY);
  void translatePlayer(int dx, int dy);
};
#endif
