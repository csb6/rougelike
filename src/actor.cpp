#include "include/actor.h"

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
        turn_index = 0;
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
    // Find the index for insertion
    for(; index < items.size(); ++index) {
        if(items[index] == item_type)
            break;
    }
    // Insertion
    if(index >= items.size() || items[index] != item_type) {
        // New unique item; insert it at front of this actor's items
        actor_ids.insert_at(index, actor);
        items.insert_at(index, item_type);
        amounts.insert_at(index, amount);
    } else {
        // Item already exists in this actor's inventory; increment that item's count
        amounts[index] += amount;
    }
}
