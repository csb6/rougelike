#ifndef OBJECT_TEMPLATE_H
#define OBJECT_TEMPLATE_H
#include "actor.h"
#include "item.h"

void loadItemTypes(const std::string &&path, ItemTypeTable &types);
void loadMonsterTypes(const std::string &&path, ActorTypeTable &types);
#endif
