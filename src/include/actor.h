#ifndef ACTOR_H
#define ACTOR_H
#include "item.h"

class GameBoard;

const int RNGUpperLimit = 100;
const int SkillAmount = 11; //Number of skills (e.g. strengh, agility, etc.)
const int MaxInitPoints = 25; //Total pts doled out at character creation

class Actor {
private:
  int /*m_id,*/ m_xPos, m_yPos, m_energy;
  char m_ch; //character representing this Actor on board
  std::string m_name;
  bool m_isTurn;

  //Current Status - stats that change moment-to-moment from environment
  std::int_least16_t m_health = 15;
  Item m_equipment[EQUIP_MAX]; //Items being worn; helmet, shirt, pants, boots, weapons
  std::vector<Item> m_inventory; //Contains items for player
public:
  Actor(int x = 0, int y = 0, std::string name = "Monster", char ch = 'M');
  bool operator==(const Actor &other) const;
  void move(int newX, int newY);
  bool attack(Actor &target);
  void update(GameBoard *board);
  void setTurn(bool isTurn, int energy = 3);
  bool canCarry(int itemWeight) const;
  Item& getItemAt(int index);
  void addItem(Item &item);
  void deleteItem(Item &item);
  void equipItem(int index, int position);
  void deequipItem(int position);
  Item* getEquipped(int index);
  void addHealth(int amount);
  //Setters/Getters
  int getX() const { return m_xPos; }
  int getY() const { return m_yPos; }
  int getEnergy() const { return m_energy; }
  int getHealth() const { return m_health; }
  char getCh() const { return m_ch; }
  void setCh(char ch) { m_ch = ch; }
  void setName(std::string name) { m_name = name; }
  std::string getName() const { return m_name; }
  bool isTurn() const { return m_isTurn; }
  bool isAlive() const { return m_health > 0; }
  int getInventorySize() const { return m_inventory.size(); }
  std::int_least16_t m_carryWeight = 0; //Current weight of inventory
  std::int_least16_t m_maxCarryWeight = 20;
  std::int_least16_t m_level = 1; //General level; when upgraded, points available to boost stats
  std::int_least16_t m_levelProgress = 0; //On scale 0-100; when 100, level-up

  //Core Skills - basic stats affecting all actions broadly
  std::int_least16_t m_strength = 0;// 0 - Affects damage in attacks, carry amount
  std::int_least16_t m_cunning = 0; // 1 - Affects likelihood of successful stealing, convincing others
  std::int_least16_t m_agility = 0; // 2 - Affects movement distance/turn, chance to dodge attacks
  std::int_least16_t m_education = 0; // 3 - Affects ability to upgrade gear, strategize in battle

  //Life Skills - stats affecting specific interactions
  std::int_least16_t m_sidearmSkill = 0;   // 0 - Skill with small guns
  std::int_least16_t m_longarmSkill = 0;   // 1 - Skill with big guns
  std::int_least16_t m_meleeSkill = 0;     // 2 - Skill with melee weapons
  std::int_least16_t m_vehicleSkill = 0;   // 3 - Skill with vehicular weapons
  std::int_least16_t m_barterSkill = 0;    // 4 - Skill affecting cost of items
  std::int_least16_t m_negotiateSkill = 0; // 5 - Skill affecting chance of successful negotiations
  std::int_least16_t m_trapSkill = 0;      // 6 - Skill affecting chance of traps working
};
#endif
