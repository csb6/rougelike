#ifndef ITEM_GAME_H
#define ITEM_GAME_H
#include <string>
#include <vector>
#include <array>
#include <tuple>
#include "strong-types.hpp"

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
struct Weight { unsigned int v; };
struct ArmorValue { unsigned int v; };
struct AttackValue { unsigned int v; };
using ItemType = std::tuple<char, std::string, Weight, ArmorValue, AttackValue>;

struct ItemTypeTable {
    std::vector<char> ids; // the char representing the item onscreen
    std::vector<std::string> names;
    std::vector<Weight> weights;
    std::vector<ArmorValue> armor_values;
    std::vector<AttackValue> attack_values;

    char add(char id, std::string name, Weight weight, ArmorValue armor = {1},
             AttackValue attack = {1});
    char add_tuple(const ItemType &new_type);
};

template<typename Row, typename Id>
std::size_t get_index_of(const Row &id_row, Id target)
{
    const auto match = std::lower_bound(id_row.begin(), id_row.end(), target);
    return std::distance(id_row.begin(), match);
}
#endif
