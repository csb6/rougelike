#include <iostream>
#include "include/item.h"
#include "include/actor.h"
#include <array>

int main()
{
    std::array<std::array<EntityId, 10>, 10> map{};
    ItemTypeTable item_types;
    ItemTable map_items;
    ActorTypeTable actor_types;
    ActorTable map_actors;
    ActorInventoryTable inventories;
    
    return 0;
}
