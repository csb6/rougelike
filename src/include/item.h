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
public:
  Item(int x = 0, int y = 0, std::string name = "Item", int weight = 0);
  int getX() { return m_xPos; }
  int getY() { return m_yPos; }
  int getId() { return m_id; }
  int getWeight() { return m_weight; }
  std::string getName() { return m_name; }
  bool isEquipped() { return m_isEquipped; }
  void setEquip(bool isEquipped) { m_isEquipped = isEquipped; }
};
#endif
