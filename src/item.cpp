#include "include/item.h"
#include <functional>
#include <algorithm>

static_assert(EQUIP_MAX > ARMOR_MAX, "ARMOR_MAX too big");
static_assert(ARMOR_BOOTS < ARMOR_MAX, "ARMOR_MAX too small");
static_assert(MELEE_WEAPON < EQUIP_MAX && MELEE_WEAPON == ARMOR_MAX,
	      "MELEE_WEAPON in wrong position");

char ItemTypeTable::add(char id, std::string name, Weight weight,
                        ArmorValue armor, AttackValue attack)
{
    const auto insert_index = get_index_of(ids, id);
    ids.insert(ids.begin()+insert_index, id);
    names.insert(names.begin()+insert_index, name);
    weights.insert(weights.begin()+insert_index, weight);
    armor_values.insert(armor_values.begin()+insert_index, armor);
    attack_values.insert(attack_values.begin()+insert_index, attack);
    return id;
}

char ItemTypeTable::add_tuple(const ItemType &new_type)
{
    using namespace std::placeholders;
    auto add_type = std::bind(&ItemTypeTable::add, this, _1, _2, _3, _4, _5);
    return std::apply(add_type, new_type);
}

bool ItemTypeTable::contains(char type) const
{
    return std::binary_search(ids.begin(), ids.end(), type);
}
