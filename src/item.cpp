#include "include/item.h"

char ItemTypeTable::add(char id, std::string name, Weight weight,
                        ArmorValue armor, AttackValue attack)
{
    const auto insert_index = ids.index_of(id);
    ids.insert_at(insert_index, id);
    names.insert_at(insert_index, name);
    weights.insert_at(insert_index, weight);
    armor_values.insert_at(insert_index, armor);
    attack_values.insert_at(insert_index, attack);
    return id;
}

char ItemTypeTable::add(const ItemType &new_type)
{
    return add(new_type.id, new_type.name, new_type.weight, new_type.armor_value,
               new_type.attack_value);
}

bool ItemTypeTable::contains(char type) const
{
    return ids.index_of(type) != ids.size();
}
