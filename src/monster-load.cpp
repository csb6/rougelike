#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class Actor
{
public:
  //Actor();
  std::string m_name;
  char m_ch;
  int m_energy, m_health;
  std::int_least16_t m_carryWeight; //Current weight of inventory
  std::int_least16_t m_maxCarryWeight;
  std::int_least16_t m_level; //General level; when upgraded, points available to boost stats
  std::int_least16_t m_levelProgress; //On scale 0-100; when 100, level-up

  //Core Skills - basic stats affecting all actions broadly
  std::int_least16_t m_strength;  // 0 - Affects damage in attacks, carry amount
  std::int_least16_t m_cunning;   // 1 - Affects likelihood of successful stealing, convincing others
  std::int_least16_t m_agility;   // 2 - Affects movement distance/turn, chance to dodge attacks
  std::int_least16_t m_education; // 3 - Affects ability to upgrade gear, strategize in battle

  //Life Skills - stats affecting specific interactions
  std::int_least16_t m_sidearmSkill;   // 0 - Skill with small guns
  std::int_least16_t m_longarmSkill;   // 1 - Skill with big guns
  std::int_least16_t m_meleeSkill;     // 2 - Skill with melee weapons
  std::int_least16_t m_vehicleSkill;   // 3 - Skill with vehicular weapons
  std::int_least16_t m_barterSkill;    // 4 - Skill affecting cost of items
  std::int_least16_t m_negotiateSkill; // 5 - Skill affecting chance of successful negotiations
  std::int_least16_t m_trapSkill;      // 6 - Skill affecting chance of traps working
};

int findSplit(const std::string line)
{
  std::string::size_type split(0);
  for(std::string::size_type i=0; i<line.size(); ++i)
  {
    if(line[i] == '=')
    {
      split = i;
      break;
    }
  }
  return split;
}

char parseChar(std::string value, char defaultVal = 'A')
{
  if(value.size() < 1 || value == "default")
    return defaultVal;
  return value[0];
}

std::int_least16_t parseInt(std::string value, std::int_least16_t defaultVal = 0)
{
  if(value.size() < 1 || value == "default")
    return defaultVal;
  return std::stoi(value);
}

void applyIniPair(Actor &actor, std::string key, std::string value)
{
  if(key == "char") actor.m_ch = parseChar(value);
  else if(key == "name") actor.m_name = value;
  else if(key == "energy") actor.m_energy = parseInt(value);
  else if(key == "health") actor.m_health = parseInt(value);
  else if(key == "carryWeight") actor.m_carryWeight = parseInt(value);
  else if(key == "carryWeight") actor.m_carryWeight = parseInt(value);
  else if(key == "maxCarryWeight") actor.m_maxCarryWeight = parseInt(value);
  else if(key == "level") actor.m_level = parseInt(value);
  else if(key == "levelProgress") actor.m_levelProgress = parseInt(value);
  else if(key == "strength") actor.m_strength = parseInt(value);
  else if(key == "cunning") actor.m_cunning = parseInt(value);
  else if(key == "agility") actor.m_agility = parseInt(value);
  else if(key == "education") actor.m_education = parseInt(value);
  else if(key == "sidearmSkill") actor.m_sidearmSkill = parseInt(value);
  else if(key == "longarmSkill") actor.m_longarmSkill = parseInt(value);
  else if(key == "meleeSkill") actor.m_meleeSkill = parseInt(value);
  else if(key == "vehicleSkill") actor.m_vehicleSkill = parseInt(value);
  else if(key == "barterSkill") actor.m_barterSkill = parseInt(value);
  else if(key == "negotiateSkill") actor.m_negotiateSkill = parseInt(value);
  else if(key == "trapSkill") actor.m_trapSkill = parseInt(value);

  std::cout << key << " " << value << "\n";
}

int main()
{
  std::vector<Actor> actors;

  std::ifstream monsterFile("monsters.ini");
  if(!monsterFile)
  {
    std::cerr << "Error: could not load monster file\n";
    exit(1);
  }

  Actor newActor;

  while(monsterFile)
  {
    std::string line;
    std::getline(monsterFile, line);
    //Skip comments
    if(line[0] == '#')
      continue;
    else if(line == "")
    {
      actors.push_back(newActor);
      newActor = Actor();
      continue;
    }
    //Find out where '=' is; it's the division between key/value
    int split = findSplit(line);
    //Ignore erroneously formatted lines
    if(split == 0 || split == (line.size() - 1))
      continue;

    applyIniPair(newActor, line.substr(0, split), line.substr(split+1));
  }

  std::cout << "\n---\n";
  for(auto actor : actors)
  {
    std::cout << actor.m_name << " " << actor.m_ch << "\n";
  }

  return 0;
}
