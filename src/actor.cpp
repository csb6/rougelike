#include "include/actor.h"

//Global ID generator - should be included in all .cpp files
int generateId()
{
  static int id = -1;
  ++id;
  return id;
}

Item::Item(std::string name, int weight) : m_id(generateId()),
					   m_weight(weight), m_name(name)
{

}


Actor::Actor(int x, int y) : m_xPos(x), m_yPos(y), m_energy(0), m_isTurn(false)
{

}

void Actor::move(int newX, int newY)
{
  m_xPos = newX;
  m_yPos = newY;
  //End turn once Actor can make no more moves
  if(--m_energy <= 0)
    m_isTurn = false;
}

void Actor::setTurn(bool isTurn, int energy)
{
  m_isTurn = isTurn;
  m_energy = energy;
}
