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

//Chests are non-player objects (ex: box, cabinet, table, etc.) containing items
class Chest
{
private:
  //int id;
  std::string m_name;
  std::vector<Item> m_inventory;
  int m_xPos, m_yPos;
  int m_maxItems; //Max amount of items allowed in chest; -1 means no limit
public:
  Chest(int x, int y, int maxItems = -1);
  void setPos(int xPos, int yPos);
  void setMaxItems(int maxItems);
  Item& getItemAt(int index);
  bool hasSpace();
  void addItem(Item item);
  void deleteItem(Item &item);
};
#endif
