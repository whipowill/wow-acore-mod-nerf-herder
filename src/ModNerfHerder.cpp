#include "ModNerfHerder.h"
#include "ScriptMgr.h"
#include "Config.h"
#include "World.h"
#include "WorldPacket.h"
#include "Unit.h"
#include "Creature.h"
#include "Player.h"
#include "Group.h"
#include "Chat.h"
#include <unordered_map>

uint32_t NerfHerder_Enabled = 0;
uint32_t NerfHerder_PlayerLevelEnabled = 0;
uint32_t NerfHerder_ZoneLevelEnabled = 0;
uint32_t NerfHerder_ForcePvPEnabled = 0;
uint32_t NerfHerder_HonorPvPEnabled = 0;
float NerfHerder_HonorPvPRate = 0;
uint32_t NerfHerder_MaxPlayerLevel = 80;

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
        NerfHerder_ForcePvPEnabled = sConfigMgr->GetOption<int>("NerfHerder.ForcePvPEnabled", 0);
        NerfHerder_HonorPvPEnabled = sConfigMgr->GetOption<int>("NerfHerder.HonorPvPEnabled", 0);
        NerfHerder_MaxPlayerLevel = sConfigMgr->GetOption<int>("MaxPlayerLevel", 80); // <-- from worldserver.conf
    }
};

struct ZoneData {
    std::string zoneName;
    uint32_t mapID;
    uint32_t minLevel;
    uint32_t maxLevel;
    std::string expansion;
};

struct FactionData {
    std::string factionName;
    uint32_t factionID;
    uint32_t factionTranslatedID; // 1=ally 2=horde
};

class NerfHerderHelper
{
public:
    static std::unordered_map<uint32_t, ZoneData> zoneDataMap;
    static std::unordered_map<uint32_t, FactionData> factionDataMap;

    static uint32_t IsFieldAgent(Creature* creature)
    {
        // I know this is not the perfect way to identify
        // Alliance and Horde NPCs.  I have tried many methods
        // and thus far this is the best way I've found.

        if (creature->IsPvP()) return 1;
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

        // set new level
        creature->SetLevel(new_level, false); // flag false to bypass any hooray animations

        // calc negative multiplier
        int32_t multiplier = -100 + ((new_level / creature->GetLevel()) * 100);

        // just in case
        if (multiplier > 0) multiplier = 0;

        // nerf their abilities proportionately
        creature->CastCustomSpell(creature, HpAura, &multiplier, NULL, NULL, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, DamageDoneTakenAura, 0, &multiplier, NULL, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, BaseStatAPAura, &multiplier, &multiplier, &multiplier, true, NULL, NULL, creature->GetGUID());
        //creature->CastCustomSpell(creature, RageFromDamageAura, &RageFromDamageModifier, NULL, NULL, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, AbsorbAura, &multiplier, NULL, NULL, true, NULL, NULL, creature->GetGUID());
        creature->CastCustomSpell(creature, HealingDoneAura, &multiplier, NULL, NULL, true, NULL, NULL, creature->GetGUID());
        //creature->CastCustomSpell(creature, PhysicalDamageTakenAura, &PhysicalDamageTakenModifier, NULL, NULL, true, NULL, NULL, creature->GetGUID());
    }
};

// https://www.azerothcore.org/wiki/faction
std::unordered_map<uint32_t, FactionData> NerfHerderHelper::factionDataMap = {
    {189, {"Alliance Generic", 189, 1}},
    {469, {"Alliance", 469, 1}},
    {891, {"Alliance Forces", 891, 1}},
    {1037, {"Alliance Vanguard", 1037, 1}},
    {1037, {"Mount - Taxi - Alliance", 1037, 1}},
    {66, {"Horde Generic", 66, 2}},
    {67, {"Horde", 67, 2}},
    {892, {"Horde Forces", 892, 2}},
    {1052, {"Horde Expedition", 1052, 2}},
    {1113, {"Mount - Taxi - Horde", 1113, 2}}
}; // I didn't end up using this, but I leave it here in case someday I do.

// https://github.com/Questie/Questie/blob/master/ExternalScripts(DONOTINCLUDEINRELEASE)/DBC%20-%20WoW.tools/areatable_wotlk.csv
// https://wowpedia.fandom.com/wiki/Zones_by_level_(Cataclysm)
std::unordered_map<uint32_t, ZoneData> NerfHerderHelper::zoneDataMap = {
    {3524, {"Azuremyst Isle", 3524, 1, 10, "BC"}},
    {1, {"Dun Morogh", 1, 1, 10, ""}},
    {14, {"Durotar", 14, 1, 10, ""}},
    {12, {"Elwynn Forest", 12, 1, 10, ""}},
    {3430, {"Eversong Woods", 3430, 1, 10, "BC"}},
    {215, {"Mulgore", 215, 1, 10, ""}},
    {141, {"Teldrassil", 141, 1, 10, ""}},
    {85, {"Tirisfal Glades", 85, 1, 10, ""}},
    {40, {"Westfall", 40, 10, 15, ""}},
    {16, {"Azshara", 16, 10, 20, ""}},
    {3525, {"Bloodmyst Isle", 3525, 10, 20, "BC"}},
    {148, {"Darkshore", 148, 10, 20, ""}},
    {3433, {"Ghostlands", 3433, 10, 20, "BC"}},
    {38, {"Loch Modan", 38, 10, 20, ""}},
    {130, {"Silverpine Forest", 130, 10, 20, ""}},
    {5449, {"Ruins of Gilneas", 5449, 14, 20, ""}},
    {44, {"Redridge Mountains", 44, 15, 20, ""}},
    {331, {"Ashenvale", 331, 20, 25, ""}},
    {10, {"Duskwood", 10, 20, 25, ""}},
    {267, {"Hillsbrad Foothills", 267, 20, 25, ""}},
    {11, {"Wetlands", 11, 20, 25, ""}},
    {45, {"Arathi Highlands", 45, 25, 30, ""}},
    {33, {"Stranglethorn Vale", 33, 25, 30, ""}}, // fix from CATA
    {406, {"Stonetalon Mountains", 406, 25, 30, ""}},
    {405, {"Desolace", 405, 30, 35, ""}},
    {47, {"Hinterlands", 47, 30, 35, ""}},
    {17, {"Barrens", 17, 10, 20, ""}}, // fix from CATA
    {15, {"Dustwallow Marsh", 15, 35, 40, ""}},
    {357, {"Feralas", 357, 35, 40, ""}},
    {28, {"Western Plaguelands", 28, 35, 40, ""}},
    {139, {"Eastern Plaguelands", 139, 40, 45, ""}},
    {400, {"Thousand Needles", 400, 40, 45, ""}},
    {3, {"Badlands", 3, 45, 48, ""}},
    {361, {"Felwood", 361, 45, 50, ""}},
    {440, {"Tanaris", 440, 45, 50, ""}},
    {51, {"Searing Gorge", 51, 47, 51, ""}},
    {36, {"Blackrock Mountain", 36, 48, 52, ""}},
    {38, {"Burning Steppes", 38, 50, 52, ""}},
    {490, {"Un'goro Crater", 490, 50, 55, ""}},
    {618, {"Winterspring", 618, 50, 55, ""}},
    {51, {"Swamp of Sorrows", 51, 52, 54, ""}},
    {4, {"Blasted Lands", 4, 55, 60, ""}},
    {609, {"Plaguelands: The Scarlet Enclave", 609, 55, 56, "WOTLK"}},
    {41, {"Deadwind Pass", 41, 55, 60, ""}},
    {80, {"Moonglade", 80, 55, 60, ""}},
    {261, {"Silithus", 261, 55, 60, ""}},
    {772, {"Ahn'Qiraj: The Fallen Kingdom", 772, 55, 60, ""}},
    {3483, {"Hellfire Peninsula", 3483, 58, 63, "BC"}},
    {3521, {"Zangarmarsh", 3521, 60, 64, "BC"}},
    {3519, {"Terokkar Forest", 3519, 62, 65, "BC"}},
    {3518, {"Nagrand", 3518, 64, 67, "BC"}},
    {3522, {"Blade's Edge Mountains", 3522, 65, 68, "BC"}},
    {3523, {"Netherstorm", 3523, 67, 70, "BC"}},
    {3520, {"Shadowmoon Valley", 3520, 67, 70, "BC"}},
    {3537, {"Borean Tundra", 3537, 68, 72, "WOTLK"}},
    {495, {"Howling Fjord", 495, 68, 72, "WOTLK"}},
    {4080, {"Isle of Quel'Danas", 4080, 70, 70, "BC"}},
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
                max_level = NerfHerderHelper::GetZoneLevel(creature) + 5;

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
                creature->SetPvP(true); // no worky
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
                        if (NerfHerder_HonorPvPRate)
                        {
                            honor_f *= NerfHerder_HonorPvPRate;
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

                        /*
                        //announce to player if honor was gained
                        if (GainHonorGuardOnGuardKill && killed->ToCreature()->IsGuard() && GainHonorGuardOnGuardKillAnnounce)
                        {
                            ss << "You have been awarded |cff4CFF00%i |rHonor.";
                            ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str(), honor);
                        }
                        else if (GainHonorGuardOnEliteKill && killed->ToCreature()->isElite() && GainHonorGuardOnEliteKillAnnounce)
                        {
                            ss << "You have been awarded |cffFF8000%i |rHonor.";
                            ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str(), honor);
                        }
                        */
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