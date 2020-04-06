#include "include/item.h"

static_assert(EQUIP_MAX > ARMOR_MAX, "ARMOR_MAX too big");
static_assert(ARMOR_BOOTS < ARMOR_MAX, "ARMOR_MAX too small");
static_assert(MELEE_WEAPON < EQUIP_MAX && MELEE_WEAPON == ARMOR_MAX,
	      "MELEE_WEAPON in wrong position");

ItemTypeId ItemTypeTable::add(std::string name, Weight weight,
                              ArmorValue armor, AttackValue attack)
{
    const ItemTypeId new_id = id_count++;
    id.push_back(new_id);
    this->name.push_back(name);
    this->weight.push_back(weight);
    armor_value.push_back(armor);
    attack_value.push_back(attack);
    return new_id;
}


ItemId ItemTable::add(Position pos, ItemTypeId type)
{
    const ItemId new_id = id_count++;
    id.push_back(new_id);
    position.push_back(pos);
    this->type.push_back(type);
    return new_id;
}
