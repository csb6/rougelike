#include "src/include/actor.h"
#include <iostream>
#include <cassert>

bool actorWins(int skillAmt, int otherSkillAmt);
int getArmorBonus(int skillAmt, Item *armor);
bool actorWinsFight(int skillAmt, int otherSkillAmt, Item *armor, Item *otherArmor);

std::string getLocalDir() { return "./"; }


/* Runs trials using player/enemy skill, returning the win
 * percentage of the actor with the most wins */
static double checkWins(int playerSkill, int enemySkill )
{
  int playerWins = 0;
  int enemyWins = 0;
  for(int i=0; i<1000000; ++i)
  {
    //Difference of 5pts -> 75% more wins; 0pts -> 2% more, 20pts -> all wins
    if(actorWins(playerSkill, enemySkill)) {
      playerWins += 1;
    }
    else {
      enemyWins += 1;
    }
  }
  std::cout << "\tActor A Wins: " << playerWins << "\n";
  std::cout << "\tActor B Wins: " << enemyWins << "\n";
  return static_cast<double>(playerWins > enemyWins ? playerWins : enemyWins) / static_cast<double>(playerWins + enemyWins);
}

/* Checks that when using checkWins() with fixed arrays of skills belonging to
 * 2 actors, the win percentage is within expected range. Also swaps player and
 * enemy skill, ensuring that win percentage is the same */
static void testRange(int playerSkill, int enemySkill, double min, double max)
{
  //Test expected win percent of the actor with the higher skills
  double winPercent;
  std::cout << "Testing player=" << playerSkill << " enemy=" << enemySkill <<"\n";
  winPercent = checkWins(playerSkill, enemySkill);
  std::cout << "\t" << winPercent * 100 << "% to winner\n";
  assert(winPercent <= max && winPercent >= min && "Missing RNG target");

  std::cout << "Testing player=" << enemySkill << " enemy=" << playerSkill <<"\n";
  winPercent = checkWins(enemySkill, playerSkill);
  std::cout << "\t" << winPercent * 100 << "% to winner\n\n";
  assert(winPercent <= max && winPercent >= min && "Missing RNG target");
}

/* Returns sum of all length number of integers in array */
static int sumIntArray(int *array, int length)
{
  int sum = 0;
  for(int i=0; i<length; ++i) {
    sum += array[i];
  }
  return sum;
}

static void testRNG()
{
  std::cout << "\tTesting single-skill match-ups...\n";
  //If actors' skills differ by 12pts, higher wins ~98% of time
  testRange(0, 12, 0.96, 1);
  //If actors' skills differ by 6pts, higher wins ~74% of time
  testRange(0, 6, 0.72, 0.75);
  //If actors' skills differ by 3pts, higher wins ~62% of time
  testRange(3, 6, 0.60, 0.64);
  //If actors' skills differ by 0pts, both win ~50% of time
  testRange(6, 6, 0.49, 0.52);

  std::cout << "All RNG values in expected ranges\n";
}


static void testItems()
{
  //Getter/constructor
  {
    Item a;
    assert(a.getId() == 0 && "Item id doesn't start at 0");
    assert(a.getX() == 0 && a.getY() == 0 && "Item position doesn't start at (0,0)");
    assert(a.getWeight() == 0 && "Item weight doesn't start at 0");
    assert(a.getArmor() == 0 && "Item armor doesn't start at 0");
    assert(a.getName() == "Item" && "Item name isn't defaulted to 'Item'");
    assert(!a.isEquipped() && "Item should start out deequipped");
    assert(!a.isRanged() && "Item should start out as not a ranged weapon");
  }
  //Setters/creating multiple objects
  {
    Item b(1, 0, "Knife", 4, 1);
    assert(b.getId() == 1 && "Item id doesn't increment by 1");
    assert(b.getX() == 1 && b.getY() == 0 && "Item Position not set");
    assert(b.getWeight() == 4 && "Item weight doesn't start at 0");
    assert(b.getArmor() == 1 && "Item armor not set");
    assert(b.getName() == "Knife" && "Item name not set");
    b.setEquip(true);
    assert(b.isEquipped() && "Item not able to be equipped");
    b.setRanged(true);
    assert(b.isRanged() && "Item not able to be made ranged");
  }
  std::cout << "All item tests passed\n";
}


static void testActors()
{
  //Getter/constructor
  {
    Actor a;
    assert(a.getEnergy() == 0 && "Actor energy doesn't start at 0");
    assert(a.getX() == 0 && a.getY() == 0 && "Actor position doesn't start at (0,0)");
    assert(a.getHealth() == 15 && "Actor health doesn't start at 15");
    assert(a.isAlive() && "Actor doesn't start out alive");
    assert(a.getName() == "Monster" && "Actor name isn't defaulted to 'Monster'");
    assert(!a.isTurn() && "Actor should start out with m_turn as false");
    assert(a.getInventorySize() == 0 && a.m_carryWeight == 0 &&
	   "Actor should start out with empty inventory");
  }
  //Setters
  {
    Actor a(1, 2, "Joe", 'J');
    assert(a.getName() == "Joe" && "Actor name not set");
    assert(a.getX() == 1 && a.getY() == 2 && "Actor position not set");
    assert(a.getCh() == 'J' && "Actor char not set");
    a.setTurn(true, 2);
    assert(a.isTurn() && a.getEnergy() == 2 && "Actor turn not set correctly");
    a.setCh('O');
    assert(a.getCh() == 'O' && "Actor char not set correctly");
    a.addHealth(-15);
    assert(!a.isAlive() && "Actor not dying when out of health");
    a.move(5, 9);
    assert(a.getX() == 5 && a.getY() == 9 && "Actor not moving correctly");
    assert(a.m_maxCarryWeight == 20 && a.canCarry(19) && !a.canCarry(21)
	   && "Carry weight not being respected");
  }
  //Interactions between multiple objects
  {
    Actor a(0, 6, "Jimmy-Two-Shoes", 'T');
    Actor b(4, 5, "Timmy-Gym-Sock", 'G');
    assert(!(a == b) && "Actors operator== not working");
    auto prevHealthA = a.getHealth();
    auto prevHealthB = b.getHealth();
    assert(prevHealthA == prevHealthB && "Actors not starting out with same health");
    a.attack(b);
    assert((b.getHealth() < prevHealthB || a.getHealth() < prevHealthA)
	   && "Attacks not causing damage");

    Item i(0, 0, "Knife", 5, 2);
    Actor c;
    assert(c.canCarry(i.getWeight()) && "Item weight not matching with carry capacity");
    c.addItem(i);
    assert(c.getInventorySize() == 1 && *c.getItemAt(0) == i
	   && "Item not added to Actor inventory");
    c.equipItem(0, MELEE_WEAPON);
    assert(c.getEquipped(MELEE_WEAPON) != nullptr && *c.getEquipped(MELEE_WEAPON) == i
	   && "Item not being equipped into slot");
    assert(c.getInventorySize() == 0 && "Inventory not emptied when Item equipped");
    c.deequipItem(MELEE_WEAPON);
    assert(c.getInventorySize() == 1 && c.getEquipped(MELEE_WEAPON) == nullptr
	   && "Item not deequipping properly");
  }

  std::cout << "All actor tests passed\n";
}

int main()
{
  testRNG();
  testItems();
  testActors();
  return 0;
}
