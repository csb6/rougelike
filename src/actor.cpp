#include "include/actor.h"
#include <functional>

ActorTypeId ActorTypeTable::add(char ch, std::string name, Strength strength,
                                Weight max_carry)
{
    id.push_back(ch);
    this->name.push_back(name);
    this->strength.push_back(strength);
    this->max_carry.push_back(max_carry);
    return ch;
}

ActorTypeId ActorTypeTable::add_tuple(const ActorType &new_type)
{
    using namespace std::placeholders;
    auto add_type = std::bind(&ActorTypeTable::add, this, _1, _2, _3, _4);
    return std::apply(add_type, new_type);
}

Weight ActorTypeTable::get_max_carry(ActorTypeId type) const
{
    const auto index = get_index_of(id, type);
    return max_carry[index];
}


bool can_carry(ActorId actor, Weight item, const ActorTable &actors,
               const ActorTypeTable &types, const ActorInventoryTable &inventories)
{
    auto index = get_index_of(actors.id, actor);
    const ActorTypeId type = actors.actor_type[index];
    const Weight max_carry_weight = types.get_max_carry(type);
    const Weight carry_weight = inventories.get_carry(actor);
    return carry_weight + item <= max_carry_weight;
}

std::string get_name(ActorId actor, const ActorTable &actors,
                     const ActorTypeTable &types)
{
    const auto actor_index = get_index_of(actors.id, actor);
    const auto type_index = get_index_of(types.id, actors.actor_type[actor_index]);
    return types.name[type_index];
}


ActorId ActorTable::add(ActorTypeId type, Position pos, Energy energy,
                        Health health)
{
    const ActorId new_id = id_count++;
    this->id.push_back(new_id);
    position.push_back(pos);
    this->energy.push_back(energy);
    this->health.push_back(health);
    actor_type.push_back(type);
    return new_id;
}

void ActorTable::next_turn()
{
    if(turn_index >= id.size() - 1)
        return;
    ++turn_index;
    current_turn = id[turn_index];
}

// Assumes the actor id is present in this collection
void ActorTable::move(ActorId actor, Position newPos)
{
    const auto index = get_index_of(id, actor);
    position[index] = newPos;
}

// Assumes the actor id is present in this collection
void ActorTable::add_health(ActorId actor, Health amount)
{
    const auto index = get_index_of(id, actor);
    health[index] += amount;
}

// Assumes the actor id is present in this collection
Position ActorTable::get_position(ActorId actor) const
{
    const auto index = get_index_of(id, actor);
    return position[index];
}


void ActorInventoryTable::append(ActorId actor, Weight carry)
{
    actor_id.push_back(actor);
    inventory.emplace_back();
    this->carry.push_back(carry);
}

void ActorInventoryTable::insert(ActorId actor, Weight carry)
{
    const auto index = get_index_of(actor_id, actor);
    actor_id.emplace(actor_id.begin()+index);
    inventory.emplace(inventory.begin()+index);
    this->carry.insert(this->carry.begin()+index, carry);
}

Weight ActorInventoryTable::get_carry(ActorId actor) const
{
    const auto index = get_index_of(actor_id, actor);
    return carry[index];
}

void ActorInventoryTable::add_item(ActorId actor, ItemId item)
{
    const auto index = get_index_of(actor_id, actor);
    inventory[index].push_back(item);
}
