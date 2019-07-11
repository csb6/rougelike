#include "include/item.h"
#include <utility>

//Global ID generator - should be included in all .cpp files
int generateId()
{
  static int id = -1;
  ++id;
  return id;
}

Item::Item(std::string name, int weight) : m_id(generateId()),
					   m_weight(weight), m_name(name)
{

}


Chest::Chest(int x, int y, int maxItems) : m_xPos(x), m_yPos(y),
					   m_maxItems(maxItems)
{

}

void Chest::setPos(int newX, int newY)
{
  m_xPos = newX;
  m_yPos = newY;
}

void Chest::setMaxItems(int newMaxItems)
{
  m_maxItems = newMaxItems;
}

Item& Chest::getItemAt(int index)
{
  using index_t = std::vector<Item>::size_type;
  //Potentially dangerous, but don't think any chest will have enough items to cause overflow
  index_t v_index = static_cast<index_t>(index);
  return m_inventory.at(v_index);
}

bool Chest::hasSpace()
{
  using index_t = std::vector<Item>::size_type;
  return m_inventory.size() < static_cast<index_t>(m_maxItems);
}

void Chest::addItem(Item item)
{
  if(m_maxItems == -1)
    //maxItems == -1 means chest has unlimited space
    m_inventory.push_back(item);
  //Potentially dangerous, but don't think any chest will have enough items to cause overflow
  else if(m_maxItems >= static_cast<int>(m_inventory.size())+1)
    m_inventory.push_back(item);
}

void Chest::deleteItem(Item &item)
{
  if(m_inventory.size() > 0)
  {
    //Move matching item to back, delete it after transfer
    std::swap(item, m_inventory.back());
    m_inventory.pop_back();
    //This may hurt performance, but is useful after chests with lots of items are emptied
    if(m_inventory.size() == 0)
      m_inventory.shrink_to_fit();
  }
}
