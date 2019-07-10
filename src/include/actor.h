#ifndef ACTOR_H
#define ACTOR_H
#include <vector>
#include <string>

class Item
{
 private:
  int m_id, m_weight;
  std::string m_name;
 public:
  Item(std::string name = "Item", int weight = 0);
  int getId() { return m_id; }
  int getWeight() { return m_weight; }
  std::string getName() { return m_name; }
};

class Actor
{
private:
  int m_id, m_xPos, m_yPos, m_energy;
  std::string m_name;
  bool m_isTurn;
  /*
  //Current Status - stats that change moment-to-moment from environment
  int m_health;
  bool m_isAlive;
  //Armor armorWorn[ARMOR_AMOUNT_LIMIT]; //Array of armor being worn; helmet, shirt, pants, boots
  std::vector<Item> inventory; //Contains items for player
  std::int_least16_t m_carryWeight; //Current weight of inventory
  std::int_least16_t m_maxCarryWeight;
  std::int_least16_t m_level; //General level; when upgraded, points available to boost stats
  std::int_least16_t m_levelProgress; //On scale 0-100; when 100, level-up

  //Core Skills - basic stats affecting all actions broadly
  std::int_least16_t m_strength;  // 0 - Affects damage in attacks, carry amount
  std::int_least16_t m_cunning;   // 1 - Affects likelihood of successful stealing, convincing others
  std::int_least16_t m_agility;   // 2 - Affects movement distance/turn, chance to dodge attacks
  std::int_least16_t m_education; // 3 - Affects ability to upgrade gear, strategize in battle

  //Life Skills - stats affecting specific interactions
  std::int_least16_t m_sidearmSkill;   // 0 - Skill with small guns
  std::int_least16_t m_longarmSkill;   // 1 - Skill with big guns
  std::int_least16_t m_meleeSkill;     // 2 - Skill with melee weapons
  std::int_least16_t m_vehicleSkill;   // 3 - Skill with vehicular weapons
  std::int_least16_t m_barterSkill;    // 4 - Skill affecting cost of items
  std::int_least16_t m_negotiateSkill; // 5 - Skill affecting chance of successful negotiations
  std::int_least16_t m_trapSkill;      // 6 - Skill affecting chance of traps working
  */
public:
  Actor(int x, int y, std::string name = "Monster");
  void move(int newX, int newY);
  void update() {}
  void setTurn(bool isTurn, int energy = 3);
  //Setters/Getters
  int getX() { return m_xPos; }
  int getY() { return m_yPos; }
  int getEnergy() { return m_energy; }
  std::string getName() { return m_name; }
  bool isTurn() { return m_isTurn; }
};
#endif
