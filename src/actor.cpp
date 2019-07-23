#include "include/actor.h"
#include "include/gameboard.h"

/* Creates new Actor (a monster/player) at the given position with a name/on-screen
  character representation*/
Actor::Actor(int x, int y, std::string name, char ch)
  : m_xPos(x), m_yPos(y), m_energy(0), m_ch(ch), m_name(name), m_isTurn(false)
{
  Item knife = Item(x, y, "Knife", 4);
  m_inventory.push_back(knife);
  m_carryWeight += knife.getWeight();
}

/* Since no 2 Actors can occupy the same space at once, Actors are uniquely
  idenified by their coordinates*/
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
void Actor::attack(Actor &target)
{
  target.addHealth(-5);
  m_levelProgress += 5;
  --m_energy;
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
  if(m_energy <= 0 || m_health <= 0)
    m_isTurn = false;
}

/* Changes if turn/not, how much energy for the turn*/
void Actor::setTurn(bool isTurn, int energy)
{
  m_isTurn = isTurn;
  m_energy = energy;
}

/* Checks if actor has enough inventory space left to carry an item of given weight */
bool Actor::canCarry(int itemWeight)
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
  if(m_inventory.size() > 0)
  {
    std::swap(item, m_inventory.back());
    m_inventory.pop_back();
    //This may hurt performance, but is useful after chests with lots of items are emptied
    if(m_inventory.size() == 0)
      m_inventory.shrink_to_fit();
  }
}

/* Changes health points of Actor*/
void Actor::addHealth(int amount)
{
  m_health += amount;
}
