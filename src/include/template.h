#ifndef OBJECT_TEMPLATE_H
#define OBJECT_TEMPLATE_H
#include <map>
#include "actor.h"
#include "item.h"

std::map<char,Item> loadItemTemplates(const std::string &&path);
std::map<char,Actor> loadMonsterTemplates(const std::string &&path);
#endif
