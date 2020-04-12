#ifndef ACTOR_H
#define ACTOR_H
#include "item.h"
#include <array>
#include <tuple>

using ActorId = unsigned short;
using ActorTypeId = char;
using Position = std::array<int,2>;
using Energy = unsigned int;
using Strength = unsigned int;
using Health = int; // can have negative health deltas
using Weight = unsigned int;
using ActorType = std::tuple<ActorTypeId, std::string, Strength, Weight>;

struct ActorTypeTable {
    std::vector<ActorTypeId> id;
    std::vector<std::string> name;
    std::vector<Strength> strength;
    std::vector<Weight> max_carry;

    ActorTypeId add(char ch, std::string name, Strength strength, Weight max_carry);
    ActorTypeId add_tuple(const ActorType &new_type);
    Weight get_max_carry(ActorTypeId type) const;
};

struct ActorTable {
    std::vector<ActorId> id;
    std::vector<Position> position; // (x, y)
    std::vector<Health> health;
    std::vector<Energy> energy;
    std::vector<ActorTypeId> actor_type;

    ActorId current_turn = 0;
    std::size_t turn_index = 0;
    ActorId id_count = 0;
    std::size_t player_index = 0;

    ActorId add(ActorTypeId type, Position pos, Energy energy, Health health);
    void move(ActorId actor, Position newPos);
    void next_turn();
    void add_health(ActorId actor, Health amount);    
    Position get_position(ActorId actor) const;
};

struct ActorInventoryTable {
    std::vector<ActorId> actor_id;
    std::vector<std::vector<ItemId>> inventory;
    std::vector<Weight> carry;

    void append(ActorId actor, Weight carry);
    void insert(ActorId actor, Weight carry);
    Weight get_carry(ActorId actor) const;
    void add_item(ActorId actor, ItemId item);
};

struct ActorEquipmentTable {
    std::vector<ActorId> actor_id;
    //std::vector<ItemId> item;
};

bool can_carry(ActorId actor, Weight item, const ActorTable &actors,
               const ActorTypeTable &types, const ActorInventoryTable &inventories);
std::string get_name(ActorId actor, const ActorTable &actors,
                     const ActorTypeTable &types);

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
