#include "include/actor.h"
#include <random>
#include <ctime>

//RNG
constexpr unsigned int RNGUpperLimit = 100;
/* Gets random integer on range [min, max]*/
static unsigned int getRandomNumber(unsigned int min, unsigned int max)
{
    static std::mt19937 generator(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<unsigned int> distribution(min, max);
    return distribution(generator);
}

/* Given a skill amount for 2 actors, decide using RNG/relative skills who wins skill check */
bool actorWins(unsigned int skillAmt, unsigned int otherSkillAmt)
{
    unsigned int randNumber = getRandomNumber(0, RNGUpperLimit);
    int difference = skillAmt - otherSkillAmt;
    unsigned int divider = RNGUpperLimit / 2;
    if(difference != 0) {
	//Scales linearly; actor with 12 better wins ~98% of time ((50 + 48) / 100)
	divider += (difference * 4);
    }
    return randNumber < divider;
}



ActorTypeId ActorTypeTable::add(char icon_, std::string name_,
                                short energy_, BattleStats stats)
{
    icon.append(icon_);
    name.append(name_);
    energy.append(energy_);
    base_battle_stats.append(stats);
    return static_cast<ActorTypeId>(icon.size() - 1);
}


std::pair<bool, ActorTypeId> ActorTypeTable::find(char icon_) const
{
    for(int i = 0; i < icon.size(); ++i) {
        if(icon[i] == icon_) {
            return {true, i};
        }
    }
    return {false, 0};
}


ActorId ActorTable::add(ActorTypeId type_, std::string name_, Position pos,
                        short health_)
{
    type.append(type_);
    name.append(name_);
    position.append(pos);
    health.append(health_);
    inventory.append(InventorySet{});
    return static_cast<ActorId>(type.size() - 1);
}

bool ActorTable::subtract_health(ActorId actor, short amount)
{
    health[actor] -= amount;
    // True if dead
    return health[actor] <= 0;
}


/*
void ActorEquipmentTable::equip(ActorId actor, EquipSlot slot, char item_type)
{
    std::size_t index = actor_ids.index_of(actor);

    if(index >= actor_ids.size() || actor_ids[index] != actor) {
        // First-time equipping something for this actor
        actor_ids.append(actor);
        index = actor_ids.size() - 1;
    }

    switch(slot) {
    case EquipSlot::Head:
        equipments[index].head = item_type;
        break;
    case EquipSlot::Chest:
        equipments[index].chest = item_type;
        break;
    case EquipSlot::Legs:
        equipments[index].legs = item_type;
        break;
    case EquipSlot::Feet:
        equipments[index].feet = item_type;
        break;
    case EquipSlot::Melee:
        equipments[index].melee = item_type;
        break;
    case EquipSlot::Ranged:
        equipments[index].ranged = item_type;
        break;
    }
}

char ActorEquipmentTable::deequip(ActorId actor, EquipSlot slot)
{
    std::size_t index = actor_ids.index_of(actor);

    if(index >= actor_ids.size() || actor_ids[index] != actor) {
        return -1;
    }

    char old_item;
    switch(slot) {
    case EquipSlot::Head:
        old_item = equipments[index].head;
        equipments[index].head = -1;
        return old_item;
    case EquipSlot::Chest:
        old_item = equipments[index].chest;
        equipments[index].chest = -1;
        return old_item;
    case EquipSlot::Legs:
        old_item = equipments[index].legs;
        equipments[index].legs = -1;
        return old_item;
    case EquipSlot::Feet:
        old_item = equipments[index].feet;
        equipments[index].feet = -1;
        return old_item;
    case EquipSlot::Melee:
        old_item = equipments[index].melee;
        equipments[index].melee = -1;
        return old_item;
    case EquipSlot::Ranged:
        old_item = equipments[index].ranged;
        equipments[index].ranged = -1;
        return old_item;
    }
}

char ActorEquipmentTable::equipment_at(ActorId actor, EquipSlot slot) const
{
    std::size_t index = actor_ids.index_of(actor);

    if(index >= actor_ids.size() || actor_ids[index] != actor) {
        return -1;
    }

    switch(slot) {
    case EquipSlot::Head:
        return equipments[index].head;
    case EquipSlot::Chest:
        return equipments[index].chest;
    case EquipSlot::Legs:
        return equipments[index].legs;
    case EquipSlot::Feet:
        return equipments[index].feet;
    case EquipSlot::Melee:
        return equipments[index].melee;
    case EquipSlot::Ranged:
        return equipments[index].ranged;
    }
}

char ActorEquipmentTable::to_bits(size_t index) const
{
    const ActorEquipment &slots = equipments[index];
    char result = 0;
    if(slots.head != -1)   result |= 0b0000'0001;
    if(slots.chest != -1)  result |= 0b0000'0010;
    if(slots.legs != -1)   result |= 0b0000'0100;   
    if(slots.feet != -1)   result |= 0b0000'1000;
    if(slots.melee != -1)  result |= 0b0001'0000; 
    if(slots.ranged != -1) result |= 0b0010'0000;

    return result;
}
*/
