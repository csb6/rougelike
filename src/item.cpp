#include "include/item.h"
#include <utility>

//Global ID generator - should be included in all .cpp files
int generateId()
{
  static int id = -1;
  ++id;
  return id;
}

Item::Item(int x, int y, std::string name, int weight)
  : m_xPos(x), m_yPos(y), m_id(generateId()), m_weight(weight), m_name(name)
{

}
