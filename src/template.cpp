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

static std::int_least16_t parseInt(std::string value, std::int_least16_t defaultVal = 0)
{
    if(value.size() < 1 || value == "default") {
	return defaultVal;
    }
    try {
	return std::stoi(value);
    } catch(const std::out_of_range& e) {
	return defaultVal;
    }
}

static bool parseBool(std::string value, bool defaultVal = false)
{
    if(value == "true") return true;
    else if(value == "false") return false;
    else return defaultVal;
}

static void applyIniPair(Actor &actor, std::string key, std::string value)
{
    if(key == "char") actor.setCh(parseChar(value));
    else if(key == "name") actor.setName(value);
    else if(key == "energy") actor.setEnergy(parseInt(value));
    else if(key == "health") actor.addHealth(parseInt(value, 100));
    else if(key == "carryWeight") actor.m_carryWeight = parseInt(value);
    else if(key == "carryWeight") actor.m_carryWeight = parseInt(value);
    else if(key == "maxCarryWeight") actor.m_maxCarryWeight = parseInt(value, 10);
    else if(key == "level") actor.m_level = parseInt(value);
    else if(key == "levelProgress") actor.m_levelProgress = parseInt(value);
    else if(key == "strength") actor.m_strength = parseInt(value);
    else if(key == "cunning") actor.m_cunning = parseInt(value);
    else if(key == "agility") actor.m_agility = parseInt(value);
    else if(key == "education") actor.m_education = parseInt(value);
    else if(key == "sidearmSkill") actor.m_sidearmSkill = parseInt(value);
    else if(key == "longarmSkill") actor.m_longarmSkill = parseInt(value);
    else if(key == "meleeSkill") actor.m_meleeSkill = parseInt(value);
    else if(key == "barterSkill") actor.m_barterSkill = parseInt(value);
    else if(key == "negotiateSkill") actor.m_negotiateSkill = parseInt(value);
}

static void applyIniPair(Item &item, std::string key, std::string value)
{
    if(key == "name") item.setName(value);
    else if(key == "weight") item.setWeight(parseInt(value));
    else if(key == "attack") item.setAttack(parseInt(value));
    else if(key == "armor") item.setArmor(parseInt(value));
    else if(key == "isMelee") item.setMelee(parseBool(value));
    else if(key == "isRanged") item.setRanged(parseBool(value));
}

std::map<char,Item> loadItemTemplates(const std::string &&path)
{
    std::ifstream itemFile(path);

    char ch = 0;
    bool hasCh = false;
    std::map<char,Item> templates;
    Item newTemplate;
    while(itemFile) {
	std::string line;
	std::getline(itemFile, line);
	//Skip comments
	if(line[0] == '#')
	    continue;
	//Finalize/add template when at blank line (end of section)
	else if(line == "" && hasCh) {
	    templates[ch] = newTemplate;
	    newTemplate = Item();
	    hasCh = false;
	    continue;
	}
	//Find out where '=' is; it's the division between key/value
	auto split = findSplit(line);
	//Ignore erroneously formatted lines
	if(split == 0 || split == (line.size() - 1)) {
	    continue;
	}

	if(line.substr(0, split) == "char") {
	    //All items represented by a char in map files, but not onscreen,
	    //so char not part of the Item objects themselves
	    ch = parseChar(line.substr(split+1), '-');
	    hasCh = true;
	}

	applyIniPair(newTemplate, line.substr(0, split), line.substr(split+1));
    }
    return templates;
}

std::map<char,Actor> loadMonsterTemplates(const std::string &&path)
{
    std::ifstream monsterFile(path);

    std::map<char,Actor> templates;
    Actor newTemplate;
    while(monsterFile) {
	std::string line;
	std::getline(monsterFile, line);
	//Skip comments
	if(line[0] == '#')
	    continue;
	//Finalize/add template when at blank line (end of section)
	else if(line == "") {
	    templates[newTemplate.getCh()] = newTemplate;
	    newTemplate = Actor();
	    continue;
	}
	//Find out where '=' is; it's the division between key/value
	auto split = findSplit(line);
	//Ignore erroneously formatted lines
	if(split == 0 || split == (line.size() - 1)) {
	    continue;
	}

	applyIniPair(newTemplate, line.substr(0, split), line.substr(split+1));
    }
    return templates;
}
