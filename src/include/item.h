#ifndef ITEM_GAME_H
#define ITEM_GAME_H
#include <string>
#include "SortedArray.h"
#include <utility>

struct BaseItemType {
    std::string name;
    char icon;
};

struct WeaponType : public BaseItemType {
    int attack;
};

struct ArmorType : public BaseItemType {
    int defense;
};

struct MiscItemType : public BaseItemType {};

using ItemId = size_t;

enum class ItemCategory : char {
    Melee, Ranged, Armor, Misc, None
};

struct ItemTypeTable {
    Array<WeaponType, 30>   melee_weapons;
    Array<WeaponType, 30>   ranged_weapons;
    Array<ArmorType, 30>    armor;
    Array<MiscItemType, 30> misc;

    std::pair<ItemCategory, ItemId> find(char icon) const;
};
#endif
