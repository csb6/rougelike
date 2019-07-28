#ifndef GAMEBOARD_H
#define GAMEBOARD_H
#include "display.h"
#include "actor.h"
class GameBoard
//Purpose: To represent the game map/the actors/pieces on it, as well as to
//    handle user input for controlling the player
{
private:
  bool &m_running;
  LevelMap m_map;
  Display m_screen;
  //m_player_index is always location of player object in m_actors
  int m_player_index;
  //m_turn_index is always location of object whose turn it is in m_actors
  int m_turn_index;
  std::vector<Item> m_items;
  std::vector<Actor> m_actors;
  inline Actor& player() { return m_actors[m_player_index]; }
  inline Actor& currActor() { return m_actors[m_turn_index]; }
  void deleteItem(Item &item);
  void deleteActor(Actor &actor);
  void changePos(Actor &actor, int newX, int newY);
  void pickupItem(Actor &actor, int x, int y);
  void attack(Actor &attacker, int targetX, int targetY);
public:
  GameBoard(bool &running, const std::string &mapPath);
  void loadMap(const std::string &path);
  bool processInput();
  void updateActors();
  void showInventory(Actor &actor);
  void showStats(Actor &actor);
  void showEquipped(Actor &actor);
  void log(const std::string &text);
  void present();
  bool isValid(int x, int y);
  bool moveActor(Actor &actor, int newX, int newY);
  bool translateActor(Actor &actor, int dx, int dy);
  void movePlayer(int newX, int newY);
  void translatePlayer(int dx, int dy);
};
#endif
