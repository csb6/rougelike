#include <iostream>
#include <fstream>
#include <vector>

class Actor
{
public:
  Actor();
  std::string m_name;
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

int main()
{
  std::vector<Actor> actors;
  
  std::ifstream monsterFile("monsters.ini");
  if(!monsterFile)
  {
    std::cerr << "Error: could not load monster file\n";
    exit(1);
  }
  while(monsterFile)
  {
    std::string line;
    std::getline(monsterFile, line);
    //Skip comments
    if(line[0] == '#')
      continue;
    //Find out where '=' is; it's the division between key/value
    std::string::size_type split(0);
    for(std::string::size_type i=0; i<line.size(); ++i)
    {
      if(line[i] == '=')
      {
	split = i;
	break;
      }
    }
    //Ignore erroneously formatted lines
    if(split == 0 || split == (line.size() - 1))
      continue;
    
    Actor newActor;

    actors.push_back(newActor);
  }
  
  return 0;
}
