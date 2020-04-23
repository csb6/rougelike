#include "include/actor.h"
#include <functional>

char ActorTypeTable::add(char ch, std::string name, Strength strength,
                         Weight max_carry)
{
    ids.push_back(ch);
    names.push_back(name);
    strengths.push_back(strength);
    max_carries.push_back(max_carry);
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
    return std::binary_search(ids.begin(), ids.end(), type);
}


ActorId ActorTable::add(char type, Energy energy, Health health)
{
    const ActorId new_id = id_count++;
    ids.push_back(new_id);
    healths.push_back(health);
    energies.push_back(energy);
    carries.push_back({0}); // initially, all actors carry nothing
    types.push_back(type);
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
    current_turn = ids[turn_index];
}

// Assumes the actor id is present in this collection
void ActorTable::add_health(ActorId actor, Health amount)
{
    const auto index = get_index_of(ids, actor);
    healths[index] += amount;
}
