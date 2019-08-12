#include "include/item.h"

static int generateId()
{
  static int id = -1;
  ++id;
  return id;
}

Item::Item(int x, int y, std::string name, int weight, int armor)
  : m_xPos(x), m_yPos(y), m_id(generateId()), m_weight(weight), m_name(name),
    m_armor(armor)
{

}
