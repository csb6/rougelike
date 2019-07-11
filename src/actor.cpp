#include "include/actor.h"
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


Actor::Actor(int x, int y, std::string name)
  : m_xPos(x), m_yPos(y), m_energy(0), m_name(name), m_isTurn(false),
    m_carryWeight(0), m_maxCarryWeight(20)
{
  m_inventory.push_back(Item("Knife", 4));
}

void Actor::move(int newX, int newY)
{
  m_xPos = newX;
  m_yPos = newY;
  --m_energy;
  //End turn once Actor can make no more moves
  if(m_energy <= 0)
    m_isTurn = false;
}

void Actor::setTurn(bool isTurn, int energy)
{
  m_isTurn = isTurn;
  m_energy = energy;
}

/* Checks if actor has enough inventory space left to carry an item of given weight */
bool Actor::canCarry(int itemWeight)
{
  return (m_carryWeight + itemWeight) <= m_maxCarryWeight;
}

Item& Actor::getItemAt(int index)
{
  using index_t = std::vector<Item>::size_type;
  //Potentially dangerous, but don't think any chest will have enough items to cause overflow
  index_t v_index = static_cast<index_t>(index);
  return m_inventory.at(v_index);
}

void Actor::addItem(Item &item)
{
  m_inventory.push_back(item);
  m_carryWeight += item.getWeight();
}

void Actor::deleteItem(Item &item)
{
  if(m_inventory.size() > 0)
  {
    std::swap(item, m_inventory.back());
    m_inventory.pop_back();
    //This may hurt performance, but is useful after chests with lots of items are emptied
    if(m_inventory.size() == 0)
      m_inventory.shrink_to_fit();
  }
}
