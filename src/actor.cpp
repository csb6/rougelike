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

char ActorTypeTable::add(char ch, std::string name, Strength strength,
                         Weight max_carry)
{
    const auto insert_index = ids.index_of(ch);
    ids.insert_at(insert_index, ch);
    names.insert_at(insert_index, name);
    strengths.insert_at(insert_index, strength);
    max_carries.insert_at(insert_index, max_carry);
    return ch;
}

char ActorTypeTable::add(const ActorType &new_type)
{
    return add(new_type.id, new_type.name, new_type.strength, new_type.max_carry);
}

bool ActorTypeTable::contains(char type) const
{
    const auto index = ids.index_of(type);
    return index < ids.size() && ids[index] == type;
}

bool ActorTypeTable::successful_attack(std::size_t attacker_type,
                                       std::size_t target_type) const
{
    return actorWins(strengths[attacker_type].v, strengths[target_type].v);
}


ActorId ActorTable::add(char type, Position pos, Energy energy, Health health)
{
    const ActorId new_id = {id_count++};
    ids.append(new_id);
    positions.append(pos);
    healths.append(health);
    energies.append(energy);
    carries.append({0}); // initially, all actors carry nothing
    types.append(type);
    return new_id;
}

void ActorTable::next_turn()
{
    if(ids.empty())
        return;
    else if(turn_index >= ids.size()) {
        turn_index = -1;
    }
    ++turn_index;
}

// Assumes the actor id is present in this collection
void ActorTable::add_health(ActorId actor, Health amount)
{
    const auto index = ids.index_of(actor);
    healths[index] += {amount};
}


void ActorInventoryTable::add(ActorId actor, char item_type, std::size_t amount)
{
    // Index of the first entry for this actor
    std::size_t index = actor_ids.index_of(actor);
    // Find the index (within this actor's set of entries) for insertion
    for(; index < items.size(); ++index) {
        if(actor_ids[index] != actor || items[index] == item_type)
            break;
    }
    // Insertion
    if(index >= items.size() || items[index] != item_type) {
        // New unique item; insert it at end of this actor's items
        actor_ids.insert_at(index, actor);
        items.insert_at(index, item_type);
        amounts.insert_at(index, amount);
    } else {
        // Item already exists in this actor's inventory; increment that item's count
        amounts[index] += amount;
    }
}

void ActorEquipmentTable::equip(ActorId actor, short slot, char item_type)
{
    std::size_t index = actor_ids.index_of(actor);

    if(index >= actor_ids.size() || actor_ids[index] != actor) {
        // First-time equipping something for this actor
        for(std::size_t i = index; i < index + EquipSlotCount; ++i) {
            actor_ids.insert_at(i, actor);
        }
    }

    equipments[index + slot] = item_type;
}

char ActorEquipmentTable::deequip(ActorId actor, short slot)
{
    std::size_t index = actor_ids.index_of(actor);

    if(index >= actor_ids.size() || actor_ids[index] != actor) {
        return -1;
    }

    const char old_item = equipments[index + slot];
    equipments[index + slot] = -1;
    return old_item;
}
