#ifndef ITEM_GAME_H
#define ITEM_GAME_H
#include <string>
#include <vector>

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
#endif
