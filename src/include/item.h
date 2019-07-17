#ifndef ITEM_GAME_H
#define ITEM_GAME_H
#include <string>
#include <vector>

class Item
{
private:
  int m_xPos, m_yPos, m_id, m_weight;
  std::string m_name;
public:
  Item(int x, int y, std::string name = "Item", int weight = 0);
  int getX() { return m_xPos; }
  int getY() { return m_yPos; }
  int getId() { return m_id; }
  int getWeight() { return m_weight; }
  std::string getName() { return m_name; }
};
#endif
