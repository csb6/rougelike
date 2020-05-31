#include "include/template.h"
#include <fstream>

static std::string::size_type findSplit(const std::string line)
{
    std::string::size_type split(0);
    for(std::string::size_type i=0; i<line.size(); ++i) {
	if(line[i] == '=') {
	    split = i;
	    break;
	}
    }
    return split;
}

static char parseChar(std::string value, char defaultVal = 'A')
{
    if(value.size() < 1 || value == "default") {
	return defaultVal;
    }
    return value[0];
}

static unsigned int parseInt(std::string value, int defaultVal = 0)
{
    if(value.size() < 1 || value == "default") {
	return defaultVal;
    }
    try {
	return static_cast<unsigned int>(std::stoi(value));
    } catch(const std::out_of_range& e) {
	return defaultVal;
    }
}

static void applyIniPair(ActorType &actor, std::string key, std::string value)
{
    if(key == "char") actor.id = {parseChar(value)};
    else if(key == "name") actor.name = {value};
    else if(key == "strength") actor.strength = {parseInt(value)};
    else if(key == "maxCarryWeight") actor.max_carry = {parseInt(value, 10)};
}

static void applyIniPair(ItemType &item, std::string key, std::string value)
{
    if(key == "char") item.id = {parseChar(value)};
    if(key == "name") item.name = {value};
    else if(key == "weight") item.weight = {parseInt(value)};
    else if(key == "attack") item.attack_value = {parseInt(value)};
    else if(key == "armor") item.armor_value = {parseInt(value)};
}

void loadItemTypes(const std::string &&path, ItemTypeTable &types)
{
    std::ifstream itemFile(path);

    bool in_process = false;
    ItemType new_type;
    while(itemFile) {
	std::string line;
	std::getline(itemFile, line);
	//Finalize/add template when at blank line (end of section)
	if(line == "" && in_process) {
            types.add(new_type);
            in_process = false;
            new_type = {};
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

	if(line.substr(0, split) == "char") {
	    in_process = true;
	}

	applyIniPair(new_type, line.substr(0, split), line.substr(split+1));
    }
}

void loadMonsterTypes(const std::string &&path, ActorTypeTable &types)
{
    std::ifstream monsterFile(path);

    bool in_process = false;
    ActorType new_type;
    while(monsterFile) {
	std::string line;
	std::getline(monsterFile, line);
	//Skip comments
	if(line[0] == '#')
	    continue;
	//Finalize/add template when at blank line (end of section)
	else if(line == "" && in_process) {
            types.add(new_type);
	    new_type = {};
            in_process = false;
	    continue;
	}
	//Find out where '=' is; it's the division between key/value
	auto split = findSplit(line);
	//Ignore erroneously formatted lines
	if(split == 0 || split == (line.size() - 1)) {
	    continue;
	}

        if(line.substr(0, split) == "char") {
	    in_process = true;
	}

	applyIniPair(new_type, line.substr(0, split), line.substr(split+1));
    }
}
