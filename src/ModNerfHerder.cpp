#include "ModNerfHerder.h"
#include "ScriptMgr.h"
#include "Formulas.h"
#include "Config.h"
#include "World.h"
#include "WorldPacket.h"
#include "Unit.h"
#include "Creature.h"
#include "Player.h"
#include "Pet.h"
#include "Chat.h"
#include <unordered_map>
#include <ctime>
#include <random>
#include <chrono>

uint32_t NerfHerder_Enabled = 0;
float NerfHerder_NerfRate = 0;
float NerfHerder_EliteNerfRate = 0;
uint32_t NerfHerder_PlayerLevelEnabled = 0;
uint32_t NerfHerder_ZoneLevelEnabled = 0;
uint32_t NerfHerder_HidePvPVendorsEnabled = 0;
uint32_t NerfHerder_ForcePvPEnabled = 0;
uint32_t NerfHerder_Honor_Enabled = 0;
float NerfHerder_Honor_Rate = 0;
uint32_t NerfHerder_Honor_GreyEnabled = 0;
float NerfHerder_Honor_GreyRate = 0;
uint32_t NerfHerder_Honor_PlunderEnabled = 0;
uint32_t NerfHerder_Honor_PlunderAmountPerLevel = 0;
uint32_t NerfHerder_MaxPlayerLevel = 80;
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
uint32_t NerfHerder_WorldEvent_Enabled = 0;
uint32_t NerfHerder_WorldEvent_HealthThreshold = 0;
float NerfHerder_WorldEvent_NerfRate = 0;
uint32_t NerfHerder_Battleground_Enabled = 0;
uint32_t NerfHerder_Battleground_HKReward = 0;
float NerfHerder_Battleground_DamageRate = 0;
float NerfHerder_Battleground_HealingRate = 0;
uint32_t NerfHerder_NPCBots_XPEnabled = 0;

class NerfHerderConfig : public WorldScript
{
public:
    NerfHerderConfig() : WorldScript("NerfHerderConfig") {}

    void OnBeforeConfigLoad(bool reload) override
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
        NerfHerder_NerfRate = sConfigMgr->GetOption<float>("NerfHerder.NerfRate", 0);
        NerfHerder_EliteNerfRate = sConfigMgr->GetOption<float>("NerfHerder.EliteNerfRate", 0);
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
        NerfHerder_HidePvPVendorsEnabled = sConfigMgr->GetOption<int>("NerfHerder.HidePvPVendorsEnabled", 0);
        NerfHerder_WorldEvent_Enabled = sConfigMgr->GetOption<int>("NerfHerder.WorldEvent.Enabled", 0);
        NerfHerder_WorldEvent_HealthThreshold = sConfigMgr->GetOption<int>("NerfHerder.WorldEvent.HealthThreshold", 100000);
        NerfHerder_WorldEvent_NerfRate = sConfigMgr->GetOption<float>("NerfHerder.WorldEvent.NerfRate", 0);
        NerfHerder_Battleground_Enabled = sConfigMgr->GetOption<int>("NerfHerder.Battleground.Enabled", 0);
        NerfHerder_Battleground_HKReward = sConfigMgr->GetOption<int>("NerfHerder.Battleground.HKReward", 0);
        NerfHerder_Battleground_DamageRate = sConfigMgr->GetOption<float>("NerfHerder.Battleground.DamageRate", 0);
        NerfHerder_Battleground_HealingRate = sConfigMgr->GetOption<float>("NerfHerder.Battleground.HealingRate", 0);
        NerfHerder_NPCBots_XPEnabled = sConfigMgr->GetOption<int>("NerfHerder.NPCBots.XPEnabled", 0);
    }
};

class NerfHerderCreatureInfo : public DataMap::Base
{
public:
    NerfHerderCreatureInfo() {}

    uint32_t is_brand_new = 1;
    uint32_t is_altered = 0;

    uint64_t last_reset = 0;

    uint32_t original_level = 0;
    uint32_t original_health = 0;
    uint32_t original_armor = 0;

    uint32_t new_level = 0;
    uint32_t new_health = 0;
    uint32_t new_armor = 0;

    float nerf_rate = 0;
    float additional_nerf_rate = 0;
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

class NerfHerderHelper
{
public:
    static std::unordered_map<uint32_t, VendorData> vendorDataMap;
    static std::unordered_map<uint32_t, ZoneData> zoneDataMap;
    static std::unordered_map<uint32_t, TownData> townDataMap;

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

        // apply world buff
        player->GetMap()->DoForAllPlayers([&](Player* p)
        {
            uint32_t is_faction_match = 0;
            if (player->GetTeamId() == TEAM_ALLIANCE) is_faction_match = p->GetTeamId() == TEAM_ALLIANCE ? 1 : 0;
            if (player->GetTeamId() == TEAM_HORDE) is_faction_match = p->GetTeamId() == TEAM_HORDE ? 1 : 0;

            // no matter where they are, buff all players
            if (p->IsAlive() && p->IsPlayer() && is_faction_match)
            {
                if (NerfHerder_WorldBuff_SpellId_01)
                    p->CastSpell(p, NerfHerder_WorldBuff_SpellId_01, true);
                if (NerfHerder_WorldBuff_SpellId_02)
                    p->CastSpell(p, NerfHerder_WorldBuff_SpellId_02, true);
                if (NerfHerder_WorldBuff_SpellId_03)
                    p->CastSpell(p, NerfHerder_WorldBuff_SpellId_03, true);
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

        // Below is every other method I tried for flagging horde/alliance
        // and why it didn't work.  I ended up just marking them by the town.

        //if (creature->IsPvP()) return 1; // this messed up capitol guards

        // GetTeamId() only works on players.
        // GetFaction() doesn't seem to work at all, and involves sub-factions.
        // GetMapId() involves too many sub-areas, even inside of towns.
        // GetUnitFlags() is the same thing as the below method.

        // this method only works on players
        //if (creature->GetTeamId() == TEAM_ALLIANCE) return 1;
        //if (creature->GetTeamId() == TEAM_HORDE) return 1;

        /*
        // this only seems to work for players
        if (creature->getRace() == RACE_HUMAN) return 1;
        if (creature->getRace() == RACE_ORC) return 1;
        if (creature->getRace() == RACE_DWARF) return 1;
        if (creature->getRace() == RACE_NIGHTELF) return 1;
        if (creature->getRace() == RACE_UNDEAD_PLAYER) return 1;
        if (creature->getRace() == RACE_TAUREN) return 1;
        if (creature->getRace() == RACE_GNOME) return 1;
        if (creature->getRace() == RACE_TROLL) return 1;
        if (creature->getRace() == RACE_BLOODELF) return 1;
        if (creature->getRace() == RACE_DRAENEI) return 1;
        */

        /*
        // this worked but imperfectly
        if (creature->IsGossip()) return 1;
        if (creature->IsVendor()) return 1;
        if (creature->IsTrainer()) return 1;
        if (creature->IsQuestGiver()) return 1;
        if (creature->IsTaxi()) return 1;
        if (creature->IsGuildMaster()) return 1;
        if (creature->IsBattleMaster()) return 1;
        if (creature->IsBanker()) return 1;
        if (creature->IsInnkeeper()) return 1;
        //if (creature->IsSpiritHealer()) return 1;
        //if (creature->IsSpiritGuide()) return 1;
        if (creature->IsTabardDesigner()) return 1;
        if (creature->IsAuctioner()) return 1;
        if (creature->IsArmorer()) return 1;
        if (creature->IsServiceProvider()) return 1;
        */
    }

    static uint32_t IsPvPVendor(Creature* creature)
    {
        uint32_t entry_id = creature->GetEntry();

        if (NerfHerderHelper::vendorDataMap.find(entry_id) == NerfHerderHelper::vendorDataMap.end())
            return 0;

        uint32_t expansion_id = NerfHerderHelper::vendorDataMap[entry_id].expansionID;

        if (NerfHerder_MaxPlayerLevel <= 60 and expansion_id > 0)
            return 1;
        if (NerfHerder_MaxPlayerLevel <= 70 and expansion_id > 1)
            return 1;

        return 0;
    }

    static uint32_t GetZoneLevel(Creature* creature)
    {
        uint32_t zone_id = creature->GetZoneId();

        if (NerfHerderHelper::zoneDataMap.find(zone_id) == NerfHerderHelper::zoneDataMap.end())
            return 0;

        return NerfHerderHelper::zoneDataMap[zone_id].maxLevel;
    }

    static uint64_t GetTime()
    {
        // number of seconds
        return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    static void ResetCreatureWorld(Creature* creature)
    {
        // if not evading, bail... (we only reset after a fight)
        if (!creature->IsInEvadeMode() && !creature->isDead()) return;

        // load info
        NerfHerderCreatureInfo *creatureInfo = creature->CustomData.GetDefault<NerfHerderCreatureInfo>("NerfHerderCreatureInfo");

        // if not a modified creature, bail...
        if (!creatureInfo->is_altered) return;

        // if it's been less than 10 seconds since last reset, bail...
        if (NerfHerderHelper::GetTime() - creatureInfo->last_reset < 10) return;

        // amend logs
        creatureInfo->is_altered = 0;
        creatureInfo->last_reset = NerfHerderHelper::GetTime();

        // if this is a dead creature situation...
        if (creature->isDead())
        {
            return; // go no further
        }

        // The reason we even check for a necessary reset on the nerf is bc
        // when a guard exits combat and he lived, he resets his own health
        // and he ignores the nerf limitation completely.

        // nerf auras
        uint32_t HpAura = 89501;
        uint32_t DamageDoneTakenAura = 89502;
        uint32_t BaseStatAPAura = 89503;
        uint32_t AbsorbAura = 89505;
        uint32_t HealingDoneAura = 89506;
        uint32_t PhysicalDamageTakenAura = 89507;

        // remove auras
        creature->RemoveAura(HpAura);
        creature->RemoveAura(DamageDoneTakenAura);
        creature->RemoveAura(BaseStatAPAura);
        creature->RemoveAura(AbsorbAura);
        creature->RemoveAura(HealingDoneAura);
        creature->RemoveAura(PhysicalDamageTakenAura);

        // reset level
        creature->SetLevel(creatureInfo->original_level); // happens so fast you never see it

        // force rechange
        NerfHerderHelper::UpdateCreatureWorld(creature, creatureInfo->new_level, creatureInfo->nerf_rate, creatureInfo->additional_nerf_rate);
    }

    static void UpdateCreatureWorld(Creature* creature, uint32_t new_level, float nerf_rate = 0, float additional_nerf_rate = 0)
    {
        // load info
        NerfHerderCreatureInfo *creatureInfo = creature->CustomData.GetDefault<NerfHerderCreatureInfo>("NerfHerderCreatureInfo");

        // if creature is already altered, bail...
        if (creatureInfo->is_altered) return;
        //if (creature->HasAura(89501)) return; // if has HP nerf already

        // if first time...
        if (creatureInfo->is_brand_new)
        {
            // log original numbers
            creatureInfo->original_level = creature->GetLevel();
            creatureInfo->original_health = creature->GetMaxHealth();
            creatureInfo->original_armor = creature->GetArmor();

            // log changes
            creatureInfo->new_level = new_level;
            creatureInfo->nerf_rate = nerf_rate;
            creatureInfo->additional_nerf_rate = additional_nerf_rate;

            // mark as not new anymore
            creatureInfo->is_brand_new = 0;
        }

        // calc proportional level change
        float ratio = static_cast<float>(new_level) / static_cast<float>(creatureInfo->original_level);

        // calc nerf multiplier (negative)
        float multiplier = -100 + (ratio * 100);

        // if elite...
        if (creature->isElite())
        {
            multiplier = multiplier - (NerfHerder_EliteNerfRate * (100 + multiplier)); // elites get nerfed harder
        }

        // some assumptions here:
        // the proportional difference between a lvl 80 and lvl 60 is 25%, but
        // the dmg done by a lvl 80 is not only 25% higher than a lvl 60, it's
        // probably closer to 3x as much!  so our nerf needs to not be a linear
        // or proportional nerf, it needs to curve.
        // AT THE PRESENT TIME I WILL DO NOTHING
        // https://us.forums.blizzard.com/en/wow/t/a-look-back-at-health-values/587645

        // calc custom hp nerf (extra nerfs only apply to health, not damage)
        float hp_multiplier = -100 + (ratio * 100);
        if (creatureInfo->nerf_rate > 0)
        {
            hp_multiplier = hp_multiplier - (creatureInfo->nerf_rate * (100 + hp_multiplier));
        }
        if (creatureInfo->additional_nerf_rate > 0)
        {
            hp_multiplier = hp_multiplier - (creatureInfo->additional_nerf_rate * (100 + hp_multiplier));
        }

        // convert to int
        int32_t negative_multiplier = static_cast<int>(multiplier);
        int32_t negative_hp_multiplier = static_cast<int>(hp_multiplier);

        // just in case something goes wrong
        if (negative_multiplier > 0) negative_multiplier = 0;
        if (negative_hp_multiplier > 0) negative_hp_multiplier = 0;
        if (negative_multiplier < -100) negative_multiplier = -100;
        if (negative_hp_multiplier < -100) negative_hp_multiplier = -100;

        // some armor calculations
        //int32_t armor_reduction = static_cast<int>(negative_multiplier * -1);
        //int32_t armor_reduction = static_cast<int>((creatureInfo->original_armor / ([467.5 * new_level] + creatureInfo->original_armor - 22167.5)) * 100);

        // calc proper health and armor
        //uint32_t new_health = creatureInfo->original_health * (1 - ((-1 * negative_hp_multiplier) / 100));
        //int32_t new_armor = creatureInfo->original_armor * (1 - ((-1 * negative_multiplier) / 100)); // not using negative_hp_multiplier

        // log calculations
        //creatureInfo->new_health = new_health;
        //creatureInfo->new_armor = new_armor;

        /*
        // the following health and armor technique comes from autobalance mod (way more complicated than it should be)

        // health
        creature->SetCreateHealth(new_health);
        creature->SetMaxHealth(new_health);
        creature->SetHealth(new_health);
        creature->ResetPlayerDamageReq();
        creature->SetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE, (float)new_health);

        // armor
        creature->SetArmor(new_armor);
        creature->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, (float)new_armor);
        */

        // nerf auras
        uint32_t HpAura = 89501;
        uint32_t DamageDoneTakenAura = 89502;
        uint32_t BaseStatAPAura = 89503;
        uint32_t AbsorbAura = 89505;
        uint32_t HealingDoneAura = 89506;
        //uint32_t PhysicalDamageTakenAura = 89507;

        // nerf their damage done, base stats, absorbsion, and healing done
        creature->CastCustomSpell(creature, HpAura, &negative_hp_multiplier, NULL, NULL, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, DamageDoneTakenAura, 0, &negative_multiplier, NULL, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, BaseStatAPAura, &negative_multiplier, &negative_multiplier, &negative_multiplier, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, AbsorbAura, &negative_multiplier, NULL, NULL, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, HealingDoneAura, &negative_multiplier, NULL, NULL, true, NULL, NULL, creature->GetGUID());
        //creature->CastCustomSpell(creature, PhysicalDamageTakenAura, &armor_reduction, NULL, NULL, true, NULL, NULL, creature->GetGUID());

        // set new level
        creature->SetLevel(new_level, false); // flag false to bypass any hooray animations

        // amend logs
        creatureInfo->is_altered = 1;
    }

    static void ProcessCreatureWorld(Creature* creature)
    {
        // catch errors
        if (!creature) return;
        if (creature->IsPlayer()) return;
        if (creature->GetMap()->IsDungeon() || creature->GetMap()->IsRaid() || creature->GetMap()->IsBattleground()) return;

        // catch errors
        if (!NerfHerder_Enabled) return;

        // reset
        NerfHerderHelper::ResetCreatureWorld(creature);

        // Notes to self -- this code will only ever nerf an NPC a single time, subsequent attempts will fail.

        // init
        uint32_t max_level;

        // determine alliance / horde npcs in the world
        uint32_t is_field_agent = NerfHerderHelper::IsFieldAgent(creature);

        // if nerfing high health npcs...
        if (NerfHerder_WorldEvent_Enabled)
        {
            // if npc has high health...
            if (creature->GetMaxHealth() > NerfHerder_WorldEvent_HealthThreshold)
            {
                // start off w/ no level change
                max_level = creature->GetLevel();

                // if we are enforcing level limits...
                if (NerfHerder_PlayerLevelEnabled)
                {
                    // set appropriate level
                    max_level = creature->GetLevel() > NerfHerder_MaxPlayerLevel ? NerfHerder_MaxPlayerLevel : creature->GetLevel();
                }

                // nerf them harder than normal
                NerfHerderHelper::UpdateCreatureWorld(creature, max_level, NerfHerder_NerfRate, NerfHerder_WorldEvent_NerfRate);
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
                    NerfHerderHelper::UpdateCreatureWorld(creature, max_level, NerfHerder_NerfRate);
                }
            }
        }

        // if max zone level is enabled...
        if (NerfHerder_ZoneLevelEnabled)
        {
            if (is_field_agent)
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

        // if force pvp is enabled...
        if (NerfHerder_ForcePvPEnabled)
        {
            if (is_field_agent)
            {
                // force them to be pvp
                creature->SetPvP(true);
            }
        }

        // if hiding pvp vendors...
        if (NerfHerder_HidePvPVendorsEnabled)
        {
            if (NerfHerderHelper::IsPvPVendor(creature))
            {
                // This "hiding" method works, but not if you are a GM.
                // That means the characters aren't really removed from the game?
                // Also not sure how this might effect world PvP.

                // hide them
                creature->SetVisible(false);
            }
        }
    }

    static void ResetCreatureBattleground(Creature* creature)
    {
        if (!creature->HasAura(89502)) return;

        // nerf auras
        uint32_t DamageDoneTakenAura = 89502;
        uint32_t AbsorbAura = 89505;
        uint32_t HealingDoneAura = 89506;

        // remove auras
        creature->RemoveAura(DamageDoneTakenAura);
        creature->RemoveAura(AbsorbAura);
        creature->RemoveAura(HealingDoneAura);
    }

    static void UpdateCreatureBattleground(Creature* creature)
    {
        // if already debuffed, bail
        if (creature->HasAura(89502)) return;

        // calc
        int32_t negative_multiplier_damage = static_cast<int>(-100 + (100 * NerfHerder_Battleground_DamageRate));
        int32_t negative_multiplier_healing = static_cast<int>(-100 + (100 * NerfHerder_Battleground_HealingRate));

        // nerf auras
        uint32_t DamageDoneTakenAura = 89502;
        uint32_t AbsorbAura = 89505;
        uint32_t HealingDoneAura = 89506;

        // add debuffs
        creature->CastCustomSpell(creature, DamageDoneTakenAura, 0, &negative_multiplier_damage, NULL, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, AbsorbAura, &negative_multiplier_healing, NULL, NULL, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, HealingDoneAura, &negative_multiplier_healing, NULL, NULL, true, NULL, NULL, creature->GetGUID());
    }

    static void ProcessCreatureBattleground(Creature* creature)
    {
        // catch errors
        if (!creature) return;

        // catch errors
        if (!NerfHerder_Enabled) return;
        if (!NerfHerder_Battleground_Enabled) return;

        // if in battleground...
        if (creature->GetMap()->IsBattleground())
        {
            NerfHerderHelper::UpdateCreatureBattleground(creature);
        }
        else
        {
            NerfHerderHelper::ResetCreatureBattleground(creature);
        }
    }

    static void ResetPlayerBattleground(Player* creature)
    {
        if (!creature->HasAura(89502)) return;

        // nerf auras
        uint32_t DamageDoneTakenAura = 89502;
        uint32_t AbsorbAura = 89505;
        uint32_t HealingDoneAura = 89506;

        // remove auras
        creature->RemoveAura(DamageDoneTakenAura);
        creature->RemoveAura(AbsorbAura);
        creature->RemoveAura(HealingDoneAura);
    }

    static void UpdatePlayerBattleground(Player* creature)
    {
        // if already debuffed, bail
        if (creature->HasAura(89502)) return;

        // calc
        int32_t negative_multiplier_damage = static_cast<int>(-100 + (100 * NerfHerder_Battleground_DamageRate));
        int32_t negative_multiplier_healing = static_cast<int>(-100 + (100 * NerfHerder_Battleground_HealingRate));

        // nerf auras
        uint32_t DamageDoneTakenAura = 89502;
        uint32_t AbsorbAura = 89505;
        uint32_t HealingDoneAura = 89506;

        // add debuffs
        creature->CastCustomSpell(creature, DamageDoneTakenAura, 0, &negative_multiplier_damage, NULL, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, AbsorbAura, &negative_multiplier_healing, NULL, NULL, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, HealingDoneAura, &negative_multiplier_healing, NULL, NULL, true, NULL, NULL, creature->GetGUID());
    }

    static void ProcessPlayerBattleground(Player* creature)
    {
        // catch errors
        if (!creature) return;

        // catch errors
        if (!NerfHerder_Enabled) return;
        if (!NerfHerder_Battleground_Enabled) return;

        // if in battleground...
        if (creature->GetMap()->IsBattleground() || creature->InArena())
        {
            NerfHerderHelper::UpdatePlayerBattleground(creature);
        }
        else
        {
            NerfHerderHelper::ResetPlayerBattleground(creature);
        }
    }

    static void RewardXP(Player* player, Creature* creature)
    {
        if (!NerfHerder_Enabled) return;
        if (!NerfHerder_NPCBots_XPEnabled) return;

        // if not battleground, bail
        if (!player->GetMap()->IsBattleground() && !player->InArena()) return;

        if (player->IsAlive())
        {
            // Calculate and grant XP to the player
            uint32 xpReward = (creature->getLevel() * 200) / (player->getLevel() + 1);
            player->GiveXP(xpReward, creature);
        }
    }

    static void RewardHonor(Player* player, Creature* killed)
    {
        if (!NerfHerder_Enabled) return;
        if (!NerfHerder_Honor_Enabled) return;

        // This was all taken straight from HonorGuard mod, but tweaked to
        // give honor on any pvp flagged creature.
        // https://github.com/azerothcore/mod-gain-honor-guard/blob/master/src/GainHonorGuard.cpp

        // catch errors
        if (player->GetMap()->IsDungeon() || player->GetMap()->IsRaid() || player->GetMap()->IsBattleground() || player->InArena()) return;

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
                    uint8 k_level = player->getLevel();
                    uint8 k_grey = Acore::XP::GetGrayLevel(k_level);
                    uint8 v_level = killed->getLevel();

                    // handle grey override setting
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

                        //Not sure if this works.
                        WorldPacket data(SMSG_PVP_CREDIT, 4 + 8 + 4);
                        data << honor;

                        // add honor points to player
                        player->ModifyHonorPoints(honor);

                        player->ApplyModUInt32Value(PLAYER_FIELD_TODAY_CONTRIBUTION, honor, true);

                        // announce to player if honor was gained
                        uint32_t is_chat_enable = 0; // not a fan atm
                        if (is_chat_enable)
                        {
                            ss << "You have been awarded |cff4CFF00%i |rHonor.";
                            ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str(), honor);
                        }

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
                        }

                        // apply world buff (if applicable)
                        NerfHerderHelper::ApplyWorldBuff(player);
                    }
                }
            }
        }
    }

    static void RewardHonorableKills(Player* player, Battleground* bg)
    {
        if (!NerfHerder_Enabled) return;
        if (!NerfHerder_Battleground_Enabled) return;
        if (!NerfHerder_Battleground_HKReward) return;

        // if not battleground, bail
        if (!player->GetMap()->IsBattleground()) return;

        // if winner...
        if (bg->GetWinner() == player->GetTeam())
        {
            // amend stats
            player->ApplyModUInt32Value(PLAYER_FIELD_KILLS, NerfHerder_Battleground_HKReward, true);
            player->ApplyModUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS, NerfHerder_Battleground_HKReward, true);

            // trigger achieves
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_EARN_HONORABLE_KILL);
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL_AT_AREA, player->GetAreaId());
        }

        /*
        // can't use this code bc the data is protected in the class

        // load bg data
        Battleground::BattlegroundScoreMap const* bgScores = bg->GetPlayerScores();
        auto const& score = bgScores->find(player->GetGUID().GetCounter());

        if (score != bgScores->end())
        {
            uint32 killingBlows = score->second->KillingBlows;
            uint32 honorableKills = score->second->HonorableKills;

            uint32 count = honorableKills - killingBlows;

            // amend stats
            player->ApplyModUInt32Value(PLAYER_FIELD_KILLS, count, true);
            player->ApplyModUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS, count, true);

            // trigger achieves
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_EARN_HONORABLE_KILL);
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL_AT_AREA, player->GetAreaId());
        }
        else
        {
            // player scores not found
        }
        */
    }
};

std::unordered_map<uint32_t, VendorData> NerfHerderHelper::vendorDataMap = {
    {34060, {2}}, // Doris Volanthius (Horde)
    {34078, {2}}, // Lieutenant Tristia (Alliance)
    {34063, {2}}, // Blood Guard Zar'shi (Horde)
    {34084, {2}}, // Knight-Lieutenant Moonstrike (Alliance)
    {34038, {2}}, // Sergeant Thunderhorn (Horde)
    {34075, {2}}, // Captain Dirgehammer (Alliance)
    {34043, {2}}, // Lady Palanseer (Horde) - jewel crafting vendor
    {34081, {2}}, // Captain O'Neal (Alliance) - jewel crafting vendor
    //{12796, {2}}, // Raider Bork (Horde) - honor mount vendor
    //{12783, {2}}, // Lieutenant Karter (Alliance) - honor mount vendor
    {12788, {1}}, // Legionnaire Teena (Horde) - tbc armor honor vendor
    {12778, {1}}, // Lieutenant Rachel Vaccar (Alliance) - tbc armor honor vendor
    {33934, {2}}, // Ecton Brasstumbler (Neutral) - arena points vendor gadgetzan
    {33935, {2}}, // Evee Copperspring (Neutral) - arena points vendor gadgetzan
    {33939, {2}}, // Argex Irongut (Neutral) - arena points vendor gadgetzan
    {34093, {2}} // Blazzek the Biter (Neutral) - arena points vendor gadgetzan
};

// https://github.com/Questie/Questie/blob/master/ExternalScripts(DONOTINCLUDEINRELEASE)/DBC%20-%20WoW.tools/areatable_wotlk.csv
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

// https://github.com/Questie/Questie/blob/master/ExternalScripts(DONOTINCLUDEINRELEASE)/DBC%20-%20WoW.tools/areatable_wotlk.csv
// https://wowpedia.fandom.com/wiki/Zones_by_level_(original)
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

class NerfHerderCreature : public AllCreatureScript
{
public:
    NerfHerderCreature() : AllCreatureScript("NerfHerderCreature") {}

    void OnAllCreatureUpdate(Creature* creature, uint32 /*diff*/) override
    {
        // add any blanket debuffs (only effects world)
        NerfHerderHelper::ProcessCreatureWorld(creature);

        // add any blanket debuffs (only effects battlegrounds and arenas)
        NerfHerderHelper::ProcessCreatureBattleground(creature);
    }
};

class NerfHerderPlayer : public PlayerScript
{
public:
    NerfHerderPlayer() : PlayerScript("NerfHerderPlayer") {}

    void OnUpdate(Player* player, uint32 /*p_time*/)
    {
        // add any blanket debuffs (only effects battlegrounds and arenas)
        NerfHerderHelper::ProcessPlayerBattleground(player);
    }

    void OnCreatureKill(Player* player, Creature* killed)  //override
    {
        // only effects world
        NerfHerderHelper::RewardHonor(player, killed);

        // only effects battlegrounds
        NerfHerderHelper::RewardXP(player, killed);
    }

    void OnCreatureKilledByPet(Player* player, Creature* killed) //override
    {
        // only effects world
        NerfHerderHelper::RewardHonor(player, killed);

        // only effects battlegrounds
        NerfHerderHelper::RewardXP(player, killed);
    }

    void OnPlayerRemoveFromBattleground(Player* player, Battleground* bg)
    {
        NerfHerderHelper::RewardHonorableKills(player, victim);
    }
};

void AddNerfHerderScripts()
{
    new NerfHerderConfig();
    new NerfHerderHelper();
    new NerfHerderCreature();
    new NerfHerderPlayer();
}