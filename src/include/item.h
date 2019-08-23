#ifndef ITEM_GAME_H
#define ITEM_GAME_H
#include <string>
#include <vector>

enum Equipment {
 ARMOR_HELMET,
 ARMOR_CHEST,
 ARMOR_PANTS,
 ARMOR_BOOTS,
 MELEE_WEAPON,
 RANGE_WEAPON,
 EQUIP_MAX
};

const int ARMOR_MAX = ARMOR_BOOTS + 1;

class Item {
private:
  int m_xPos, m_yPos, m_id, m_weight;
  std::string m_name;
  bool m_isEquipped = false;
  bool m_isRanged = false; //If this is a ranged weapon
  bool m_isMelee = false;
  //How much protection Item provides when equipped
  std::int_least16_t m_armor, m_attack;
public:
  Item(int x = 0, int y = 0, std::string name = "Item", int weight = 0,
       std::int_least16_t armor = 0, std::int_least16_t attack = 0);
  bool operator==(const Item &other) const;
  int getX() const { return m_xPos; }
  int getY() const { return m_yPos; }
  int getId() const { return m_id; }
  int getWeight() const { return m_weight; }
  std::int_least16_t getArmor() const { return m_armor; }
  std::int_least16_t getAttack() const { return m_attack; }
  std::string getName() const { return m_name; }
  bool isEquipped() const { return m_isEquipped; }
  bool isRanged() const { return m_isRanged; }
  bool isMelee() const { return m_isMelee; }
  void setEquip(bool isEquipped) { m_isEquipped = isEquipped; }
  void setRanged(bool isRanged) { m_isRanged = isRanged; }
};
#endif
