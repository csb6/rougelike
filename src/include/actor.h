#ifndef ACTOR_H
#define ACTOR_H
#include "item.h"

struct ActorId : strong_type<unsigned short, ActorId> {};
struct Energy : strong_type<unsigned int, Energy> {};
struct Strength : strong_type<unsigned int, Strength> {};
struct Health : strong_type<int, Health> {}; // can have negative health deltas

struct ActorType {
    char id;
    std::string name;
    Strength strength;
    Weight max_carry;
};

struct Position {
    int x = 0;
    int y = 0;
};

constexpr ActorId InitActorId{0};

struct ActorTypeTable {
    SortedArray<char, 30> ids;
    Array<std::string, 30> names;
    Array<Strength, 30> strengths;
    Array<Weight, 30> max_carries;

    char add(char ch, std::string name, Strength strength, Weight max_carry);
    char add(const ActorType &new_type);
    bool contains(char actor) const;
};

struct ActorTable {
    SortedArray<ActorId, 40> ids;
    Array<Position, 40> positions;
    Array<Health, 40> healths;
    Array<Energy, 40> energies;
    Array<Weight, 40> carries;
    Array<char, 40> types;

    std::size_t turn_index = 0;
    ActorId id_count = InitActorId;
    // NOTE: player_index assumed to be 0, and assumed to == index of player in ActorTypesTable
    std::size_t player_index = 0;
    inline ActorId player() const { return ids[player_index]; }
    inline int player_x() const { return positions[player_index].x; }
    inline int player_y() const { return positions[player_index].y; }
    inline ActorId curr() const { return ids[turn_index]; }

    ActorId add(char type, Position pos, Energy energy, Health health);
    void next_turn();
    void add_health(ActorId actor, Health amount);    
};

struct ActorInventoryTable {
    SortedArray<ActorId, 60> actor_ids;
    Array<char, 60> items;
    Array<std::size_t, 60> amounts;

    void add(ActorId actor, char item_type, std::size_t amount = 1);
};

/*struct ActorEquipmentTable {
    std::vector<ActorId> actor_id;
    //std::vector<ItemId> item;
    };*/

constexpr int RNGUpperLimit = 100;
constexpr int SkillAmount = 9; //Number of skills (e.g. strengh, agility, etc.)
constexpr int MaxInitPoints = 25; //Total pts doled out at character creation

/*  std::int_least16_t m_carryWeight = 0; //Current weight of inventory
  std::int_least16_t m_maxCarryWeight = 20;
  std::int_least16_t m_level = 1; //General level; when upgraded, points available to boost stats
  std::int_least16_t m_levelProgress = 0; //On scale 0-100; when 100, level-up

  //Core Skills - basic stats affecting all actions broadly
  std::int_least16_t m_strength = 0;// 0 - Affects damage in attacks, carry amount
  std::int_least16_t m_cunning = 0; // 1 - Affects likelihood of successful stealing, convincing others
  std::int_least16_t m_agility = 0; // 2 - Affects movement distance/turn, chance to dodge attacks
  std::int_least16_t m_education = 0; // 3 - Affects ability to upgrade gear, strategize in battle

  //Life Skills - stats affecting specific interactions
  std::int_least16_t m_sidearmSkill = 0;   // 0 - Skill with small guns
  std::int_least16_t m_longarmSkill = 0;   // 1 - Skill with big guns
  std::int_least16_t m_meleeSkill = 0;     // 2 - Skill with melee weapons
  std::int_least16_t m_barterSkill = 0;    // 3 - Skill affecting cost of items
  std::int_least16_t m_negotiateSkill = 0; // 4 - Skill affecting chance of successful negotiations*/
#endif
