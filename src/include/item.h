#ifndef ITEM_GAME_H
#define ITEM_GAME_H
#include <string>
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

struct Weight : strong_type<unsigned int, Weight> {};
struct ArmorValue : strong_type<unsigned int, ArmorValue> {};
struct AttackValue : strong_type<unsigned int, AttackValue> {};

struct ItemType {
    char id;
    std::string name;
    Weight weight;
    ArmorValue armor_value;
    AttackValue attack_value;
};

struct ItemTypeTable {
    SortedArray<char, 30> ids; // the char representing the item onscreen
    Array<std::string, 30> names;
    Array<Weight, 30> weights;
    Array<ArmorValue, 30> armor_values;
    Array<AttackValue, 30> attack_values;

    char add(char id, std::string name, Weight weight, ArmorValue armor = {1},
             AttackValue attack = {1});
    char add(const ItemType &new_type);
    bool contains(char type) const;
};
#endif
