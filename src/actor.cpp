#include "include/actor.h"

ActorTypeId ActorTypeTable::add(std::string name, Strength strength, Weight max_carry)
{
    const ActorTypeId new_id = id_count++;
    id.push_back(new_id);
    this->name.push_back(name);
    this->strength.push_back(strength);
    this->max_carry.push_back(max_carry);
    return new_id;
}

Weight ActorTypeTable::get_max_carry(ActorTypeId type) const
{
    const auto index = get_index_of(id, type);
    return max_carry[type];
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


int main()
{
    ActorTable actors;
    ActorTypeTable actor_types;
    ActorInventoryTable inventories;
    ItemTable items;
    ItemTypeTable item_types;

    const auto player_type = actor_types.add("Player", 5, 10);
    const auto rat_type = actor_types.add("Rat", 3, 1);
    const auto snake_type = actor_types.add("Snake", 4, 2);

    const ActorId player = actors.add(player_type, {0, 0}, 3, 10);
    actors.add(rat_type, {5, 5}, 3, 5);
    actors.add(rat_type, {5, 6}, 3, 5);
    actors.add(rat_type, {5, 7}, 3, 5);
    actors.add(snake_type, {0, 4}, 3, 5);

    inventories.append(player, 100);

    const auto sword_type = item_types.add("Sword", 10, 1, 7);
    const auto torch_type = item_types.add("Torch", 5, 3);

    items.add({0, 2}, sword_type);
    items.add({2, 4}, torch_type);
    items.add({3, 3}, torch_type);

    return 0;
}
