#include "ModNerfHerder.h"
#include "ScriptMgr.h"
#include "Formulas.h"
#include "Config.h"
#include "World.h"
#include "WorldPacket.h"
#include "Unit.h"
#include "Creature.h"
#include "Player.h"
#include "Unit.h"
#include "Pet.h"
#include "Chat.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellInfo.h"
#include <unordered_map>
#include <ctime>
#include <random>
#include <chrono>

uint32_t NerfHerder_Enabled = 0;
uint32_t NerfHerder_PlayerLevelEnabled = 0;
uint32_t NerfHerder_ZoneLevelEnabled = 0;
uint32_t NerfHerder_ForcePvPEnabled = 0;
uint32_t NerfHerder_MaxPlayerLevel = 80;

uint32_t NerfHerder_Honor_Enabled = 0;
float NerfHerder_Honor_Rate = 0;
uint32_t NerfHerder_Honor_GreyEnabled = 0;
float NerfHerder_Honor_GreyRate = 0;
uint32_t NerfHerder_Honor_PlunderEnabled = 0;
uint32_t NerfHerder_Honor_PlunderAmountPerLevel = 0;

uint32_t NerfHerder_WorldBuff_Enabled = 0;
float NerfHerder_WorldBuff_KillChance = 0;
uint32_t NerfHerder_WorldBuff_Cooldown = 0;
uint32_t NerfHerder_WorldBuff_SpellId_01 = 0;
uint32_t NerfHerder_WorldBuff_SpellId_02 = 0;
uint32_t NerfHerder_WorldBuff_SpellId_03 = 0;
uint32_t NerfHerder_WorldBuff_Alliance_LastKillTime = 0;
uint32_t NerfHerder_WorldBuff_Alliance_LastBuffTime = 0;
uint32_t NerfHerder_WorldBuff_Horde_LastKillTime = 0;
uint32_t NerfHerder_WorldBuff_Horde_LastBuffTime = 0;

uint32_t NerfHerder_Battleground_Enabled = 0;

uint32_t NerfHerder_Battleground_Win_HKReward = 0;
uint32_t NerfHerder_Battleground_Win_RepReward = 0;
uint32_t NerfHerder_Battleground_Win_HonorReward = 0;
uint32_t NerfHerder_Battleground_Win_APReward = 0;
uint32_t NerfHerder_Battleground_Win_GoldReward = 0;
uint32_t NerfHerder_Battleground_Win_XPReward = 0;

uint32_t NerfHerder_Battleground_Lose_HKReward = 0;
uint32_t NerfHerder_Battleground_Lose_RepReward = 0;
uint32_t NerfHerder_Battleground_Lose_HonorReward = 0;
uint32_t NerfHerder_Battleground_Lose_APReward = 0;
uint32_t NerfHerder_Battleground_Lose_GoldReward = 0;
uint32_t NerfHerder_Battleground_Lose_XPReward = 0;

uint32_t NerfHerder_Battleground_PerKill_HKReward = 0;
uint32_t NerfHerder_Battleground_PerKill_RepReward = 0;
uint32_t NerfHerder_Battleground_PerKill_HonorReward = 0;
uint32_t NerfHerder_Battleground_PerKill_APReward = 0;
uint32_t NerfHerder_Battleground_PerKill_GoldReward = 0;
uint32_t NerfHerder_Battleground_PerKill_XPReward = 0;

float NerfHerder_Battleground_DamageRate = 0;
float NerfHerder_Battleground_HealingRate = 0;

uint32_t NerfHerder_PartyNerf_Enabled = 0;
uint32_t NerfHerder_RaidFixes_Enabled = 0;

uint32_t NerfHerder_Heroics_Enabled = 0;
float NerfHerder_Heroics_DamageRate = 0;

class NerfHerderConfig : public WorldScript
{
public:
    NerfHerderConfig() : WorldScript("NerfHerderConfig") {}

    void OnBeforeConfigLoad(bool reload)
    {
        if (!reload)
        {
            SetInitialWorldSettings();
        }
    }

    void SetInitialWorldSettings()
    {
        // pull configs
        NerfHerder_MaxPlayerLevel = sConfigMgr->GetOption<int>("MaxPlayerLevel", 80); // <-- from worldserver.conf
        NerfHerder_Enabled = sConfigMgr->GetOption<int>("NerfHerder.Enabled", 0);
        NerfHerder_PlayerLevelEnabled = sConfigMgr->GetOption<int>("NerfHerder.PlayerLevelEnabled", 0);
        NerfHerder_ZoneLevelEnabled = sConfigMgr->GetOption<int>("NerfHerder.ZoneLevelEnabled", 0);
        NerfHerder_ForcePvPEnabled = sConfigMgr->GetOption<int>("NerfHerder.ForcePvPEnabled", 0);

        NerfHerder_Honor_Enabled = sConfigMgr->GetOption<int>("NerfHerder.Honor.Enabled", 0);
        NerfHerder_Honor_Rate = sConfigMgr->GetOption<float>("NerfHerder.Honor.Rate", 1);
        NerfHerder_Honor_GreyEnabled = sConfigMgr->GetOption<int>("NerfHerder.Honor.GreyEnabled", 0);
        NerfHerder_Honor_GreyRate = sConfigMgr->GetOption<float>("NerfHerder.Honor.GreyRate", 1);
        NerfHerder_Honor_PlunderEnabled = sConfigMgr->GetOption<int>("NerfHerder.Honor.PlunderEnabled", 0);
        NerfHerder_Honor_PlunderAmountPerLevel = sConfigMgr->GetOption<int>("NerfHerder.Honor.PlunderAmountPerLevel", 0);

        NerfHerder_WorldBuff_Enabled = sConfigMgr->GetOption<int>("NerfHerder.WorldBuff.Enabled", 0);
        NerfHerder_WorldBuff_KillChance = sConfigMgr->GetOption<float>("NerfHerder.WorldBuff.KillChance", 0);
        NerfHerder_WorldBuff_Cooldown = sConfigMgr->GetOption<int>("NerfHerder.WorldBuff.Cooldown", 0);
        NerfHerder_WorldBuff_SpellId_01 = sConfigMgr->GetOption<int>("NerfHerder.WorldBuff.SpellId.01", 0);
        NerfHerder_WorldBuff_SpellId_02 = sConfigMgr->GetOption<int>("NerfHerder.WorldBuff.SpellId.02", 0);
        NerfHerder_WorldBuff_SpellId_03 = sConfigMgr->GetOption<int>("NerfHerder.WorldBuff.SpellId.03", 0);

        NerfHerder_Battleground_Enabled = sConfigMgr->GetOption<int>("NerfHerder.Battleground.Enabled", 0);
        NerfHerder_Battleground_DamageRate = sConfigMgr->GetOption<float>("NerfHerder.Battleground.DamageRate", 0);
        NerfHerder_Battleground_HealingRate = sConfigMgr->GetOption<float>("NerfHerder.Battleground.HealingRate", 0);

        NerfHerder_Battleground_Win_HKReward = sConfigMgr->GetOption<int>("NerfHerder.Battleground.Win.HKReward", 0);
        NerfHerder_Battleground_Win_RepReward = sConfigMgr->GetOption<int>("NerfHerder.Battleground.Win.RepReward", 0);
        NerfHerder_Battleground_Win_HonorReward = sConfigMgr->GetOption<int>("NerfHerder.Battleground.Win.HonorReward", 0);
        NerfHerder_Battleground_Win_APReward = sConfigMgr->GetOption<int>("NerfHerder.Battleground.Win.APReward", 0);
        NerfHerder_Battleground_Win_GoldReward = sConfigMgr->GetOption<int>("NerfHerder.Battleground.Win.GoldReward", 0);
        NerfHerder_Battleground_Win_XPReward = sConfigMgr->GetOption<int>("NerfHerder.Battleground.Win.XPReward", 0);

        NerfHerder_Battleground_Lose_HKReward = sConfigMgr->GetOption<int>("NerfHerder.Battleground.Lose.HKReward", 0);
        NerfHerder_Battleground_Lose_RepReward = sConfigMgr->GetOption<int>("NerfHerder.Battleground.Lose.RepReward", 0);
        NerfHerder_Battleground_Lose_HonorReward = sConfigMgr->GetOption<int>("NerfHerder.Battleground.Lose.HonorReward", 0);
        NerfHerder_Battleground_Lose_APReward = sConfigMgr->GetOption<int>("NerfHerder.Battleground.Lose.APReward", 0);
        NerfHerder_Battleground_Lose_GoldReward = sConfigMgr->GetOption<int>("NerfHerder.Battleground.Lose.GoldReward", 0);
        NerfHerder_Battleground_Lose_XPReward = sConfigMgr->GetOption<int>("NerfHerder.Battleground.Lose.XPReward", 0);

        NerfHerder_Battleground_PerKill_HKReward = sConfigMgr->GetOption<int>("NerfHerder.Battleground.PerKill.HKReward", 0);
        NerfHerder_Battleground_PerKill_RepReward = sConfigMgr->GetOption<int>("NerfHerder.Battleground.PerKill.RepReward", 0);
        NerfHerder_Battleground_PerKill_HonorReward = sConfigMgr->GetOption<int>("NerfHerder.Battleground.PerKill.HonorReward", 0);
        NerfHerder_Battleground_PerKill_APReward = sConfigMgr->GetOption<int>("NerfHerder.Battleground.PerKill.APReward", 0);
        NerfHerder_Battleground_PerKill_GoldReward = sConfigMgr->GetOption<int>("NerfHerder.Battleground.PerKill.GoldReward", 0);
        NerfHerder_Battleground_PerKill_XPReward = sConfigMgr->GetOption<int>("NerfHerder.Battleground.PerKill.XPReward", 0);

        NerfHerder_PartyNerf_Enabled = sConfigMgr->GetOption<int>("NerfHerder.PartyNerfEnabled", 0);
        NerfHerder_RaidFixes_Enabled = sConfigMgr->GetOption<int>("NerfHerder.RaidFixesEnabled", 0);

        NerfHerder_Heroics_Enabled = sConfigMgr->GetOption<float>("NerfHerder.Heroics.Enabled", 0);
        NerfHerder_Heroics_DamageRate = sConfigMgr->GetOption<float>("NerfHerder.Heroics.DamageRate", 0);
    }
};

class NerfHerderCreatureInfo : public DataMap::Base
{
public:
    NerfHerderCreatureInfo() {}

    uint32_t is_brand_new = 1;
    uint32_t is_altered = 0;

    uint64_t last_second = 0; // for tracking timestamps
    uint64_t last_minute = 0; // for tracking timestamps

    uint32_t original_level = 0;
    uint32_t original_health = 0;
    uint32_t original_armor = 0;

    uint32_t new_level = 0;
    uint32_t new_health = 0;
    uint32_t new_armor = 0;
};

struct VendorData {
    uint32_t expansionID; // 1=TBC, 2=WOTLK
};

struct TownData {
    uint32_t teamID; // 0=neutral, 1=alliance, 2=horde
    uint32_t isCapitolCity; // 0=false, 1=true
};

struct ZoneData {
    uint32_t minLevel;
    uint32_t maxLevel;
};

// zone_id, aura_id
struct raidDisallowedAurasData {
    uint32_t map_id;
    uint32_t aura_id; // spell id
};

// zone_id, unit_id
struct raidDisallowedMobsData {
    uint32_t map_id;
    uint32_t creature_id;
};

// zone_id, unit_id, health % to start bleeding, heath % stop bleeding, amount to bleed per second
struct raidBleedingMobsData {
    uint32_t map_id;
    uint32_t creature_id;
    uint32_t health_percentage_start;
    uint32_t health_percentage_stop;
    float damage;
};

// map_id, creature_id, multiplier (.1 = 90% reduction)
struct raidHealMultiMobsData {
    uint32_t map_id;
    uint32_t creature_id;
    float multi;
};

// zone_id, unit_id, dmg multi (.1 = 90% reduction)
struct raidDmgMultiMobsData {
    uint32_t map_id;
    uint32_t creature_id;
    float multi;
};

// zone_id, unit_id, spell_id, dmg multi
struct raidDmgMultiSpellsData {
    uint32_t map_id;
    uint32_t creature_id;
    uint32_t spell_id;
    float multi;
};

class NerfHerderHelper
{
public:
    static std::unordered_map<uint32_t, ZoneData> zoneDataMap;
    static std::unordered_map<uint32_t, TownData> townDataMap;

    static std::vector<raidDisallowedAurasData> raidDisallowedAurasMap;
    static std::vector<raidDisallowedMobsData> raidDisallowedMobsMap;
    static std::vector<raidBleedingMobsData> raidBleedingMobsMap;
    static std::vector<raidHealMultiMobsData> raidHealMultiMobsMap;
    static std::vector<raidDmgMultiMobsData> raidDmgMultiMobsMap;
    static std::vector<raidDmgMultiSpellsData> raidDmgMultiSpellsMap;

    static uint32 GetBattlegroundFactionId(Player* player)
    {
        uint32 faction_id = 0;
        switch (player->GetMapId())
        {
            case MAP_WARSONG_GULCH:
                // Warsong Outriders (Horde) / Silverwing Sentinels (Alliance)
                faction_id = player->GetTeamId() == TEAM_ALLIANCE ? 890 : 889;
                break;
            case MAP_ARATHI_BASIN:
                // The Defilers (Horde) / The League of Arathor (Alliance)
                faction_id = player->GetTeamId() == TEAM_ALLIANCE ? 509 : 510;
                break;
            case MAP_ALTERAC_VALLEY:
                // Frostwolf Clan (Horde) / Stormpike Guard (Alliance)
                faction_id = player->GetTeamId() == TEAM_ALLIANCE ? 730 : 729;
                break;
            case MAP_EYE_OF_THE_STORM:
                // no factions on this one
                break;
            case MAP_STRAND_OF_THE_ANCIENTS:
                // The Valiance Expedition is the Alliance faction for the battleground,
                // and the Warsong Offensive is the Horde faction. However, no reputation
                // is earned by competing in this battleground.
                break;
            case MAP_ISLE_OF_CONQUEST:
                // no factions on this one??
                break;
        }

        return faction_id;
    }

    static void GrantRewardGold(Player* player, uint32_t amount, uint32_t is_announce = 0)
    {
        uint32_t currentmoney = player->GetMoney();
        uint32_t rewardmoney = player->GetLevel() * amount; // in copper from config

        if (is_announce)
        {
            std::ostringstream ss;
            ss << "|cffffc107You have been awarded " << (rewardmoney / 100 / 100) << " gold.|r";
            ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str());
        }

        // add bonus gold
        player->SetMoney(currentmoney + rewardmoney);
    }

    static void GrantRewardHK(Player* player, uint32_t amount, uint32_t is_announce = 0)
    {
        if (is_announce)
        {
            std::ostringstream ss;
            ss << "|cff2196f3You have been awarded " << amount << " honorable kills.|r";
            ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str());
        }

        // amend stats
        player->ApplyModUInt32Value(PLAYER_FIELD_KILLS, amount, true);
        player->ApplyModUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS, amount, true);

        // trigger achieves
        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_EARN_HONORABLE_KILL);
        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL_AT_AREA, player->GetAreaId());
    }

    static void GrantRewardHonor(Player* player, uint32_t amount, uint32_t is_announce = 0)
    {
        if (is_announce)
        {
            std::ostringstream ss;
            ss << "|cff2196f3You have been awarded " << amount << " honor.|r";
            ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str());
        }

        // add bonus honor
        player->ModifyHonorPoints(amount);
        player->ApplyModUInt32Value(PLAYER_FIELD_TODAY_CONTRIBUTION, amount, true);
    }

    static void GrantRewardArena(Player* player, uint32_t amount, uint32_t is_announce = 0)
    {
        if (is_announce)
        {
            std::ostringstream ss;
            ss << "|cff2196f3You have been awarded " << amount << " arena points.|r";
            ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str());
        }

        // add bonus arena points
        player->ModifyArenaPoints(amount);
    }

    static void GrantRewardRep(Player* player, uint32_t faction_id, uint32_t amount, uint32_t is_announce = 0)
    {
        if (is_announce)
        {
            // game reports this to player already
        }

        // get current rep
        uint32 current = player->GetReputation(faction_id);
        player->SetReputation(faction_id, current + amount);
    }

    static void GrantRewardXP(Player* player, uint32_t percentage, uint32_t is_announce = 0)
    {
        if (is_announce)
        {
            std::ostringstream ss;
            ss << "|cff7e57c2You have been awarded " << percentage << " percent experience.|r";
            ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str());
        }

        // add bonus gold
        uint32 basexp = sObjectMgr->GetXPForLevel(player->GetLevel());
        float bonusxp = static_cast<float>(basexp) * (static_cast<float>(percentage) / 100);
        player->GiveXP(static_cast<uint32>(bonusxp), nullptr);
    }

    static void ApplyWorldBuff(Player* player)
    {
        // if disabled, bail
        if (!NerfHerder_WorldBuff_Enabled) return;

        // log the time
        uint32_t timestamp = std::time(nullptr);

        // Seed the random number generator
        std::random_device rd;
        std::mt19937 gen(rd());

        // Define the range for random percentage
        double minPercentage = 0.0;
        double maxPercentage = 1.0;

        // Generate a random percentage
        std::uniform_real_distribution<double> dis(minPercentage, maxPercentage);
        double randomNumber = dis(gen);

        // if we don't roll to trigger world buff, bail
        if (randomNumber > NerfHerder_WorldBuff_KillChance)
            return;

        // flight checks
        if (player->GetTeamId() == TEAM_ALLIANCE)
        {
            // has it been enough time since last world buff?  if not, bail
            if ((timestamp - NerfHerder_WorldBuff_Alliance_LastBuffTime) < (NerfHerder_WorldBuff_Cooldown * 60))
                return;

            // at this point, we are going to world buff so log it
            NerfHerder_WorldBuff_Alliance_LastBuffTime = timestamp;
        }
        else if (player->GetTeamId() == TEAM_HORDE)
        {
            // has it been enough time since last world buff?  if not, bail
            if ((timestamp - NerfHerder_WorldBuff_Horde_LastBuffTime) < (NerfHerder_WorldBuff_Cooldown * 60))
                return;

            // at this point, we are going to world buff so log it
            NerfHerder_WorldBuff_Horde_LastBuffTime = timestamp;
        }

        // choose which buff to give
        uint32_t chosen_buff = 0;
        uint rand_number = urand(1, 3);
        if (rand_number == 1)
            chosen_buff = NerfHerder_WorldBuff_SpellId_01;
        else if (rand_number == 2)
            chosen_buff = NerfHerder_WorldBuff_SpellId_02;
        else if (rand_number == 3)
            chosen_buff = NerfHerder_WorldBuff_SpellId_03;

        // apply world buff to all players
        player->GetMap()->DoForAllPlayers([&](Player* p)
        {
            uint32_t is_faction_match = 0;
            if (player->GetTeamId() == TEAM_ALLIANCE) is_faction_match = p->GetTeamId() == TEAM_ALLIANCE ? 1 : 0;
            if (player->GetTeamId() == TEAM_HORDE) is_faction_match = p->GetTeamId() == TEAM_HORDE ? 1 : 0;

            // no matter where they are, buff all players
            if (p->IsAlive() && p->IsPlayer() && is_faction_match)
            {
                p->CastSpell(p, chosen_buff, true);
            }
        });
    }

    static uint32_t IsFieldAgent(Creature* creature)
    {
        // this will get the area id (as opposed to zone id or map id)
        uint32_t area_id = creature->GetAreaId();

        // find any NPC NOT in an alliance/horde town
        if (NerfHerderHelper::townDataMap.find(area_id) == NerfHerderHelper::townDataMap.end())
        {
            // not in a town, mark as false
            return 0;
        }

        // yes, this npc is in a faction town
        return 1;
    }

    static uint32_t GetZoneLevel(Creature* creature)
    {
        uint32_t zone_id = creature->GetZoneId();

        if (NerfHerderHelper::zoneDataMap.find(zone_id) == NerfHerderHelper::zoneDataMap.end())
            return 0;

        return NerfHerderHelper::zoneDataMap[zone_id].maxLevel;
    }

    static int32_t GetNerfValueHP(uint32_t current_health)
    {
        return 0; // disable this for now, never nerf health

        // no longer nerfing npcs based on high health, going to
        // nerf world bosses individually instead.

        /*
        if (!NerfHerder_WorldEvent_Enabled) return 0;
        if (!NerfHerder_WorldEvent_MaxHealth) return 0;
        if (current_health <= NerfHerder_WorldEvent_MaxHealth) return 0;

        // Calculate health ratio based on max_allowed_health
        float health_ratio = static_cast<float>(NerfHerder_WorldEvent_MaxHealth) / static_cast<float>(current_health);

        // Calculate health nerf percentage
        int32_t health_nerf = static_cast<int>((1.0f - health_ratio) * 100.0f);

        // Ensure the nerf is within the -1 to -99 range
        health_nerf = std::min(-1, std::max(-99, -health_nerf));

        // Return
        return health_nerf;
        */
    }

    static int32_t GetNerfValue(uint32_t current_level, uint32_t new_level)
    {
        // calc proportional level change
        float ratio = static_cast<float>(new_level) / static_cast<float>(current_level);

        // calc nerf multiplier (negative)
        float multiplier = -100 + (ratio * 100);

        // increase nerf slightly (to account for exponential differences)
        multiplier = multiplier * 1.25;

        // make it an integer
        int32_t negative_multiplier = static_cast<int>(multiplier);

        // check for errors
        if (negative_multiplier > 0) negative_multiplier = -1;
        if (negative_multiplier < -100) negative_multiplier = -99;

        // return
        return negative_multiplier;
    }

    static bool HasTimeElapsed(Unit* unit, uint32_t seconds)
    {
        // don't run invalid mobs
        if (!unit || !unit->IsAlive() || unit->IsDuringRemoveFromWorld())
            return false;

        // load info
        NerfHerderCreatureInfo *creatureInfo = unit->CustomData.GetDefault<NerfHerderCreatureInfo>("NerfHerderCreatureInfo");

        // get the last timestamp saved
        uint32_t last = 0;
        if (seconds == 1)
            last = creatureInfo->last_second;
        else
            last = creatureInfo->last_minute;

        // get the current time
        uint32_t time = GetEpochTime().count();

        // if this creature had no last timestamp...
        if (!last)
        {
            // save current time
            if (seconds == 1)
                creatureInfo->last_second = time;
            else
                creatureInfo->last_minute = time;

            // return
            return false;
        }

        // if desired time has passed...
        if (time >= (last + seconds))
        {
            // save current time
            if (seconds == 1)
                creatureInfo->last_second = time;
            else
                creatureInfo->last_minute = time;

            // return
            return true;
        }

        // return
        return false;
    }

    static void ResetCreatureWorld(Creature* creature)
    {
        if (creature->isDead()) return; // go no further

        // load info
        NerfHerderCreatureInfo *creatureInfo = creature->CustomData.GetDefault<NerfHerderCreatureInfo>("NerfHerderCreatureInfo");

        // if not a modified creature, bail...
        if (!creatureInfo->is_altered) return;

        // we would reset if evading, or if respawned...
        if (creature->IsInEvadeMode() || creatureInfo->new_level != creature->GetLevel())
        {
            // amend logs
            creatureInfo->is_altered = 0;

            // nerf auras
            uint32_t HpAura = 89501;
            uint32_t DamageDoneTakenAura = 89502;
            uint32_t BaseStatAura = 89503;
            uint32_t AbsorbAura = 89505;
            uint32_t HealingAura = 89506;
            uint32_t PhysicalDamageTakenAura = 89507;

            // remove auras
            creature->RemoveAura(HpAura);
            creature->RemoveAura(DamageDoneTakenAura);
            creature->RemoveAura(BaseStatAura);
            creature->RemoveAura(AbsorbAura);
            creature->RemoveAura(HealingAura);
            creature->RemoveAura(PhysicalDamageTakenAura);

            // reset level
            creature->SetLevel(creatureInfo->original_level); // happens so fast you never see it
        }
    }

    static void UpdateCreatureWorld(Creature* creature, uint32_t new_level)
    {
        // load info
        NerfHerderCreatureInfo *creatureInfo = creature->CustomData.GetDefault<NerfHerderCreatureInfo>("NerfHerderCreatureInfo");

        // if creature is already altered, bail...
        if (creatureInfo->is_altered) return;

        // if first time...
        if (creatureInfo->is_brand_new)
        {
            // log original numbers
            creatureInfo->original_level = creature->GetLevel();
            creatureInfo->original_health = creature->GetMaxHealth();
            creatureInfo->original_armor = creature->GetArmor();

            // log changes
            creatureInfo->new_level = new_level;

            // mark as not new anymore
            creatureInfo->is_brand_new = 0;
        }

        // get nerf multiplier
        int32_t negative_multiplier_hp = NerfHerderHelper::GetNerfValueHP(creatureInfo->original_health);
        int32_t negative_multiplier = NerfHerderHelper::GetNerfValue(creatureInfo->original_level, new_level);
        int32_t positive_multiplier = -1 * negative_multiplier;

        // make sure numbers make sense
        if (negative_multiplier > 0) negative_multiplier = -1;
        if (negative_multiplier < -100) negative_multiplier = -99;
        if (negative_multiplier_hp > 0) negative_multiplier_hp = -1;
        if (negative_multiplier_hp < -100) negative_multiplier_hp = -99;

        // some assumptions here:
        // the proportional difference between a lvl 80 and lvl 60 is 25%, but
        // the dmg done by a lvl 80 is not only 25% higher than a lvl 60, it's
        // probably closer to 3x as much!  so our nerf needs to not be a linear
        // or proportional nerf, it needs to curve.
        // AT THE PRESENT TIME I WILL DO NOTHING
        // https://us.forums.blizzard.com/en/wow/t/a-look-back-at-health-values/587645

        // nerf auras
        uint32_t HpAura = 89501;
        uint32_t DamageDoneTakenAura = 89502;
        //uint32_t BaseStatAura = 89503;
        uint32_t AbsorbAura = 89505;
        uint32_t HealingAura = 89506;
        uint32_t PhysicalDamageTakenAura = 89507;

        // You can't mix HpAura and BaseStatAura????  HpAura cannot be removed after it's given.

        // nerf their damage done, base stats, absorbsion, and healing done
        creature->CastCustomSpell(creature, HpAura, &negative_multiplier_hp, NULL, NULL, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, DamageDoneTakenAura, 0, &negative_multiplier, NULL, true, NULL, NULL, creature->GetGUID());
        //creature->CastCustomSpell(creature, BaseStatAura, &negative_multiplier, &negative_multiplier, &negative_multiplier, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, AbsorbAura, &negative_multiplier, NULL, NULL, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, HealingAura, &negative_multiplier, NULL, NULL, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, PhysicalDamageTakenAura, &positive_multiplier, NULL, NULL, true, NULL, NULL, creature->GetGUID());

        // set new level
        creature->SetLevel(new_level, false); // flag false to bypass any hooray animations

        // amend logs
        creatureInfo->is_altered = 1;
    }

    static void ProcessCreatureWorld(Creature* creature)
    {
        // catch errors
        if (!NerfHerder_Enabled) return;

        // catch errors
        if (!creature) return;
        if (creature->IsPlayer() || creature->IsPet()) return;

        // DO NOT RUN ON NPC BOTS (COMMENT OUT IF NOT USING NPCBOTS!)
        if (creature->IsNPCBotOrPet()) return;

        // do not run in instances
        if (creature->GetMap()->IsDungeon() || creature->GetMap()->IsRaid() || creature->GetMap()->IsBattlegroundOrArena()) return;

        // load info
        NerfHerderCreatureInfo *creatureInfo = creature->CustomData.GetDefault<NerfHerderCreatureInfo>("NerfHerderCreatureInfo");

        // reset (this checks a mob to see if they need to be renerfed, and will do so)
        NerfHerderHelper::ResetCreatureWorld(creature);

        // if not a modified creature (we only will do this once)...
        if (!creatureInfo->is_altered)
        {
            // init
            uint32_t max_level;

            // determine alliance / horde npcs in the world
            uint32_t is_field_agent = NerfHerderHelper::IsFieldAgent(creature);

            // if max zone level is enabled...
            if (NerfHerder_ZoneLevelEnabled)
            {
                if (is_field_agent) // this won't catch goblin guards in gadgetzan bc it's not faction?
                {
                    // get max level for zone
                    max_level = NerfHerderHelper::GetZoneLevel(creature);

                    // if valid
                    if (max_level && max_level >= 10)
                    {
                        // if creature is too high...
                        if (creature->GetLevel() > max_level)
                        {
                            // nerf em
                            NerfHerderHelper::UpdateCreatureWorld(creature, max_level); // don't add additional nerf to these guys
                        }
                    }
                }
            }

            // if max player level is enabled...
            if (NerfHerder_PlayerLevelEnabled)
            {
                // what is max level allowed
                max_level = NerfHerder_MaxPlayerLevel;

                // if valid
                if (max_level && max_level >= 10)
                {
                    // if creature is too high...
                    if (creature->GetLevel() > max_level)
                    {
                        // calc new max level
                        max_level = creature->isElite() ? max_level : max_level - 5;

                        // nerf em
                        NerfHerderHelper::UpdateCreatureWorld(creature, max_level);
                    }
                }
            }

            // if force pvp is enabled...
            if (NerfHerder_ForcePvPEnabled)
            {
                if (is_field_agent)
                {
                    // force them to be pvp
                    creature->SetPvP(true);
                }
            }
        }
    }

    static void ResetPlayerParty(Player* creature)
    {
        // if not a modified creature, bail...
        if (!creature->HasAura(89501) && !creature->HasAura(89502) && !creature->HasAura(89503) && !creature->HasAura(89505) && !creature->HasAura(89506) && !creature->HasAura(89507))
            return;

        // load info
        NerfHerderCreatureInfo *creatureInfo = creature->CustomData.GetDefault<NerfHerderCreatureInfo>("NerfHerderCreatureInfo");

        // if not a modified creature, bail...
        //if (!creatureInfo->is_altered) return;

        // nerf auras
        uint32_t HpAura = 89501;
        uint32_t DamageDoneTakenAura = 89502;
        uint32_t BaseStatAura = 89503;
        uint32_t AbsorbAura = 89505;
        uint32_t HealingAura = 89506;
        uint32_t PhysicalDamageTakenAura = 89507;

        // remove auras
        creature->RemoveAura(HpAura);
        creature->RemoveAura(DamageDoneTakenAura);
        creature->RemoveAura(AbsorbAura);
        creature->RemoveAura(BaseStatAura);
        creature->RemoveAura(HealingAura);
        creature->RemoveAura(PhysicalDamageTakenAura);

        // amend logs
        creatureInfo->new_level = 0;
        creatureInfo->is_altered = 0;
    }

    static void UpdatePlayerParty(Player* creature, uint32_t new_level)
    {
        // load info
        NerfHerderCreatureInfo *creatureInfo = creature->CustomData.GetDefault<NerfHerderCreatureInfo>("NerfHerderCreatureInfo");

        // if we don't need to make a change, bail
        if (creatureInfo->new_level == new_level) return;

        // calc nerf value
        //int32_t negative_multiplier_hp = NerfHerderHelper::GetNerfValueHP(creature->GetMaxHealth());
        int32_t negative_multiplier = NerfHerderHelper::GetNerfValue(creature->GetLevel(), new_level);
        int32_t positive_multiplier = -1 * negative_multiplier;

        // nerf auras
        uint32_t HpAura = 89501;
        uint32_t DamageDoneTakenAura = 89502;
        //uint32_t BaseStatAura = 89503;
        uint32_t AbsorbAura = 89505;
        uint32_t HealingAura = 89506;
        uint32_t PhysicalDamageTakenAura = 89507;

        // nerf their damage done, base stats, absorbsion, and healing done
        creature->CastCustomSpell(creature, HpAura, &negative_multiplier, NULL, NULL, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, DamageDoneTakenAura, 0, &negative_multiplier, NULL, true, NULL, NULL, creature->GetGUID());
        //creature->CastCustomSpell(creature, BaseStatAura, &negative_multiplier, &negative_multiplier, &negative_multiplier, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, AbsorbAura, &negative_multiplier, NULL, NULL, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, HealingAura, &negative_multiplier, NULL, NULL, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, PhysicalDamageTakenAura, &positive_multiplier, NULL, NULL, true, NULL, NULL, creature->GetGUID());

        // prob need to amend player pet too??

        // amend logs
        creatureInfo->new_level = new_level;
        creatureInfo->is_altered = 1;
    }

    static void ProcessPlayerParty(Player* creature)
    {
        // catch errors
        if (!NerfHerder_Enabled) return;
        if (!NerfHerder_PartyNerf_Enabled) return;

        // catch errors
        if (!creature) return;
        if (creature->isDead()) return;

        // if in a group (and not in a battleground)...
        if (Group* group = creature->GetGroup())
        {
            if (!group->isRaidGroup() && !creature->GetMap()->IsBattlegroundOrArena())
            {
                uint32_t lowestLevel = creature->GetLevel(); // Initialize with the current player's level

                for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
                {
                    Player* member = itr->GetSource();
                    if (member)
                    {
                        if (member->GetLevel() < lowestLevel)
                        {
                            lowestLevel = member->GetLevel();
                        }
                    }
                }

                // if lowest lvl teammate is more than 5 levels below you...
                if (lowestLevel < (creature->GetLevel() - 5))
                {
                    NerfHerderHelper::UpdatePlayerParty(creature, lowestLevel);
                }
                else
                {
                    NerfHerderHelper::ResetPlayerParty(creature);
                }
            }
            else
            {
                NerfHerderHelper::ResetPlayerParty(creature);
            }
        }
        else
        {
            NerfHerderHelper::ResetPlayerParty(creature);
        }
    }

    static void PvPBattlegroundOnKill(Player* player)
    {
        // this is running at the end of a battleground, so we multiply
        // by the honorable kill count that the player had

        if (!NerfHerder_Enabled) return;
        if (!NerfHerder_Battleground_Enabled) return;

        // if not battleground, bail
        if (!player->GetMap()->IsBattlegroundOrArena()) return;

        if (NerfHerder_Battleground_PerKill_GoldReward)
        {
            NerfHerderHelper::GrantRewardGold(player, NerfHerder_Battleground_PerKill_GoldReward);
        }

        if (NerfHerder_Battleground_PerKill_HKReward)
        {
            NerfHerderHelper::GrantRewardHK(player, NerfHerder_Battleground_PerKill_HKReward);
        }

        if (NerfHerder_Battleground_PerKill_HonorReward)
        {
            NerfHerderHelper::GrantRewardHonor(player, NerfHerder_Battleground_PerKill_HonorReward);
        }

        if (NerfHerder_Battleground_PerKill_APReward)
        {
            NerfHerderHelper::GrantRewardArena(player, NerfHerder_Battleground_PerKill_APReward);
        }

        if (NerfHerder_Battleground_PerKill_RepReward)
        {
            uint32 faction_id = NerfHerderHelper::GetBattlegroundFactionId(player);
            if (faction_id)
            {
                NerfHerderHelper::GrantRewardRep(player, faction_id, NerfHerder_Battleground_PerKill_RepReward);
            }
        }

        if (NerfHerder_Battleground_PerKill_XPReward)
        {
            // this should be a float but I don't want to fix it (so turning it off)
            //NerfHerderHelper::GrantRewardXP(player, NerfHerder_Battleground_PerKill_XPReward);
        }
    }

    static void PvPWorldOnKill(Player* player, Creature* killed)
    {
        if (!NerfHerder_Enabled) return;
        if (!NerfHerder_Honor_Enabled) return;

        // This was all taken straight from HonorGuard mod, but tweaked to
        // give honor on any pvp flagged creature.
        // https://github.com/azerothcore/mod-gain-honor-guard/blob/master/src/GainHonorGuard.cpp

        // catch errors
        if (player->GetMap()->IsDungeon() || player->GetMap()->IsRaid() || player->GetMap()->IsBattlegroundOrArena()) return;

        if (player->IsAlive() && !player->HasAura(SPELL_AURA_PLAYER_INACTIVE))
        {
            if (killed || !killed->HasAuraType(SPELL_AURA_NO_PVP_CREDIT))
            {
                if ((NerfHerder_Honor_Enabled && killed->ToCreature()->IsPvP()))
                {
                    std::ostringstream ss;
                    int honor = -1; //Honor is added as an int
                    float honor_f = (float)honor; //Convert honor to float for calculations
                    player->UpdateHonorFields();

                    //Determine level that is gray
                    uint8 k_level = player->GetLevel();
                    uint8 k_grey = Acore::XP::GetGrayLevel(k_level);
                    uint8 v_level = killed->GetLevel();

                    // handle grey setting
                    float honor_multiplier = NerfHerder_Honor_Rate;
                    if (v_level <= k_grey) // if npc was too low
                    {
                        if (NerfHerder_Honor_GreyEnabled)
                        {
                            v_level = k_grey + 1; // treat npc as just above limit
                            honor_multiplier = NerfHerder_Honor_GreyRate;
                        }
                    }

                    // If guard or elite is grey to the player then no honor rewarded
                    if (v_level > k_grey)
                    {
                        honor_f = ceil(Acore::Honor::hk_honor_at_level_f(k_level) * (v_level - k_grey) / (k_level - k_grey));

                        // count the number of playerkills in one day
                        player->ApplyModUInt32Value(PLAYER_FIELD_KILLS, 1, true);
                        // and those in a lifetime
                        player->ApplyModUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS, 1, true);
                        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_EARN_HONORABLE_KILL);
                        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HK_CLASS, killed->getClass());
                        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HK_RACE, killed->getRace());
                        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL_AT_AREA, player->GetAreaId());
                        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL, 1, 0, killed);

                        if (killed != nullptr)
                        {
                            // apply honor multiplier from aura (not stacking-get highest)
                            AddPct(honor_f, player->GetMaxPositiveAuraModifier(SPELL_AURA_MOD_HONOR_GAIN_PCT));
                        }

                        //Custom Gain Honor Rate
                        if (honor_multiplier)
                        {
                            honor_f *= honor_multiplier;
                        }
                        else
                        {
                            honor_f *= sWorld->getRate(RATE_HONOR);
                        }

                        //sLog->outError("%u: gained honor with a rate: %0.2f", player->GetGUID(), sWorld->getRate(RATE_HONOR));

                        // Convert Honor Back to an int to add to player
                        honor = int32(honor_f);

                        // if honor is actually to be given...
                        if (honor > 0)
                        {
                            // add honor points to player
                            player->ModifyHonorPoints(honor);
                            player->ApplyModUInt32Value(PLAYER_FIELD_TODAY_CONTRIBUTION, honor, true);

                            // announce to player if honor was gained
                            //std::ostringstream ss;
                            //ss << "|cff2196f3You have been awarded " << honor << " honor.|r";
                            //ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str());

                            // give plunder
                            if (NerfHerder_Honor_PlunderEnabled)
                            {
                                uint32_t currentmoney = player->GetMoney();
                                uint32_t givenmoney = (v_level * NerfHerder_Honor_PlunderAmountPerLevel); // the creature's level in silver

                                // Seed the random number generator
                                std::random_device rd;
                                std::mt19937 gen(rd());

                                // Define the range for random percentage (±20%)
                                double minPercentage = 0.8; // 80%
                                double maxPercentage = 1.2; // 120%

                                // Generate a random percentage
                                std::uniform_real_distribution<double> dis(minPercentage, maxPercentage);
                                double randomPercentage = dis(gen);

                                // Apply the random percentage to modify givenmoney
                                uint32_t modifiedmoney = static_cast<uint32_t>(givenmoney * randomPercentage);

                                // give them reward
                                player->SetMoney(currentmoney + modifiedmoney);

                                // report
                                //std::ostringstream ss;
                                //ss << "|cffffc107You have recovered " << (modifiedmoney / 100) << " silver.|r";
                                //ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str());
                            }

                            // apply world buff (if applicable)
                            NerfHerderHelper::ApplyWorldBuff(player);
                        }
                    }
                }
            }
        }
    }

    static uint32 NerfIncomingHeals(Unit* patient, Unit* healer, uint32 amount, uint32 spellID = 0)
    {
        float famount = static_cast<float>(amount);

        // catch errors...
        if (!patient) return static_cast<uint32>(amount);

        // if battleground...
        if (NerfHerder_Battleground_Enabled)
        {
            if (patient->GetMap()->IsBattlegroundOrArena())
                famount = famount * NerfHerder_Battleground_HealingRate;
        }

        // if custom end game fixes...
        if (NerfHerder_RaidFixes_Enabled)
        {
            // enact custom healing multipliers
            for (const auto& entry : NerfHerderHelper::raidHealMultiMobsMap)
            {
                if (patient->GetMapId() == entry.map_id && patient->GetEntry() == entry.creature_id)
                {
                    famount = famount * entry.multi;
                }
            }
        }

        return static_cast<uint32>(famount);
    }

    static uint32 NerfIncomingDamage(Unit* victim, Unit* attacker, uint32 damage, uint32 spellID = 0)
    {
        float fdamage = static_cast<float>(damage);

        // catch errors...
        if (!victim) return static_cast<uint32>(fdamage);

        // if battleground...
        if (NerfHerder_Battleground_Enabled)
        {
            if (victim->GetMap()->IsBattlegroundOrArena())
                fdamage = fdamage * NerfHerder_Battleground_DamageRate;
        }

        // if heroic...
        if (NerfHerder_Heroics_Enabled)
        {
            // if player or npcbot or pet of either
            bool isPlayerOrBotControlled = false;

            // CHANGE CONDITION IF NOT USING NPCBOTS!!
            if (attacker->IsPlayer() || attacker->IsNPCBotOrPet())
                isPlayerOrBotControlled = true;

            if (attacker->IsPet() && attacker->GetOwner())
            {
                 if (attacker->GetOwner()->IsPlayer())
                    isPlayerOrBotControlled = true;
            }

            if (!isPlayerOrBotControlled)
            {
                fdamage = fdamage * NerfHerder_Heroics_DamageRate;
            }
        }

        // if custom end game fixes...
        if (NerfHerder_RaidFixes_Enabled)
        {
            if (attacker)
            {
                // enact custom damage multipliers
                for (const auto& entry : NerfHerderHelper::raidDmgMultiMobsMap)
                {
                    if (victim->GetMapId() == entry.map_id && attacker->GetEntry() == entry.creature_id)
                    {
                        fdamage = fdamage * entry.multi;
                    }
                }

                // enact custom spell damage multipliers
                for (const auto& entry : NerfHerderHelper::raidDmgMultiSpellsMap)
                {
                    if (victim->GetMapId() == entry.map_id && (attacker->GetEntry() == entry.creature_id || !entry.creature_id) && spellID == entry.spell_id)
                    {
                        fdamage = fdamage * entry.multi;
                    }
                }

                // if dungeon or raid...
                if (victim->GetMap()->IsDungeon() || victim->GetMap()->IsRaid())
                {
                    // kara chess event horde king
                    if (victim->GetEntry() == 21752)
                    {
                        fdamage = 0; // you can't lose
                    }

                    // kara chess event (any alliance piece)
                    if (victim->GetEntry() == 21684 || victim->GetEntry() == 17211 || victim->GetEntry() == 21160 || victim->GetEntry() == 21664 || victim->GetEntry() == 21682|| victim->GetEntry() == 21683)
                    {
                        fdamage = fdamage * 10;
                    }
                }

                // if world...
                if (!victim->GetMap()->IsDungeon() && !victim->GetMap()->IsRaid() && !victim->GetMap()->IsBattlegroundOrArena())
                {
                    // World Faction Leaders
                    // Horde: Thrall (4949), Lady Sylvanis (10181), Cairne Bloodhoof (3057), Lor'themar Theron (16802)
                    // Alliance: King Varian (29611), King Mangi (2784), Tyrande Whisperwind (7999), Prophet Velen (17468)
                    if (attacker->GetEntry() == 4949 || attacker->GetEntry() == 10181 || attacker->GetEntry() == 3057 || attacker->GetEntry() == 16802 || attacker->GetEntry() == 29611 || attacker->GetEntry() == 2784 || attacker->GetEntry() == 7999 || attacker->GetEntry() == 17468)
                    {
                        if (NerfHerder_MaxPlayerLevel <= 60)
                            fdamage = fdamage * 0.33;

                        else if (NerfHerder_MaxPlayerLevel > 60 && NerfHerder_MaxPlayerLevel <= 70)
                            fdamage = fdamage * 0.66;
                    }
                }
            }
        }

        return static_cast<uint32>(fdamage);
    }
};

/*
Best place to lookup map ids:
https://wago.tools/db2/AreaTable?build=3.4.0.43659&page=1
*/

std::unordered_map<uint32_t, TownData> NerfHerderHelper::townDataMap =
{
    // These are the towns wherein all NPCs will be flagged as PVP,
    // yield honor, and nerfed to zone appropriate levels.

    // This data may be imperfect and I will fix as I discover mistakes.

    // =================================================
    // Kalimdor
    // =================================================
    // Ashenvale
    {2897, {2, 0}}, // Zoram'gar Outpost
    {415, {1, 0}}, // Astranaar
    {431, {2, 0}}, // Splintertree Post
    {2358, {1, 0}}, // Forest Song
    // Azshara
    // Azuremist Isle
    {3576, {1, 0}}, // Azure Watch
    {3573, {1, 0}}, // Odesyus' Landing
    {3572, {1, 0}}, // Stillpine Hold
    {3557, {1, 1}}, // The Exodar
    // Bloodmist Isle
    {3584, {1, 0}}, // Blood Watch
    {3608, {1, 0}}, // Vindicator's Rest
    // Darkshore
    {442, {1, 0}}, // Auberdine
    // Desolace
    {2408, {2, 0}}, // Shadowprey Village
    {608, {1, 0}}, // Nijel's Point
    // Durotar
    {1637, {2, 1}}, // Orgrimmar
    {362, {2, 0}}, // Razor Hill
    {367, {2, 0}}, // Sen'jin Village
    // Dustwallow Marsh
    {496, {2, 0}}, // Brackenwall Village
    {513, {1, 0}}, // Theramore Isle
    // Felwood
    {1997, {2, 0}}, // Bloodvenom Post
    {2479, {1, 0}}, // Emerald Sanctuary
    {1998, {1, 0}}, // Talonbranch Glade
    // Feralas
    {1116, {1, 0}}, // Feathermoon Stronghold
    {1099, {2, 0}}, // Camp Mojache
    // Moonglade
    // Nighthaven -- considered a neutral town
    // Mulgore
    {1638, {2, 1}}, // Thunder Bluff
    {1639, {2, 1}}, // Thunder Bluff
    {1640, {2, 1}}, // Thunder Bluff
    {1641, {2, 1}}, // Thunder Bluff
    {222, {2, 0}}, // Bloodhoof Village
    // Silithus
    // Cenarion Hold -- considered a neutral town
    // Stonetalon Mountains
    {2539, {2, 0}}, // Malaka'jin
    {460, {2, 0}}, // Sun Rock Retreat
    {467, {1, 0}}, // Stonetalon Peak
    // Tanaris
    // Teldrassil
    {1657, {1, 1}}, // Darnassus
    {186, {1, 0}}, // Dolanaar
    {256, {1, 0}}, // Aldrassil
    // The Barrens
    {380, {2, 0}}, // Crossroads
    {378, {2, 0}}, // Camp Taurajo
    // Thousand Needles
    {484, {2, 0}}, // Freewind Post
    {489, {1, 0}}, // Thalanaar
    // Un'goro Crater
    //{541, {0, 0}}, // Marshal's Refuge - considered a neutral town
    // Winterspring
    // =================================================
    // Eastern Kingdoms
    // =================================================
    // Alterac Mountains
    // Arathi Highlands
    {320, {1, 0}}, // Refuge Pointe
    {321, {2, 0}}, // Hammerfall
    // Badlands
    {340, {2, 0}}, // Kargath
    // Blasted Lands
    {1438, {1, 0}}, // Nethergarde Keep
    // Burning Steppes
    {2418, {1, 0}}, // Morgan's Vigil
    // Deadwind Pass
    // Dun Morogh
    {77, {1, 0}}, // Anvilmar
    {131, {1, 0}}, // Kharanos
    {1537, {1, 1}}, // Ironforge
    {189, {1, 0}}, // Steelgrill's Depot
    // Duskwood
    {42, {1, 0}}, // Darkshire
    // Eastern Plaguelands
    // Light's Hope Chapel - considered a neutral town
    // Elwynn Forest
    {87, {1, 0}}, // Goldshire
    {1519, {1, 1}}, // Stormwind
    {4411, {1, 1}}, // Stormwind Harbor
    {24, {1, 0}}, // Northshire Abbey
    // Eversong Woods
    {3487, {1, 1}}, // Silvermoon City
    {3665, {1, 0}}, // Falconwing Square
    // Ghostlands
    {3488, {2, 0}}, // Tranquillien
    // Hillsbrad Foothills
    {271, {1, 0}}, // Southshore
    {2369, {1, 0}}, // Southshore
    {272, {2, 0}}, // Tauren Mill
    {2368, {2, 0}}, // Tauren Mill
    // Isle of Quel'Danas
    // Loch Modan
    {144, {1, 0}}, // Thelsamar
    // Redridge Mountains
    {69, {1, 0}}, // Lakeshire
    // Searing Gorge
    // Thorium Point - considered a neutral town
    // Silverpine Forest
    {228, {2, 0}}, // The Sepulcher
    // Stranglethorn Vale
    {117, {2, 0}}, // Grom'gol Base Camp
    {99, {1, 0}}, // Rebel Camp
    // Swamp of Sorrows
    {75, {2, 0}}, // Stonard
    // Hinterlands
    {348, {1, 0}}, // Aerie Peak
    {3317, {2, 0}}, // Revantusk Village
    // Tirisfal Glades
    {1497, {2, 1}}, // Undercity
    {159, {2, 0}}, // Brill
    {2118, {2, 0}}, // Brill
    {152, {2, 0}}, // The Bulwark
    {813, {2, 0}}, // The Bulwark
    // Western Plaguelands
    {3197, {1, 0}}, // Chillwind Camp
    // Westfall
    {108, {1, 0}}, // Sentinel Hill
    // Wetlands
    {150, {1, 0}}, // Menethil Harbor
    {269, {1, 0}}, // Dun Algaz (tunnels?)
    // =================================================
    // Outland
    // =================================================
    // Blades Edge Mountains
    {3772, {1, 0}}, // Sylvanaar
    {3769, {2, 0}}, // Thunderlord Stronghold
    // Hellfire Peninsula
    {3536, {2, 0}}, // Thrallmar
    {3538, {1, 0}}, // Honor Hold
    // Nagrand
    {3626, {1, 0}}, // Telaar
    {3613, {2, 0}}, // Garadar
    // Netherstorm
    // Shadowmoon Valley
    {3745, {2, 0}}, // Wildhammer Stronghold
    {3744, {1, 0}}, // Shadowmoon Village
    // Terokkar Forest
    {3684, {1, 0}}, // Allerian Stronghold
    {3683, {2, 0}}, // Stonebreaker Hold
    // Zangamarsh
    {3644, {1, 0}}, // Telredor
    {3645, {2, 0}}, // Zabra'jin
    {3718, {2, 0}}, // Swamprat Post
    // =================================================
    // Northrend
    // =================================================
    // Borean Tundra
    {4122, {2, 0}}, // Bor'gorok Outpost
    {4129, {2, 0}}, // Warsong Hold
    {4037, {2, 0}}, // Taunka'le Village
    {4032, {1, 0}}, // Valiance Keep
    {4108, {1, 0}}, // Fizzcrank Airstrip
    // ?? // Valiance Landing Camp
    // Howling Fjord
    {3981, {1, 0}}, // Valgarde
    {4379, {1, 0}}, // Valgarde
    {3998, {1, 0}}, // Westguard Keep
    {4000, {2, 0}}, // Vengeance Landing
    {4532, {2, 0}}, // Vengeance Landing Inn
    {3991, {2, 0}}, // New Agamand
    {4404, {2, 0}}, // New Agamand Inn
    // Dragonblight
    {4177, {1, 0}}, // Wintergarde Keep
    {4165, {2, 0}}, // Agmar's Hammer
    {14339, {2, 0}}, // Agmar's Hammer (Wintergrasp)
    // Grizzly Hills
    {4204, {1, 0}}, // Amberpine Lodge
    {4159, {1, 0}}, // Westfall Brigade Encampment
    {4206, {2, 0}}, // Conquest Hold
    {4211, {2, 0}}, // Camp Oneqwah
    // Zul'Drak
    // The Argent Stand - considered a neutral town
    // Sholazar Basin
    // Crystalsong Forest
    // Hrothgar's Landing
    // Icecrown
    {4427, {1, 0}}, // Argent Vanguard
    {4501, {1, 0}}, // The Argent Vanguard ??
    {4580, {1, 0}}, // Crusaders' Pinnacle
    {4512, {2, 0}}, // Orgrim's Hammer
    {4477, {2, 0}}, // The Shadow Vault
    // Storm Peaks
    // Wintergrasp
};

// list of zones and the ideal level range
std::unordered_map<uint32_t, ZoneData> NerfHerderHelper::zoneDataMap =
{
    // 1-10
    {3524, {1, 10}}, // Azuremyst Isle
    {1, {1, 10}}, // Dun Morogh
    {14, {1, 10}}, // Durotar
    {12, {1, 10}}, // Elwynn Forest
    {3430, {1, 10}}, // Eversong Woods
    {215, {1, 10}}, // Mulgore
    {141, {1, 10}}, // Teldrassil
    {85, {1, 10}}, // Tirisfal Glades

    // 10-20
    {3525, {10, 20}}, // Bloodmyst Isle
    {148, {10, 20}}, // Darkshore
    {3433, {10, 20}}, // Ghostlands
    {38, {10, 20}}, // Loch Modan
    {130, {10, 20}}, // Silverpine Forest
    {40, {10, 20}}, // Westfall
    {17, {10, 25}}, // Barrens

    // 15-25
    {44, {15, 25}}, // Redridge Mountains
    {406, {15, 27}}, // Stonetalon Mountains

    // 20-30
    {331, {18, 30}}, // Ashenvale
    {10, {18, 30}}, // Duskwood
    {267, {20, 30}}, // Hillsbrad Foothills
    {11, {20, 30}}, // Wetlands

    // 25-35
    {400, {25, 35}}, // Thousand Needles

    // 30-40
    {36, {30, 40}}, // Alterac Mountains
    {45, {30, 40}}, // Arathi Highlands
    {405, {30, 40}}, // Desolace
    {33, {30, 45}}, // Stranglethorn Vale

    // 35-45
    {15, {35, 45}}, // Dustwallow Marsh
    {3, {35, 45}}, // Badlands
    {51, {35, 45}}, // Swamp of Sorrows

    // 40-50
    {357, {40, 50}}, // Feralas
    {47, {40, 50}}, // Hinterlands
    {440, {40, 50}}, // Tanaris

    // 45-55
    {51, {45, 50}}, // Searing Gorge
    {16, {45, 55}}, // Azshara
    {4, {45, 55}}, // Blasted Lands
    {490, {48, 55}}, // Un'goro Crater
    {361, {48, 55}}, // Felwood

    // 50-60
    {38, {50, 58}}, // Burning Steppes
    {28, {51, 58}}, // Western Plaguelands
    {139, {53, 60}}, // Eastern Plaguelands
    {618, {53, 60}}, // Winterspring

    // 55-60
    {609, {55, 58}}, // Plaguelands: The Scarlet Enclave
    {41, {55, 60}}, // Deadwind Pass
    {80, {55, 60}}, // Moonglade
    {261, {55, 60}}, // Silithus
    {36, {55, 60}}, // Blackrock Mountain
    {772, {55, 60}}, // Ahn'Qiraj

    // 60-70
    {3483, {58, 63}}, // Hellfire Peninsula
    {3521, {60, 64}}, // Zangarmarsh
    {3519, {62, 65}}, // Terokkar Forest
    {3518, {64, 67}}, // Nagrand
    {3522, {65, 68}}, // Blade's Edge Mountains
    {3523, {67, 70}}, // Netherstorm
    {3520, {67, 70}}, // Shadowmoon Valley
    {4080, {70, 73}}, // Isle of Quel'Danas

    // 70-80
    {3537, {68, 72}}, // Borean Tundra
    {495, {68, 72}}, // Howling Fjord
    {65, {71, 75}}, // Dragonblight
    {394, {73, 75}}, // Grizzly Hills
    {66, {74, 76}}, // Zul'Drak
    {67, {76, 78}}, // Sholazar Basin
    {2817, {77, 80}}, // Crystalsong Forest
    {4742, {77, 80}}, // Hrothgar's Landing
    {210, {77, 80}}, // Icecrown
    {67, {77, 80}}, // Storm Peaks
    {4197, {77, 80}} // Wintergrasp
};

// auras that will be cancelled in dungeon or raid
std::vector<raidDisallowedAurasData> NerfHerderHelper::raidDisallowedAurasMap =
{
    // zone_id, aura_id
    {532, 30468}, // kara, netherspite domination
    {532, 30466}, // kara, netherspite perserverance
    {532, 30467}, // kara, netherspite serenity
    {532, 30423}, // kara, netherspite domination
    {532, 30421}, // kara, netherspite perserverance
    {532, 30422}, // kara, netherspite serenity
    {550, 34190}, // tempest keep, void reaver arcane orb silence
    {550, 37135}, // tempest keep, domination charm effect
    {550, 36797}, // tempest keep, mind control charm effect
    {548, 38112}, // serpenshrine carverns, magic barrier (lady vashj phase 2)
    {534, 31306}, // hyjal summit, anetheron carrion swarm

    // note that outland (all zones) have map_id 530
    {530, 32960}, // hellfire peninsula, doom lord kazzak mark of kazzak
    {530, 37128}, // shadowmoon valley, doomwalker mark of death
};

// mobs that will be killed in dungeon or raid
std::vector<raidDisallowedMobsData> NerfHerderHelper::raidDisallowedMobsMap =
{
    // zone_id, unit_id
    {289, 11284}, // scholo, malicia dark shade
    {531, 15728}, // aq40, cthun tentacles (phase 2)
    {531, 15334}, // aq40, cthun tentacles (phase 2)
    {571, 351069}, // naxx60, zombie chow
    {556, 20343}, // sethek halls, charming totem
    {554, 20481}, // mechanar, nethermancer sepethrea raging flames
    {550, 19551}, // tempest keep, ember of alar
    {550, 21364}, // tempest keep, phoenix egg
    //{550, 21362}, // tempest keep, phoenix
    {534, 17906}, // hyjal summit, gargoyles (horde camp)
    {534, 17907}, // hyjal summit, frost wyrms (horde camp)
};

// mobs that will be bled in dungeon or raid
std::vector<raidBleedingMobsData> NerfHerderHelper::raidBleedingMobsMap =
{
    // zone_id, unit_id, health % to start bleeding, heath % stop bleeding, amount to bleed per second
    {531, 15299, 10, 0, 20000}, // aq40, viscidus <-- to skip freeze phase
    //{531, 15727, 100, 0, 20000}, // aq40, cthun (phase 2) <-- this doesn't work, have to do manual
    {571, 351000, 100, 50, 20000}, // naxx60, thaddius
    {550, 19514, 100, 50, 20000}, // tempest keep, alar
    // faction leaders are bled custom based on server max lvl
};

// mobs that cannot heal at all
std::vector<raidHealMultiMobsData> NerfHerderHelper::raidHealMultiMobsMap =
{
    // map_id, entry_id, multiplier (.1 = 90% reduction)
    {531, 15276, 0}, // aq40, twin emperors
    {531, 15275, 0}, // aq40, twin emperors
    {532, 15689, 0}, // kara, netherspite
    {544, 17256, 0}, // magetheridons lair, hellfire channeler
    {544, 18829, 0}, // magetheridons lair, hellfire warder
    {534, 17808, 0}, // hyjal summit, anetheron
    {530, 18728, 0}, // hellfire peninsula, doom lord kazzak

    // this is essential bc they can evade and reset easily
    {0, 4949, 0}, // horde leader, thrall (4949),
    {0, 10181, 0}, // horde leader, lady sylvanis (10181),
    {0, 3057, 0}, // horde leader, cairne bloodhoof (3057),
    {0, 16802, 0}, // horde leader, lor'themar theron (16802)
    {0, 29611, 0}, // alliance leader, king varian (29611),
    {0, 2784, 0}, // alliance leader, king mangi (2784),
    {0, 7999, 0}, // alliance leader, tyrande whisperwind (7999),
    {0, 17468, 0}, // alliance leader, prophet velen (17468)
};

// damage multiplier for specific mobs
std::vector<raidDmgMultiMobsData> NerfHerderHelper::raidDmgMultiMobsMap =
{
    // zone_id, unit_id, dmg multi (.1 = 90% reduction)
    {571, 351037, 0.5}, // naxx60, four horsemen
    {571, 351038, 0.5}, // naxx60, four horsemen
    {571, 351039, 0.5}, // naxx60, four horsemen
    {571, 351040, 0.5}, // naxx60, four horsemen
    {556, 18473, 0.5}, // sethek halls, talon king ikiss
    {532, 15689, 0.7}, // kara, netherspite
    {532, 17225, 0.7}, // kara, nightbane
    {544, 17256, 0.5}, // magtheridons lair, hellfire channeler
    {544, 18829, 0.5}, // magtheridons lair, hellfire warderer
    {530, 18728, 0.3}, // hellfire peninsula, doom lord kazzak
    {530, 17711, 0.7}, // shadowmoon valley, doomwalker
    {548, 21216, 0.7}, // serpentshrine caverns, hydross the unstable
    //{550, 19622, 0.5}, // tempest keep, kaelthas sunstrider
};

// damage multiplier for specific spells
std::vector<raidDmgMultiSpellsData> NerfHerderHelper::raidDmgMultiSpellsMap =
{
    // zone_id, unit_id, spell_id, dmg multi (.1 = 90% reduction)
    // {556, 18473, 38197, 0.5}, // sethek halls, talon king ikiss, arcane explosion (doesn't work, so nerfing all his dmg?)
    {540, 16808, 30739, 0.5}, // shattered halls, warchief kargath bladefist, blade dance
    {469, 0, 23478, 0.3}, // bwl, vaelastrasz (but comes from allies?), burning adrenaline -- hurt your neighbor version (23478), instakill version (23644)
    {531, 15509, 26053, 0.7}, // aq40, princess huhuran, noxious poison
    {531, 15509, 26050, 0.7}, // aq40, princess huhuran, acid spit
    {531, 15509, 26052, 0.3}, // aq40, princess huhuran, poison bolt
    {531, 0, 26134, 0.1}, // aq40, cthun (but doesn't originate from him?), eye beam
    {531, 0, 26029, 0.1}, // aq40, cthun (but doesn't originate from him?), dark glare
    {571, 351036, 26613, 0.5}, // naxx60, razuvious, unbalancing strike
    {571, 16024, 28322, 0.5}, // naxx60, embalming slime, embalming cloud
    {571, 16027, 28433, 0.1}, // naxx60, living poison, explode
    {571, 351028, 28308, 0.5}, // naxx60, patchwerk, hateful strike
    {571, 0, 28084, 0.5}, // naxx60, thaddius (but not from him), charges
    {571, 0, 28085, 0.5}, // naxx60, thaddius (but not from him), charges
    {571, 0, 29660, 0.5}, // naxx60, thaddius (but not from him), charges
    {571, 0, 28062, 0.5}, // naxx60, thaddius (but not from him), charges
    {571, 0, 28059, 0.5}, // naxx60, thaddius (but not from him), charges
    {571, 0, 29659, 0.5}, // naxx60, thaddius (but not from him), charges
    {571, 16218, 28099, 0.5}, // naxx60, thaddius (this is tesla coil unit), tesla coil shock
    {571, 351000, 28167, 0.5}, // naxx60, thaddius, chain lightning
    {571, 351018, 28524, 0.1}, // naxx60, saphiron, frost breath
    {544, 17256, 30616, 0.5}, // magtheridons lair, magtheridon, blast nova
    {544, 17256, 30613, 0.5}, // magtheridons lair, magtheridon, blast nova
    {550, 19514, 34229, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34269, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34270, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34271, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34272, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34273, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34274, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34275, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34276, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34277, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34278, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34279, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34280, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34281, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34282, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34283, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34284, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34285, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34286, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34287, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34288, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34289, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34314, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34315, 0}, // tempest keep, alar, flame quills
    {550, 19514, 34316, 0}, // tempest keep, alar, flame quills
    {550, 20052, 37123, 0.5}, // tempest keep, crystalcore mechanic, saw blades
    //{550, 19622, 36805, 0.5}, // tempest keep, kaelthas, fireball
    {550, 19622, 36819, 0.5}, // tempest keep, kaelthas, pyroblast
    {550, 0, 36731, 0}, // tempest keep, kaelthas, flamestrike (doesn't originate from boss)
    {534, 17968, 31984, 0}, // hyjal summit, archimonde, finger of death
    {534, 17968, 32111, 0}, // hyjal summit, archimonde, finger of death
    {534, 17968, 39369, 0}, // hyjal summit, archimonde, finger of death
    {530, 17711, 28167, 0.1}, // shadowmoon valley, doomwalker, chain lightning
};

class NerfHerderUnit : public UnitScript
{
public:
    NerfHerderUnit() : UnitScript("NerfHerderUnit") {}

    void ModifyPeriodicDamageAurasTick(Unit* victim, Unit* attacker, uint32& damage, SpellInfo const* spellInfo)
    {
        // catch errors
        if (!NerfHerder_Enabled) return;

        // nerf world bosses & raids
        uint32 udamage = NerfHerderHelper::NerfIncomingDamage(victim, attacker, static_cast<uint32>(damage), spellInfo->Id);

        // modify damage
        damage = static_cast<uint32>(udamage);
    }

    void ModifyMeleeDamage(Unit* victim, Unit* attacker, uint32& damage)
    {
        // catch errors
        if (!NerfHerder_Enabled) return;

        // nerf world bosses & raids
        uint32 udamage = NerfHerderHelper::NerfIncomingDamage(victim, attacker, static_cast<uint32>(damage));

        // modify damage
        damage = static_cast<uint32>(udamage);
    }

    void ModifyHealReceived(Unit* victim, Unit* healer, uint32& addHealth, SpellInfo const* spellInfo)
    {
        // catch errors
        if (!NerfHerder_Enabled) return;

        // nerf world bosses & raids
        uint32 udamage = NerfHerderHelper::NerfIncomingHeals(victim, healer, static_cast<uint32>(addHealth), spellInfo->Id);

        // modify damage
        addHealth = static_cast<int32>(udamage);
    }

    void ModifySpellDamageTaken(Unit* victim, Unit* attacker, int32& damage, SpellInfo const* spellInfo)
    {
        // catch errors
        if (!NerfHerder_Enabled) return;

        // spellInfo->IsPositive() if is healing, even tho amount is positive number whether heal or dmg

        // nerf world bosses & raids
        uint32 udamage = NerfHerderHelper::NerfIncomingDamage(victim, attacker, static_cast<uint32>(damage), spellInfo->Id);

        // modify damage
        damage = static_cast<int32>(udamage);
    }

    void OnUnitDeath(Unit* unit, Unit* /*killer*/)
    {
        // catch errors
        if (!NerfHerder_Enabled) return;

        // save some overhead by not running this in dungeons or raids
        if (unit->GetMap()->IsDungeon() || unit->GetMap()->IsRaid()) return;

        // Get the unit's team/faction when it was alive
        TeamId killedTeam = TEAM_NEUTRAL;
        if (Player* killedPlayer = unit->ToPlayer())
        {
            killedTeam = killedPlayer->GetTeamId();
        }

        // get all players in map...
        Map::PlayerList const& players = unit->GetMap()->GetPlayers();
        for (auto const& i : players)
        {
            if (Player* player = i.GetSource())
            {
                // if player is alive...
                if (player->IsAlive())
                {
                    // check if player is within reward distance
                    if (player->IsAtGroupRewardDistance(unit) && player->GetTeamId() != killedTeam)
                    {
                        // battleground logic
                        NerfHerderHelper::PvPBattlegroundOnKill(player);

                        // world logic
                        NerfHerderHelper::PvPWorldOnKill(player, unit->ToCreature());
                    }
                }
            }
        }
    }
};

class NerfHerderCreature : public AllCreatureScript
{
public:
    NerfHerderCreature() : AllCreatureScript("NerfHerderCreature") {}

    void OnAllCreatureUpdate(Creature* creature, uint32 /*diff*/)
    {
        // catch errors...
        if (!creature) return;
        if (!creature->IsAlive()) return;
        if (!NerfHerder_Enabled) return;

        // only run once a second
        if (!NerfHerderHelper::HasTimeElapsed(creature, 1)) return;

        // add any blanket debuffs (only effects world)
        NerfHerderHelper::ProcessCreatureWorld(creature);

        // fix weird dead but not dead creatures (bug fix)
        if (creature->GetMap()->IsDungeon() || creature->GetMap()->IsRaid())
        {
            if (creature->GetHealth() < 1 && creature->IsAlive())
            {
                creature->setDeathState(DeathState::JustDied);
            }
        }

        // end game fixes
        if (NerfHerder_RaidFixes_Enabled)
        {
            // find and remove disallowed auras
            for (const auto& entry : NerfHerderHelper::raidDisallowedAurasMap)
            {
                if (creature->GetMapId() == entry.map_id && creature->HasAura(entry.aura_id))
                {
                    creature->RemoveAura(entry.aura_id);
                }
            }

            // find and remove disallowed mobs
            for (const auto& entry : NerfHerderHelper::NerfHerderHelper::raidDisallowedMobsMap)
            {
                if (creature->GetMapId() == entry.map_id && creature->GetEntry() == entry.creature_id)
                {
                    creature->setDeathState(DeathState::JustDied);
                }
            }

            // find and bleed bleedable mobs
            uint32 current_health_percentage = 100;
            for (const auto& entry : NerfHerderHelper::raidBleedingMobsMap)
            {
                if (creature->GetMapId() == entry.map_id && creature->GetEntry() == entry.creature_id)
                {
                    if (creature->IsInCombat())
                    {
                        //current_health_percentage = 100 * (creature->GetHealth() / creature->GetMaxHealth());
                        current_health_percentage = static_cast<uint32>(100.0f * (
                            static_cast<float>(creature->GetHealth()) / static_cast<float>(creature->GetMaxHealth())
                        ));
                        if (current_health_percentage <= entry.health_percentage_start && current_health_percentage > entry.health_percentage_stop)
                        {
                            if (Player* nearest_player = creature->SelectNearestPlayer(100.0f))
                            {
                                nearest_player->DealDamage(nearest_player, creature, entry.damage);
                            }
                        }
                    }
                }
            }

            // if dungeon or raid...
            if (creature->GetMap()->IsDungeon() || creature->GetMap()->IsRaid())
            {
                // if in AQ40...
                if (creature->GetMapId() == 531)
                {
                    // ouro (AQ40)
                    if (creature->GetEntry() == 15712)
                    {
                        // if this mound is NOT the respawn for boss
                        if (!creature->HasAura(26642))
                        {
                            // die
                            creature->setDeathState(DeathState::JustDied);
                        }
                    }

                    // special fix for cthun phase 2
                    if (creature->GetEntry() == 15727) // 15589 is the eye? 15727 is c'thun?
                    {
                        // if he is attackable (meaning the eye is dead)...
                        if (!(creature->GetUnitFlags() & UNIT_FLAG_NON_ATTACKABLE))
                        {
                            if (Player* nearest_player = creature->SelectNearestPlayer(100.0f))
                            {
                                // bleed him till he dies (a few seconds)
                                nearest_player->DealDamage(nearest_player, creature, 50000);
                            }
                        }
                    }
                }

                // if in kara...
                if (creature->GetMapId() == 532)
                {
                    // kara chess event (all alliance non-king peices)
                    if (creature->GetEntry() == 17211)
                    {
                        // has Heroism buff (meaning game has started)
                        if (creature->HasAura(37471))
                        {
                            // find horde king
                            if (Creature* king = creature->FindNearestCreature(21752, 100.0f))
                            {
                                // bleed him
                                creature->DealDamage(king, creature, 50000);
                            }
                        }
                    }

                    // kara chess event (horde pawns)
                    if (creature->GetEntry() == 17469)
                    {
                        // has Bloodlust buff (meaning game has started)
                        if (creature->HasAura(37472))
                        {
                            // find alliance king
                            if (Creature* king = creature->FindNearestCreature(21684, 100.0f))
                            {
                                // bleed him
                                king->DealDamage(king, creature, 50000);
                            }
                        }
                    }

                    // Prince Malchezaar Enfeebled (this is here for bots)
                    if (creature->HasAura(30843) || creature->HasAura(41624))
                    {
                        Unit::DealHeal(creature, creature, 500); // help them recover
                    }
                }
            }

            // custom fixes world...
            if (!creature->GetMap()->IsDungeon() && !creature->GetMap()->IsRaid() && !creature->GetMap()->IsBattlegroundOrArena())
            {
                // World Faction Leaders
                // Horde: Thrall (4949), Lady Sylvanis (10181), Cairne Bloodhoof (3057), Lor'themar Theron (16802)
                // Alliance: King Varian (29611), King Mangi (2784), Tyrande Whisperwind (7999), Prophet Velen (17468)
                if (creature->GetEntry() == 4949 || creature->GetEntry() == 10181 || creature->GetEntry() == 3057 || creature->GetEntry() == 16802 || creature->GetEntry() == 29611 || creature->GetEntry() == 2784 || creature->GetEntry() == 7999 || creature->GetEntry() == 17468)
                {
                    if (creature->IsInCombat())
                    {
                        if (NerfHerder_MaxPlayerLevel <= 60)
                        {
                            if (creature->GetHealth() > (creature->GetMaxHealth() / 8))
                            {
                                //creature->SetHealth(creature->GetMaxHealth() / 8);
                                if (Player* nearest_player = creature->SelectNearestPlayer(100.0f))
                                {
                                    // bleed him
                                    nearest_player->DealDamage(nearest_player, creature, 10000);
                                }
                            }
                        }

                        else if (NerfHerder_MaxPlayerLevel > 60 && NerfHerder_MaxPlayerLevel <= 70)
                        {
                            if (creature->GetHealth() > (creature->GetMaxHealth() / 4))
                            {
                                //creature->SetHealth(creature->GetMaxHealth() / 4);
                                if (Player* nearest_player = creature->SelectNearestPlayer(100.0f))
                                {
                                    // bleed him
                                    nearest_player->DealDamage(nearest_player, creature, 10000);
                                }
                            }
                        }
                    }
                }
            }
        } // end raid fixes
    }
};

class NerfHerderPlayer : public PlayerScript
{
public:
    NerfHerderPlayer() : PlayerScript("NerfHerderPlayer") {}

    void OnPlayerUpdate(Player* player, uint32 /*p_time*/)
    {
        // catch errors
        if (!player) return;
        if (!player->IsAlive()) return;
        if (!NerfHerder_Enabled) return;

        // only run once a second
        if (!NerfHerderHelper::HasTimeElapsed(player, 1)) return;

        // add any debuffs for when playing w/ lower level teammates
        NerfHerderHelper::ProcessPlayerParty(player);

        // custom end game fixes
        if (NerfHerder_RaidFixes_Enabled)
        {
            // find and remove disallowed auras
            for (const auto& entry : NerfHerderHelper::raidDisallowedAurasMap)
            {
                if (player->GetMapId() == entry.map_id && player->HasAura(entry.aura_id))
                {
                    player->RemoveAura(entry.aura_id);
                }
            }

            // if in dungeon or raid...
            if (player->GetMap()->IsDungeon() || player->GetMap()->IsRaid())
            {
                // if in kara...
                if (player->GetMapId() == 532)
                {
                    // Prince Malchezaar Enfeebled
                    if (player->HasAura(30843) || player->HasAura(41624))
                    {
                        Unit::DealHeal(player, player, 500); // help them recover
                    }
                }
            }
        } // end raid fixes
    }
};

class NerfHerderBattleground: public AllBattlegroundScript
{
public:
    NerfHerderBattleground() : AllBattlegroundScript("NerfHerderBattleground") {}

    void OnBattlegroundEndReward(Battleground* bg, Player* player, TeamId winnerTeamId)
    {
        if (!NerfHerder_Enabled) return;
        if (!NerfHerder_Battleground_Enabled) return;

        // if not battleground, bail
        if (!player->GetMap()->IsBattlegroundOrArena()) return;

        // get reputation faction id, if available
        uint32 faction_id = NerfHerderHelper::GetBattlegroundFactionId(player);

        // if winner...
        if (player->GetTeamId() == winnerTeamId)
        {
            if (NerfHerder_Battleground_Win_GoldReward)
            {
                NerfHerderHelper::GrantRewardGold(player, NerfHerder_Battleground_Win_GoldReward, 1);
            }

            if (NerfHerder_Battleground_Win_HKReward)
            {
                NerfHerderHelper::GrantRewardHK(player, NerfHerder_Battleground_Win_HKReward, 1);
            }

            if (NerfHerder_Battleground_Win_HonorReward)
            {
                NerfHerderHelper::GrantRewardHonor(player, NerfHerder_Battleground_Win_HonorReward, 1);
            }

            if (NerfHerder_Battleground_Win_APReward)
            {
                NerfHerderHelper::GrantRewardArena(player, NerfHerder_Battleground_Win_APReward, 1);
            }

            if (NerfHerder_Battleground_Win_RepReward)
            {
                if (faction_id)
                {
                    NerfHerderHelper::GrantRewardRep(player, faction_id, NerfHerder_Battleground_Win_RepReward, 1);
                }
            }

            if (NerfHerder_Battleground_Win_XPReward)
            {
                NerfHerderHelper::GrantRewardXP(player, NerfHerder_Battleground_Win_XPReward, 1);
            }
        }

        // if loser
        else
        {
            if (NerfHerder_Battleground_Lose_GoldReward)
            {
                NerfHerderHelper::GrantRewardGold(player, NerfHerder_Battleground_Lose_GoldReward, 1);
            }

            if (NerfHerder_Battleground_Lose_HKReward)
            {
                NerfHerderHelper::GrantRewardHK(player, NerfHerder_Battleground_Lose_HKReward, 1);
            }

            if (NerfHerder_Battleground_Lose_HonorReward)
            {
                NerfHerderHelper::GrantRewardHonor(player, NerfHerder_Battleground_Lose_HonorReward, 1);
            }

            if (NerfHerder_Battleground_Lose_APReward)
            {
                NerfHerderHelper::GrantRewardArena(player, NerfHerder_Battleground_Lose_APReward, 1);
            }

            if (NerfHerder_Battleground_Lose_RepReward)
            {
                if (faction_id)
                {
                    NerfHerderHelper::GrantRewardRep(player, faction_id, NerfHerder_Battleground_Lose_RepReward, 1);
                }
            }

            if (NerfHerder_Battleground_Lose_XPReward)
            {
                NerfHerderHelper::GrantRewardXP(player, NerfHerder_Battleground_Lose_XPReward, 1);
            }
        }
    }
};

using namespace Acore::ChatCommands;

class NerfHerderCommand : public CommandScript
{
public:
    NerfHerderCommand() : CommandScript("NerfHerderCommand") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable commandNerfHerderTable =
        {
            //{ "test", HandleTest, SEC_PLAYER, Console::No },
        };

        static ChatCommandTable commandTable =
        {
            //{ "nerf", commandNerfHerderTable },
        };

        return commandTable;
    }
};

void AddNerfHerderScripts()
{
    new NerfHerderConfig();
    new NerfHerderHelper();
    new NerfHerderCreature();
    new NerfHerderPlayer();
    new NerfHerderUnit();
    new NerfHerderBattleground();
    new NerfHerderCommand();
}