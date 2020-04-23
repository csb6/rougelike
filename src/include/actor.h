#ifndef ACTOR_H
#define ACTOR_H
#include "item.h"
#include <array>
#include <tuple>

struct ActorId { unsigned short v; };
struct Energy { unsigned int v; };
struct Strength { unsigned int v; };
struct Health { int v; }; // can have negative health deltas
using ActorType = std::tuple<char, std::string, Strength, Weight>;

constexpr ActorId InitActorId{0};

struct ActorTypeTable {
    std::vector<char> ids;
    std::vector<std::string> names;
    std::vector<Strength> strengths;
    std::vector<Weight> max_carries;

    char add(char ch, std::string name, Strength strength, Weight max_carry);
    char add_tuple(const ActorType &new_type);
    bool contains(char actor) const;
};

struct ActorTable {
    std::vector<ActorId> ids;
    std::vector<Health> healths;
    std::vector<Energy> energies;
    std::vector<Weight> carries;
    std::vector<char> types;

    ActorId current_turn = InitActorId;
    std::size_t turn_index = 0;
    ActorId id_count = InitActorId;
    std::size_t player_index = 0;
    int player_x = 0;
    int player_y = 0;
    inline ActorId player() const { return ids[player_index]; }
    inline ActorId curr() const { return ids[turn_index]; }

    ActorId add(char type, Energy energy, Health health);
    void next_turn();
    void add_health(ActorId actor, Health amount);    
};

struct ActorInventoryTable {
    std::vector<ActorId> actor_ids;
    std::vector<char> items;
    std::vector<std::size_t> amounts;
};

struct ActorEquipmentTable {
    std::vector<ActorId> actor_id;
    //std::vector<ItemId> item;
};

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
