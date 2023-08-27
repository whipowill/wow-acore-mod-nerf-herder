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
#include "Group.h"
#include "Chat.h"
#include <unordered_map>
#include <ctime>

uint32_t NerfHerder_Enabled = 0;

uint32_t NerfHerder_PlayerLevelEnabled = 0;
uint32_t NerfHerder_ZoneLevelEnabled = 0;

uint32_t NerfHerder_HidePvPVendorsEnabled = 0;

uint32_t NerfHerder_ForcePvPEnabled = 0;

uint32_t NerfHerder_HonorPvPEnabled = 0;
float NerfHerder_HonorPvPRate = 0;

uint32_t NerfHerder_HonorGreyEnabled = 0;
float NerfHerder_HonorGreyRate = 0;

float NerfHerder_ExtraNerfRate = 0;

uint32_t NerfHerder_MaxPlayerLevel = 80;

uint32_t NerfHerder_WorldBuff_Enabled = 0;
uint32_t NerfHerder_WorldBuff_KillCount = 0;
uint32_t NerfHerder_WorldBuff_Cooldown = 0;
uint32_t NerfHerder_WorldBuff_SpellId_01 = 0;
uint32_t NerfHerder_WorldBuff_SpellId_02 = 0;
uint32_t NerfHerder_WorldBuff_SpellId_03 = 0;

uint32_t NerfHerder_WorldBuff_Alliance_LastKillTime = 0;
uint32_t NerfHerder_WorldBuff_Alliance_LastBuffTime = 0;
uint32_t NerfHerder_WorldBuff_Alliance_LastKillCount = 0;

uint32_t NerfHerder_WorldBuff_Horde_LastKillTime = 0;
uint32_t NerfHerder_WorldBuff_Horde_LastBuffTime = 0;
uint32_t NerfHerder_WorldBuff_Horde_LastKillCount = 0;

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
        NerfHerder_Enabled = sConfigMgr->GetOption<int>("NerfHerder.Enabled", 0);

        NerfHerder_PlayerLevelEnabled = sConfigMgr->GetOption<int>("NerfHerder.PlayerLevelEnabled", 0);
        NerfHerder_ZoneLevelEnabled = sConfigMgr->GetOption<int>("NerfHerder.ZoneLevelEnabled", 0);

        NerfHerder_HidePvPVendorsEnabled = sConfigMgr->GetOption<int>("NerfHerder.HidePvPVendorsEnabled", 0);

        NerfHerder_ForcePvPEnabled = sConfigMgr->GetOption<int>("NerfHerder.ForcePvPEnabled", 0);

        NerfHerder_HonorPvPEnabled = sConfigMgr->GetOption<int>("NerfHerder.HonorPvPEnabled", 0);
        NerfHerder_HonorPvPRate = sConfigMgr->GetOption<int>("NerfHerder.HonorPvPRate", 0);

        NerfHerder_HonorGreyEnabled = sConfigMgr->GetOption<int>("NerfHerder.HonorGreyEnabled", 0);
        NerfHerder_HonorGreyRate = sConfigMgr->GetOption<int>("NerfHerder.HonorGreyRate", 0);

        NerfHerder_ExtraNerfRate = sConfigMgr->GetOption<int>("NerfHerder.ExtraNerfRate", 0);

        NerfHerder_MaxPlayerLevel = sConfigMgr->GetOption<int>("MaxPlayerLevel", 80); // <-- from worldserver.conf

        NerfHerder_WorldBuff_Enabled = sConfigMgr->GetOption<int>("NerfHerder.WorldBuff.Enabled", 0);
        NerfHerder_WorldBuff_KillCount = sConfigMgr->GetOption<int>("NerfHerder.WorldBuff.KillCount", 0);
        NerfHerder_WorldBuff_Cooldown = sConfigMgr->GetOption<int>("NerfHerder.WorldBuff.Cooldown", 0);
        NerfHerder_WorldBuff_SpellId_01 = sConfigMgr->GetOption<int>("NerfHerder.WorldBuff.SpellId.01", 0);
        NerfHerder_WorldBuff_SpellId_02 = sConfigMgr->GetOption<int>("NerfHerder.WorldBuff.SpellId.02", 0);
        NerfHerder_WorldBuff_SpellId_03 = sConfigMgr->GetOption<int>("NerfHerder.WorldBuff.SpellId.03", 0);
    }
};

struct VendorData {
    std::string vendorName;
    uint32_t teamID;
    uint32_t expansionID; // 0, 1, or 2
    uint32_t entryID;
};

struct ZoneData {
    std::string zoneName;
    uint32_t mapID;
    uint32_t minLevel;
    uint32_t maxLevel;
    std::string expansion;
};

class NerfHerderHelper
{
public:
    static std::unordered_map<uint32_t, VendorData> vendorDataMap;
    static std::unordered_map<uint32_t, ZoneData> zoneDataMap;
    static std::unordered_set<uint32_t> townDataMap;

    static void ApplyWorldBuff(Player* player)
    {
        // if disabled, bail
        if (!NerfHerder_WorldBuff_Enabled) return;

        // log the time
        uint32_t timestamp = std::time(nullptr);

        // flight checks
        if (player->GetTeamId() == TEAM_ALLIANCE)
        {
            // has it been enough time since last world buff?  if not, bail
            if ((timestamp - NerfHerder_WorldBuff_Alliance_LastBuffTime) < (NerfHerder_WorldBuff_Cooldown * 60))
                return;

            // do we need to reset the faction kill count?
            if ((timestamp - NerfHerder_WorldBuff_Alliance_LastKillTime) >= (NerfHerder_WorldBuff_Cooldown * 60))
                NerfHerder_WorldBuff_Alliance_LastKillCount = 0;

            // update faction kill count
            NerfHerder_WorldBuff_Alliance_LastKillTime = timestamp;
            NerfHerder_WorldBuff_Alliance_LastKillCount++;

            // have we achieved enough kills?  if not, bail
            if (NerfHerder_WorldBuff_Alliance_LastKillCount < NerfHerder_WorldBuff_KillCount)
                return;

            // at this point, we are going to world buff so log it
            NerfHerder_WorldBuff_Alliance_LastBuffTime = timestamp;
        }
        else if (player->GetTeamId() == TEAM_HORDE)
        {
            // has it been enough time since last world buff?  if not, bail
            if ((timestamp - NerfHerder_WorldBuff_Horde_LastBuffTime) < (NerfHerder_WorldBuff_Cooldown * 60))
                return;

            // do we need to reset the faction kill count?
            if ((timestamp - NerfHerder_WorldBuff_Horde_LastKillTime) >= (NerfHerder_WorldBuff_Cooldown * 60))
                NerfHerder_WorldBuff_Horde_LastKillCount = 0;

            // update faction kill count
            NerfHerder_WorldBuff_Horde_LastKillTime = timestamp;
            NerfHerder_WorldBuff_Horde_LastKillCount++;

            // have we achieved enough kills?  if not, bail
            if (NerfHerder_WorldBuff_Horde_LastKillCount < NerfHerder_WorldBuff_KillCount)
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

        // find any NPC in an alliance/horde town
        if (NerfHerderHelper::townDataMap.find(area_id) == NerfHerderHelper::townDataMap.end())
            return 0;

        return 1;

        // I know this is not the perfect way to identify
        // Alliance and Horde NPCs.  I have tried many methods
        // and thus far this is the best way I've found.

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

        if (creature->IsPvP()) return 1;
        /*
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

        return 0;
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

    static void UpdateCreature(Creature* creature, uint32_t new_level)
    {
        // nerf auras
        uint32_t HpAura = 89501;
        uint32_t DamageDoneTakenAura = 89502;
        uint32_t BaseStatAPAura = 89503;
        //uint32_t RageFromDamageAura = 89504;
        uint32_t AbsorbAura = 89505;
        uint32_t HealingDoneAura = 89506;
        //uint32_t PhysicalDamageTakenAura = 89507;

        // if creature already modified, bail
        if (creature->HasAura(DamageDoneTakenAura)) return;

        // calc negative multiplier
        float ratio = static_cast<float>(new_level) / static_cast<float>(creature->GetLevel());
        float multiplier = (-100 * (1 + NerfHerder_ExtraNerfRate)) + (ratio * 100);

        // convert to int
        int32_t negative_multiplier = static_cast<int>(multiplier);

        // just in case something goes wrong
        if (negative_multiplier > 0) negative_multiplier = 0;

        // nerf their abilities proportionately
        creature->CastCustomSpell(creature, HpAura, &negative_multiplier, NULL, NULL, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, DamageDoneTakenAura, 0, &negative_multiplier, NULL, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, BaseStatAPAura, &negative_multiplier, &negative_multiplier, &negative_multiplier, true, NULL, NULL, creature->GetGUID());
        //creature->CastCustomSpell(creature, RageFromDamageAura, &RageFromDamageModifier, NULL, NULL, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, AbsorbAura, &negative_multiplier, NULL, NULL, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, HealingDoneAura, &negative_multiplier, NULL, NULL, true, NULL, NULL, creature->GetGUID());
        //creature->CastCustomSpell(creature, PhysicalDamageTakenAura, &PhysicalDamageTakenModifier, NULL, NULL, true, NULL, NULL, creature->GetGUID());

        // set new level
        creature->SetLevel(new_level, false); // flag false to bypass any hooray animations
    }
};

std::unordered_map<uint32_t, VendorData> NerfHerderHelper::vendorDataMap = {
    {34060, {"Doris Volanthius", 2, 2, 34060}},
    {34078, {"Lieutenant Tristia", 1, 2, 34078}},
    {34063, {"Blood Guard Zar'shi", 2, 2, 34063}},
    {34084, {"Knight-Lieutenant Moonstrike", 1, 2, 34084}},
    {34038, {"Sergeant Thunderhorn", 2, 2, 34038}},
    {34075, {"Captain Dirgehammer", 1, 2, 34075}},
    {34043, {"Lady Palanseer", 2, 2, 34043}}, // jewel crafting vendor
    {34081, {"Captain O'Neal", 1, 2, 34081}}, // jewel crafting vendor
    //{12796, {"Raider Bork", 2, 2, 12796}}, // honor mount vendor
    //{12783, {"Lieutenant Karter", 1, 2, 12783}}, // honor mount vendor
    {12788, {"Legionnaire Teena", 2, 1, 12788}}, // tbc armor honor vendor
    {12778, {"Lieutenant Rachel Vaccar", 1, 1, 12778}}, // tbc armor honor vendor
    {33934, {"Ecton Brasstumbler", 3, 2, 33934}}, // arena points vendor gadgetzan
    {33935, {"Evee Copperspring", 3, 2, 33935}}, // arena points vendor gadgetzan
    {33939, {"Argex Irongut", 3, 2, 33939}}, // arena points vendor gadgetzan
    {34093, {"Blazzek the Biter", 3, 2, 34093}} // arena points vendor gadgetzan
};

// https://github.com/Questie/Questie/blob/master/ExternalScripts(DONOTINCLUDEINRELEASE)/DBC%20-%20WoW.tools/areatable_wotlk.csv
std::unordered_set<uint32_t> NerfHerderHelper::townDataMap =
{
    496, // Brackenwall Village
    513, // Theramore Isle
    1497, // Undercity
    1637, // Orgrimmar
    1638, // Thunder Bluff
    3487, // Silvermoon City
};

// https://github.com/Questie/Questie/blob/master/ExternalScripts(DONOTINCLUDEINRELEASE)/DBC%20-%20WoW.tools/areatable_wotlk.csv
// https://wowpedia.fandom.com/wiki/Zones_by_level_(original)
std::unordered_map<uint32_t, ZoneData> NerfHerderHelper::zoneDataMap =
{
    // 1-10
    {3524, {"Azuremyst Isle", 3524, 1, 10, "BC"}},
    {1, {"Dun Morogh", 1, 1, 10, ""}},
    {14, {"Durotar", 14, 1, 10, ""}},
    {12, {"Elwynn Forest", 12, 1, 10, ""}},
    {3430, {"Eversong Woods", 3430, 1, 10, "BC"}},
    {215, {"Mulgore", 215, 1, 10, ""}},
    {141, {"Teldrassil", 141, 1, 10, ""}},
    {85, {"Tirisfal Glades", 85, 1, 10, ""}},

    // 10-20
    {3525, {"Bloodmyst Isle", 3525, 10, 20, "BC"}},
    {148, {"Darkshore", 148, 10, 20, ""}},
    {3433, {"Ghostlands", 3433, 10, 20, "BC"}},
    {38, {"Loch Modan", 38, 10, 20, ""}},
    {130, {"Silverpine Forest", 130, 10, 20, ""}},
    {40, {"Westfall", 40, 10, 20, ""}},
    {17, {"Barrens", 17, 10, 25, ""}},

    // 15-25
    {44, {"Redridge Mountains", 44, 15, 25, ""}},
    {406, {"Stonetalon Mountains", 406, 15, 27, ""}},

    // 20-30
    {331, {"Ashenvale", 331, 18, 30, ""}},
    {10, {"Duskwood", 10, 18, 30, ""}},
    {267, {"Hillsbrad Foothills", 267, 20, 30, ""}},
    {11, {"Wetlands", 11, 20, 30, ""}},

    // 25-35
    {400, {"Thousand Needles", 400, 25, 35, ""}},

    // 30-40
    {36, {"Alterac Mountains", 36, 30, 40, ""}},
    {45, {"Arathi Highlands", 45, 30, 40, ""}},
    {405, {"Desolace", 405, 30, 40, ""}},
    {33, {"Stranglethorn Vale", 33, 30, 45, ""}},

    // 35-45
    {15, {"Dustwallow Marsh", 15, 35, 45, ""}},
    {3, {"Badlands", 3, 35, 45, ""}},
    {51, {"Swamp of Sorrows", 51, 35, 45, ""}},

    // 40-50
    {357, {"Feralas", 357, 40, 50, ""}},
    {47, {"Hinterlands", 47, 40, 50, ""}},
    {440, {"Tanaris", 440, 40, 50, ""}},

    // 45-55
    {51, {"Searing Gorge", 51, 45, 50, ""}},
    {16, {"Azshara", 16, 45, 55, ""}},
    {4, {"Blasted Lands", 4, 45, 55, ""}},
    {490, {"Un'goro Crater", 490, 48, 55, ""}},
    {361, {"Felwood", 361, 48, 55, ""}},

    // 50-60
    {38, {"Burning Steppes", 38, 50, 58, ""}},
    {28, {"Western Plaguelands", 28, 51, 58, ""}},
    {139, {"Eastern Plaguelands", 139, 53, 60, ""}},
    {618, {"Winterspring", 618, 53, 60, ""}},

    // 55-60
    {609, {"Plaguelands: The Scarlet Enclave", 609, 55, 58, "WOTLK"}},
    {41, {"Deadwind Pass", 41, 55, 60, ""}},
    {80, {"Moonglade", 80, 55, 60, ""}},
    {261, {"Silithus", 261, 55, 60, ""}},
    {36, {"Blackrock Mountain", 36, 55, 60, ""}}, // not in official list?
    {772, {"Ahn'Qiraj", 772, 55, 60, ""}}, // not in official list?

    // 60-70
    {3483, {"Hellfire Peninsula", 3483, 58, 63, "BC"}},
    {3521, {"Zangarmarsh", 3521, 60, 64, "BC"}},
    {3519, {"Terokkar Forest", 3519, 62, 65, "BC"}},
    {3518, {"Nagrand", 3518, 64, 67, "BC"}},
    {3522, {"Blade's Edge Mountains", 3522, 65, 68, "BC"}},
    {3523, {"Netherstorm", 3523, 67, 70, "BC"}},
    {3520, {"Shadowmoon Valley", 3520, 67, 70, "BC"}},
    {4080, {"Isle of Quel'Danas", 4080, 70, 73, "BC"}},

    // 70-80
    {3537, {"Borean Tundra", 3537, 68, 72, "WOTLK"}},
    {495, {"Howling Fjord", 495, 68, 72, "WOTLK"}},
    {65, {"Dragonblight", 65, 71, 75, "WOTLK"}},
    {394, {"Grizzly Hills", 394, 73, 75, "WOTLK"}},
    {66, {"Zul'Drak", 66, 74, 76, "WOTLK"}},
    {67, {"Sholazar Basin", 67, 76, 78, "WOTLK"}},
    {2817, {"Crystalsong Forest", 2817, 77, 80, "WOTLK"}},
    {4742, {"Hrothgar's Landing", 4742, 77, 80, "WOTLK"}},
    {210, {"Icecrown", 210, 77, 80, "WOTLK"}},
    {67, {"Storm Peaks", 67, 77, 80, "WOTLK"}},
    {4197, {"Wintergrasp", 4197, 77, 80, "WOTLK"}}
};

class NerfHerderCreature : public AllCreatureScript
{
public:
    NerfHerderCreature() : AllCreatureScript("NerfHerderCreature") {}

    // Note to self, OnCreatureAddWorld doesn't work.
    void OnAllCreatureUpdate(Creature* creature, uint32 diff) override
    {
        // catch errors
        if (!creature) return;
        if (creature->IsPlayer()) return;
        if (creature->GetMap()->IsDungeon() || creature->GetMap()->IsRaid() || creature->GetMap()->IsBattleground()) return;

        // catch errors
        if (!NerfHerder_Enabled) return;

        // init
        uint32_t max_level;

        // determine alliance / horde npcs in the world
        uint32_t is_field_agent = NerfHerderHelper::IsFieldAgent(creature);

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
                        NerfHerderHelper::UpdateCreature(creature, max_level);
                    }
                }
            }
        }

        // if max player level is enabled...
        if (NerfHerder_PlayerLevelEnabled)
        {
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
                    NerfHerderHelper::UpdateCreature(creature, max_level);
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
};

class NerfHerderPlayer : public PlayerScript
{
public:
    NerfHerderPlayer() : PlayerScript("NerfHerderPlayer") {}

    // This was all taken straight from HonorGuard mod, but tweaked to
    // give honor on any pvp flagged creature.
    // https://github.com/azerothcore/mod-gain-honor-guard/blob/master/src/GainHonorGuard.cpp

    void OnCreatureKill(Player* player, Creature* killed)  //override
    {
        RewardHonor(player, killed);
    }

    void OnCreatureKilledByPet(Player* player, Creature* killed) //override
    {
        RewardHonor(player, killed);
    }

    void RewardHonor(Player* player, Creature* killed)
    {
        if (NerfHerder_HonorPvPEnabled && player->IsAlive() && !player->InArena() && !player->HasAura(SPELL_AURA_PLAYER_INACTIVE))
        {
            if (killed || !killed->HasAuraType(SPELL_AURA_NO_PVP_CREDIT))
            {
                if ((NerfHerder_HonorPvPEnabled && killed->ToCreature()->IsPvP()))
                {
                    std::ostringstream ss;
                    int honor = -1; //Honor is added as an int
                    float honor_f = (float)honor; //Convert honor to float for calculations
                    player->UpdateHonorFields();

                    int groupsize = GetNumInGroup(player); //Determine if it was a gang beatdown

                    //Determine level that is gray
                    uint8 k_level = player->getLevel();
                    uint8 k_grey = Acore::XP::GetGrayLevel(k_level);
                    uint8 v_level = killed->getLevel();

                    // handle grey override setting
                    float honor_multiplier = NerfHerder_HonorPvPRate;
                    if (v_level <= k_grey) // if npc was too low
                    {
                        if (NerfHerder_HonorGreyEnabled)
                        {
                            v_level = k_grey + 1; // treat npc as just above limit
                            honor_multiplier = NerfHerder_HonorGreyRate;
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
                            //A Gang beatdown of an enemy rewards less honor
                            if (groupsize > 1)
                                honor_f /= groupsize;

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

                        // apply world buff
                        NerfHerderHelper::ApplyWorldBuff(player);
                    }
                }
            }
        }
    }

    // Get the player's group size
    int GetNumInGroup(Player* player)
    {
        int numInGroup = 1;
        Group *group = player->GetGroup();
        if (group)
        {
            Group::MemberSlotList const& groupMembers = group->GetMemberSlots();
            numInGroup = groupMembers.size();
        }
        return numInGroup;
    }
};

void AddNerfHerderScripts()
{
    new NerfHerderConfig();
    new NerfHerderHelper();
    new NerfHerderCreature();
    new NerfHerderPlayer();
}