#include "include/template.h"
#include <fstream>
#include <type_traits>

struct ActorTypeTemplate {
    char icon;
    std::string name;
    short energy;
    BattleStats base_battle_stats;

    static_assert(std::is_same_v<decltype(icon),
                  decltype(ActorTypeTable::icon)::element_type>);
    static_assert(std::is_same_v<decltype(name),
                  decltype(ActorTypeTable::name)::element_type>);
    static_assert(std::is_same_v<decltype(energy),
                  decltype(ActorTypeTable::energy)::element_type>);
    static_assert(std::is_same_v<decltype(base_battle_stats),
                  decltype(ActorTypeTable::base_battle_stats)::element_type>);
};

std::string::size_type findSplit(const std::string &line)
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

static void applyIniPair(ActorTypeTemplate &actor, std::string key,
                         std::string value)
{
    if     (key == "icon")          actor.icon                            = parseChar(value);
    else if(key == "name")          actor.name                            = value;
    else if(key == "energy")        actor.energy                          = parseInt(value);
    else if(key == "attack")        actor.base_battle_stats.attack        = parseInt(value);
    else if(key == "defense")       actor.base_battle_stats.defense       = parseInt(value);
    else if(key == "ranged_attack") actor.base_battle_stats.ranged_attack = parseInt(value);
}

void applyIniPair(WeaponType &item, std::string key, std::string value)
{
    if     (key == "icon")   item.icon   = parseChar(value);
    else if(key == "name")   item.name   = value;
    else if(key == "attack") item.attack = parseInt(value);
}

void applyIniPair(ArmorType &item, std::string key, std::string value)
{
    if     (key == "icon")    item.icon    = parseChar(value);
    else if(key == "name")    item.name    = value;
    else if(key == "defense") item.defense = parseInt(value);
}

void applyIniPair(MiscItemType &item, std::string key, std::string value)
{
    if     (key == "icon") item.icon = parseChar(value);
    else if(key == "name") item.name = value;
}

void loadMonsterTypes(const std::string &&path, ActorTypeTable &types)
{
    std::ifstream monsterFile(path);

    bool in_process = false;
    ActorTypeTemplate new_type{};
    while(monsterFile) {
	std::string line;
	std::getline(monsterFile, line);
	//Skip comments
	if(line[0] == '#')
	    continue;
	//Finalize/add template when at blank line (end of section)
	else if(line == "" && in_process) {
            types.add(new_type.icon, new_type.name, new_type.energy,
                      new_type.base_battle_stats);
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

        if(line.substr(0, split) == "icon") {
	    in_process = true;
	}

	applyIniPair(new_type, line.substr(0, split), line.substr(split+1));
    }
}
