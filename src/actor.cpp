#include "include/actor.h"
#include "include/gameboard.h"
#include <random>
#include <ctime>

//RNG
/* Gets random integer on range [min, max]*/
int getRandomNumber(int min, int max)
{
  static std::mt19937 generator(static_cast<unsigned int>(std::time(nullptr)));
  std::uniform_int_distribution<int> distribution(min, max);
  return distribution(generator);
}

/* Given a skill amount for 2 actors, decide using RNG/relative skills who wins skill check */
bool actorWins(int skillAmt, int otherSkillAmt)
{
  int randNumber = getRandomNumber(0, RNGUpperLimit);
  int difference = skillAmt - otherSkillAmt;
  int divider = RNGUpperLimit / 2;
  if(difference != 0)
    //Scales linearly; actor with 12 better wins ~98% of time ((50 + 48) / 100)
    divider += (difference * 4);
  return randNumber < divider;
}

/* Given skill amounts/armor arrays for 2 actors, determine who wins the fight
 * using RNG based on their armor and the given skill amounts
bool actorWinsFight(int skillAmt, int otherSkillAmt, Armor *armorWorn, Armor *otherArmorWorn)
{
  int armorBonus = getArmorBonus(skillAmt, armorWorn);
  int otherArmorBonus = getArmorBonus(otherSkillAmt, otherArmorWorn);
  return actorWins(skillAmt + armorBonus, otherSkillAmt + otherArmorBonus);
}

 Given lists of skills for 2 actors, decide using RNG/relative skills who wins skill check
 * by summing skills in each list, calling actorWins(); weight using multipliers when creating
 * the list in calling location
bool multiSkillCheck(int *skills, int *otherSkills, int numberOfSkills)
{
  int i = 0;
  int skillSum = 0;
  int otherSkillSum = 0;
  while(i<numberOfSkills)
  {
    skillSum += skills[i];
    otherSkillSum += otherSkills[i];
    ++i;
  }
  return actorWins(skillSum, otherSkillSum);
}*/


/* Creates new Actor (a monster/player) at the given position with a name/on-screen
  character representation*/
Actor::Actor(int x, int y, std::string name, char ch)
  : m_xPos(x), m_yPos(y), m_energy(0), m_ch(ch), m_name(name), m_isTurn(false), m_armor{0, 0, 0, 0}
{
  Item knife = Item(x, y, "Knife", 4);
  m_carryWeight += knife.getWeight();
  m_inventory.push_back(knife);
}

/* Since no 2 Actors can occupy the same space at once, Actors are uniquely
  identified by their coordinates*/
bool Actor::operator==(const Actor &other)
{
  return m_xPos == other.m_xPos && m_yPos == other.m_yPos;
}

/* Changes the Actor's position*/
void Actor::move(int newX, int newY)
{
  m_xPos = newX;
  m_yPos = newY;
  --m_energy;
}

/* Attempts to attack another Actor*/
bool Actor::attack(Actor &target)
{
  --m_energy;
  if(actorWins(m_strength, target.m_strength))
  {
    target.addHealth(-5);
    m_levelProgress += 5;
    return true;
  }
  else
  {
    addHealth(-5);
    return false;
  }
}

/* Called once every tick; serves as location for AI, visual
  effects, or anything else that needs to happen regularly*/
void Actor::update(GameBoard *board)
{
  //Simple, dumb AI that just moves right until hitting wall/running out
  //of energy; ending turn
  if(m_isTurn && m_name != "Player")
  {
    bool moved = board->translateActor(*this, 1, 0);
    if(!moved)
      m_isTurn = false;
  }
  //End turn once Actor can make no more moves; should be in all update()'s
  if(m_energy <= 0)
    m_isTurn = false;
}

/* Changes if turn/not, how much energy for the turn*/
void Actor::setTurn(bool isTurn, int energy)
{
  m_isTurn = isTurn;
  m_energy = energy;
}

/* Checks if actor has enough inventory space left to carry an item of given weight */
bool Actor::canCarry(int itemWeight) const
{
  return (m_carryWeight + itemWeight) <= m_maxCarryWeight;
}

/* Gives reference to item in inventory*/
Item& Actor::getItemAt(int index)
{
  using index_t = std::vector<Item>::size_type;
  //Potentially dangerous, but don't think any chest will have enough items to cause overflow
  index_t v_index = static_cast<index_t>(index);
  return m_inventory.at(v_index);
}

/* Adds a new Item to inventory*/
void Actor::addItem(Item &item)
{
  m_inventory.push_back(item);
  m_carryWeight += item.getWeight();
}

/* Removes an Item from inventory*/
void Actor::deleteItem(Item &item)
{
  if(m_inventory.size() <= 0 || item.isEquipped())
    return;
  std::swap(item, m_inventory.back());
  m_inventory.pop_back();
  //This may hurt performance, but is useful after chests with lots of items are emptied
  if(m_inventory.size() == 0)
    m_inventory.shrink_to_fit();
}

/* Place's inventory item into armor slot; checks if valid*/
void Actor::equipArmor(int index, int position)
{
  using index_t = std::vector<Item>::size_type;
  if(index < 0 || static_cast<index_t>(index) >= m_inventory.size()
     || position >= ARMOR_MAX || position < 0)
    return;

  if(m_armor[position].isEquipped())
  {
    //If Item already in slot, put back in inventory
    m_armor[position].setEquip(false);
    m_inventory.push_back(m_armor[position]);
  }
  //After adding new Item to armor slot, delete from inventory
  m_armor[position] = m_inventory[index];
  m_armor[position].setEquip(true);
  m_inventory.erase(m_inventory.begin()+index);
}

/* Removes inventory item from armor slot; checks if valid*/
void Actor::deequipArmor(int position)
{
  if(position < 0 || position >= ARMOR_MAX || !m_armor[position].isEquipped())
    return;
  m_armor[position].setEquip(false);
  m_inventory.push_back(m_armor[position]);
  m_armor[position] = 0;
}

Item* Actor::getArmorAt(int index)
{
  //Ignore default placeholder Items in array
  if(index < 0 || index >= ARMOR_MAX || !m_armor[index].isEquipped())
    return nullptr;
  return &m_armor[index];
}

/* Changes health points of Actor*/
void Actor::addHealth(int amount)
{
  m_health += amount;
}
