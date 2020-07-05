#include "include/item.h"

std::pair<ItemCategory, ItemId> ItemTypeTable::find(char icon) const
{
    for(size_t i = 0; i < melee_weapons.size(); ++i) {
        if(melee_weapons[i].icon == icon) {
            return {ItemCategory::Melee, i};
        }
    }

    for(size_t i = 0; i < ranged_weapons.size(); ++i) {
        if(ranged_weapons[i].icon == icon) {
            return {ItemCategory::Ranged, i};
        }
    }

    for(size_t i = 0; i < armor.size(); ++i) {
        if(armor[i].icon == icon) {
            return {ItemCategory::Armor, i};
        }
    }

    for(size_t i = 0; i < misc.size(); ++i) {
        if(misc[i].icon == icon) {
            return {ItemCategory::Misc, i};
        }
    }

    return {ItemCategory::None, 0};
}
