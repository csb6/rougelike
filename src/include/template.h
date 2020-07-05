#ifndef OBJECT_TEMPLATE_H
#define OBJECT_TEMPLATE_H
#include "actor.h"
#include "item.h"
#include <fstream>
#include <string>

std::string::size_type findSplit(const std::string &line);
void applyIniPair(WeaponType &item, std::string key, std::string value);
void applyIniPair(ArmorType &item, std::string key, std::string value);
void applyIniPair(MiscItemType &item, std::string key, std::string value);

template<typename ItemType, size_t ArraySize>
void loadItemTypes(const std::string &&path, Array<ItemType, ArraySize> &types)
{
    std::ifstream itemFile(path);

    ItemType new_item{};
    bool in_process = false;
    while(itemFile) {
	std::string line;
	std::getline(itemFile, line);
	//Finalize/add template when at blank line (end of section)
	if(line == "" && !in_process) {
            types.append(new_item);
            new_item = {};
	    continue;
	} else if(line[0] == '#') {
            //Skip comments
	    continue;
        }
	//Find out where '=' is; it's the division between key/value
	auto split = findSplit(line);
	//Ignore erroneously formatted lines
	if(split == 0 || split == (line.size() - 1)) {
	    continue;
	}

	if(line.substr(0, split) == "icon") {
	    in_process = true;
	}

	applyIniPair(new_item, line.substr(0, split), line.substr(split+1));
    }
}

void loadMonsterTypes(const std::string &&path, ActorTypeTable &types);
#endif
