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
