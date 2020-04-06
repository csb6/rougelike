#ifndef ITEM_GAME_H
#define ITEM_GAME_H
#include <string>
#include <vector>
#include <array>
#include <tuple>

enum Equipment {
    ARMOR_HELMET,
    ARMOR_CHEST,
    ARMOR_PANTS,
    ARMOR_BOOTS,
    MELEE_WEAPON,
    RANGE_WEAPON,
    EQUIP_MAX
};

const int ARMOR_MAX = ARMOR_BOOTS + 1;

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
using ItemId = unsigned short;
using ItemTypeId = char;
using Position = std::array<int,2>;
using Weight = unsigned int;
using ArmorValue = unsigned int;
using AttackValue = unsigned int;
using ItemType = std::tuple<ItemTypeId, std::string, Weight, ArmorValue, AttackValue>;

struct ItemTypeTable {
    std::vector<ItemTypeId> id;
    std::vector<std::string> name;
    std::vector<Weight> weight;
    std::vector<ArmorValue> armor_value;
    std::vector<AttackValue> attack_value;

    ItemTypeId add(ItemTypeId id, std::string name, Weight weight, ArmorValue armor = 1,
                   AttackValue attack = 1);
    ItemTypeId add_tuple(const ItemType &new_type);
};

struct ItemTable {
    std::vector<ItemId> id;
    std::vector<Position> position;
    std::vector<ItemTypeId> type;

    ItemId id_count = 0;
    ItemId add(Position pos, ItemTypeId type);
};


template<typename Row, typename Id>
std::size_t get_index_of(const Row &id_row, Id target)
{
    const auto match = std::lower_bound(id_row.begin(), id_row.end(), target);
    return std::distance(id_row.begin(), match);
}
#endif
