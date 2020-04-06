
/*
    Initial tables:
      Actor table:
      | actorId | pos | energy | strength | health | equipment_list | is_turn | carryWeight |
      | ....    | maxCarryWeight | inventory_list | name |

    1NF:
      Actor table:
      | actorId | x | y | energy | strength | health | is_turn | carryWeight | maxCW |
      | ....    | name |

      ActorInventory table:
      | actorId | item | is_equipped |

   2NF:
      ActorStats table:
      | actorId | x | y | energy | strength | health |

      turn = some actorId

      ActorCarryWeight table:
      | actorId | carryWeight | maxCarryWeight |

      ActorInventory table:
      | actorId | item |

      ActorEquipment table:
      | actorId | item |

*/
#include "include/item.h"
#include <array>

using ActorId = unsigned short;
using Position = std::array<int,2>;
using Energy = unsigned int;
using Strength = unsigned int;
using Health = int; // can have negative health deltas
using Weight = unsigned int;

class ActorTable {
private:
    std::vector<ActorId> id;
    std::vector<std::string> name;
    std::vector<Position> position; // (x, y)
    std::vector<Energy> energy;
    std::vector<Strength> strength;
    std::vector<Health> health;

    // The ID of the Actor whose turn it is
    ActorId current_turn = 0;
    std::size_t turn_index = 0;
    ActorId player = 0;
    ActorId id_count = 0;
    std::size_t get_index_of(ActorId target) const;
public:
    void add_actor(std::string name, Position pos, Energy energy,
                   Health health);
    void move(ActorId actor, Position newPos);
    void next_turn();
    void add_health(ActorId actor, Health amount);    
    Position get_position(ActorId actor) const;
};

class ActorInventoryTable {
private:
    std::vector<ActorId> actor_id;
    std::vector<std::vector<Item>> inventory;
    std::vector<Weight> carry;
    std::vector<Weight> max_carry;
    std::size_t get_index_of(ActorId target) const;
public:
    void append(ActorId actor, Weight carry, Weight max_carry);
    void insert(ActorId actor, Weight carry, Weight max_carry);
    bool can_carry(ActorId actor, Weight item) const;
    /*TODO:
      void add_item(ItemId item); (maybe items/actors should have same Id type?
      void delete_item(ItemId item);
      Weight get_carry_weight(ActorId actor);
      Weight get_max_carry_weight(ActorId actor);
     */
};

class ActorEquipmentTable {
private:
    std::vector<ActorId> actor_id;
    std::vector<Item> item;
};


std::size_t ActorTable::get_index_of(ActorId target) const
{
    const auto match = std::lower_bound(id.begin(), id.end(), target);
    return std::distance(id.begin(), match);
}

void ActorTable::add_actor(std::string name, Position pos, Energy energy,
                           Health health)
{
    // Add the new actor
    this->id.push_back(id_count++);
    this->name.push_back(name);
    position.push_back(pos);
    this->energy.push_back(energy);
    this->health.push_back(health);
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
    const auto index = get_index_of(actor);
    position[index] = newPos;
}

// Assumes the actor id is present in this collection
void ActorTable::add_health(ActorId actor, Health amount)
{
    const auto index = get_index_of(actor);
    health[index] += amount;
}

// Assumes the actor id is present in this collection
Position ActorTable::get_position(ActorId actor) const
{
    const auto index = get_index_of(actor);
    return position[index];
}


std::size_t ActorInventoryTable::get_index_of(ActorId target) const
{
    const auto match = std::lower_bound(actor_id.begin(), actor_id.end(), target);
    return std::distance(actor_id.begin(), match);
}

// Assumes the actor id is present in this collection
bool ActorInventoryTable::can_carry(ActorId actor, Weight item) const
{
    const auto index = get_index_of(actor);
    return carry[index] + item <= max_carry[index];
}

void ActorInventoryTable::append(ActorId actor, Weight carry, Weight max_carry)
{
    actor_id.push_back(actor);
    inventory.emplace_back();
    this->carry.push_back(carry);
    this->max_carry.push_back(max_carry);
}

void ActorInventoryTable::insert(ActorId actor, Weight carry, Weight max_carry)
{
    const auto index = get_index_of(actor);
    actor_id.emplace(actor_id.begin()+index);
    inventory.emplace(inventory.begin()+index);
    this->carry.insert(this->carry.begin()+index, carry);
    this->max_carry.insert(this->max_carry.begin()+index, max_carry);
}


int main()
{
    return 0;
}
