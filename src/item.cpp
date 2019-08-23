#include "include/item.h"

static int generateId()
{
  static int id = -1;
  ++id;
  return id;
}

static_assert(EQUIP_MAX > ARMOR_MAX, "ARMOR_MAX too big");
static_assert(ARMOR_BOOTS < ARMOR_MAX, "ARMOR_MAX too small");
static_assert(MELEE_WEAPON < EQUIP_MAX && MELEE_WEAPON == ARMOR_MAX,
	      "MELEE_WEAPON in wrong position");

Item::Item(int x, int y, std::string name, int weight, std::int_least16_t armor,
	   std::int_least16_t attack)
  : m_xPos(x), m_yPos(y), m_id(generateId()), m_weight(weight), m_name(name),
    m_armor(armor), m_attack(attack)
{

}

bool Item::operator==(const Item &other) const
{
  return m_id == other.m_id;
}
