#ifndef ACTOR_H
#define ACTOR_H
#include "item.h"
#include <map>

struct BattleStats {
    short attack;
    short defense;
    short ranged_attack;
};

struct Position {
    short x;
    short y;
};

using ItemAmount = unsigned short;
struct InventorySet {
    std::map<ItemId, ItemAmount> melee_weapon;
    std::map<ItemId, ItemAmount> ranged_weapon;
    std::map<ItemId, ItemAmount> armor;
    std::map<ItemId, ItemAmount> misc;
};


using ActorTypeId = unsigned short;
struct ActorTypeTable {
    Array<char, 30> icon;
    Array<std::string, 30> name;
    Array<short, 30> energy;
    Array<BattleStats, 30> base_battle_stats;

    ActorTypeId add(char, std::string, short, BattleStats);
    std::pair<bool, ActorTypeId> find(char icon_) const;
    const size_t player_type_id = 0;
};

using ActorId = short;
struct ActorTable {
    Array<ActorTypeId, 30> type;
    Array<std::string, 30> name;
    Array<Position, 30> position;
    Array<short, 30> health;
    Array<InventorySet, 30> inventory;

    ActorId add(ActorTypeId, std::string, Position, short);
    bool subtract_health(ActorId, short amount);
    const size_t player_id = 0;
    Position player_pos() const { return position[player_id]; }
};

// 0. Head, 1. Chest, 2. Legs, 3. Feet, 4. Melee, 5. Ranged
/*constexpr int EquipSlotCount = 6;

enum class EquipSlot : char {
    Head, Chest, Legs, Feet, Melee, Ranged
};

struct ActorEquipment {
    ItemId head;
    ItemId chest;
    ItemId legs;
    ItemId feet;
    ItemId melee;
    ItemId ranged;
};

struct ActorEquipmentTable {
    SortedArray<ActorId, 30> actor_ids;
    Array<ActorEquipment, 30> equipments{};

    void equip(ActorId, EquipSlot, char item_type);
    char deequip(ActorId, EquipSlot);
    char equipment_at(ActorId, EquipSlot) const;
    char to_bits(ActorId) const;
};

constexpr int SkillAmount = 9; //Number of skills (e.g. strengh, agility, etc.)
constexpr int MaxInitPoints = 25; //Total pts doled out at character creation

  std::int_least16_t m_carryWeight = 0; //Current weight of inventory
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
