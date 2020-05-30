#include "include/actor.h"
#include <functional>

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

char ActorTypeTable::add_tuple(const ActorType &new_type)
{
    using namespace std::placeholders;
    auto add_type = std::bind(&ActorTypeTable::add, this, _1, _2, _3, _4);
    return std::apply(add_type, new_type);
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
    // Iterator to the first entry for this actor
    const auto actor_begin = std::lower_bound(actor_ids.begin(), actor_ids.end(),
                                              actor);
    // Index of the first entry for this actor
    const auto actor_begin_index = std::distance(actor_ids.begin(), actor_begin);
    // Iterator to the position of the item's entry for the given actor
    const auto insert_point = std::find(items.begin()+actor_begin_index, items.end(),
                                        item_type);
    if(insert_point == items.end()) {
        // New unique item; insert it at front of this actor's items
        actor_ids.insert(actor_begin, actor);
        items.insert(items.begin()+actor_begin_index, item_type);
        amounts.insert(amounts.begin()+actor_begin_index, amount);
    } else {
        // Item already exists in this actor's inventory; increment that item's count
        const auto insert_point_index = std::distance(items.begin(), insert_point);
        amounts[insert_point_index] += amount;
    }
}
