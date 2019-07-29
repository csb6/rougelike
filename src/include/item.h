#ifndef ITEM_GAME_H
#define ITEM_GAME_H
#include <string>
#include <vector>

enum Armor
{
 ARMOR_HELMET,
 ARMOR_CHEST,
 ARMOR_PANTS,
 ARMOR_BOOTS,
 ARMOR_MAX
};

class Item
{
private:
  int m_xPos, m_yPos, m_id, m_weight;
  std::string m_name;
  bool m_isEquipped = false;
  //How much protection Item provides when equipped
  std::int_least16_t m_armor;
public:
  Item(int x = 0, int y = 0, std::string name = "Item", int weight = 0, int armor = 0);
  int getX() const { return m_xPos; }
  int getY() const { return m_yPos; }
  int getId() const { return m_id; }
  int getWeight() const { return m_weight; }
  std::int_least16_t getArmor() const { return m_armor; }
  std::string getName() const { return m_name; }
  bool isEquipped() const { return m_isEquipped; }
  void setEquip(bool isEquipped) { m_isEquipped = isEquipped; }
};
#endif
