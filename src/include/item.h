#ifndef ITEM_GAME_H
#define ITEM_GAME_H
#include <string>
#include <tuple>
#include "strong-types.hpp"
#include "SortedArray.h"

enum Equipment {
    ARMOR_HELMET,
    ARMOR_CHEST,
    ARMOR_PANTS,
    ARMOR_BOOTS,
    MELEE_WEAPON,
    RANGE_WEAPON,
    EQUIP_MAX
};

constexpr int ARMOR_MAX = ARMOR_BOOTS + 1;

/*
  Initial tables:
    Item table:
    | itemId | pos | weight | name | armor_value | attack_value |

  1NF:
    Item table:
    | itemId | pos | itemType

    ItemTypes table:
    | itemType | name | weight | armor_value | attack_value

*/
struct Weight : strong_type<unsigned int, Weight> {};
struct ArmorValue : strong_type<unsigned int, ArmorValue> {};
struct AttackValue : strong_type<unsigned int, AttackValue> {};
using ItemType = std::tuple<char, std::string, Weight, ArmorValue, AttackValue>;

struct ItemTypeTable {
    SortedArray<char, 30> ids; // the char representing the item onscreen
    Array<std::string, 30> names;
    Array<Weight, 30> weights;
    Array<ArmorValue, 30> armor_values;
    Array<AttackValue, 30> attack_values;

    char add(char id, std::string name, Weight weight, ArmorValue armor = {1},
             AttackValue attack = {1});
    char add_tuple(const ItemType &new_type);
    bool contains(char type) const;
};

template<typename Row, typename Id>
std::size_t get_index_of(const Row &id_row, Id target)
{
    const auto match = std::lower_bound(id_row.begin(), id_row.end(), target);
    return std::distance(id_row.begin(), match);
}
#endif
