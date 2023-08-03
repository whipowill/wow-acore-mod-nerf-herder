#include "ModNerfHerder.h"
#include "ScriptMgr.h"
#include "Config.h"
#include "Creature.h"
#include <unordered_map>

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

class NerfHerder
{
public:
    static std::unordered_map<uint32_t, ZoneData> zoneDataMap;
    static std::unordered_map<uint32_t, FactionData> factionDataMap;

    static uint32_t GetCreatureFactionByRace(Creature* creature)
    {
        // I found it very difficult to determine the faction of a creature.
        // GetFaction() returns a sub-faction, and I'd have to match to alliances.
        // IsHorde() and IsAlliance() only works on players.
        // GetReactionTo() requires another creature to compare to, but is limited
        // to only comparing creatures in the same zone.

        // Also tried AreaTable flag, but can't figure out how ot get that info
        // without copious DB queries.

        switch (creature->getRace())
        {
            case RACE_HUMAN:
                return 1;
                break;     // Human
            case RACE_ORC:
                return 2;
                break;     // Orc
            case RACE_DWARF:
                return 1;
                break;     // Dwarf
            case RACE_NIGHTELF:
                return 1;
                break;     // Night Elf
            case RACE_UNDEAD_PLAYER:
                return 2;
                break;// Undead
            case RACE_TAUREN:
                return 2;
                break;     // Tauren
            case RACE_GNOME:
                return 1;
                break;     // Gnome
            case RACE_TROLL:
                return 2;
                break;     // Troll
            case RACE_BLOODELF:
                return 2;
                break;     // Blood Elf
            case RACE_DRAENEI:
                return 1;
                break;     // Draenei
        }

        return 0;
    }

    static uint32_t GetCreatureFactionByFaction(uint32_t faction_id)
    {
        // This does not work and I don't know why.

        if (NerfHerder::factionDataMap.find(faction_id) == NerfHerder::factionDataMap.end()) return 0;
        return NerfHerder::factionDataMap[faction_id].factionTranslatedID;
    }

    static uint32_t GetZoneLevel(uint32_t zone_id)
    {
        if (NerfHerder::zoneDataMap.find(zone_id) == NerfHerder::zoneDataMap.end()) return 0;
        return NerfHerder::zoneDataMap[zone_id].maxLevel;
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
std::unordered_map<uint32_t, FactionData> NerfHerder::factionDataMap = {
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
};

// https://github.com/Questie/Questie/blob/master/ExternalScripts(DONOTINCLUDEINRELEASE)/DBC%20-%20WoW.tools/areatable_wotlk.csv
std::unordered_map<uint32_t, ZoneData> NerfHerder::zoneDataMap = {
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
    {17, {"Barrens", 17, 30, 35, ""}}, // fix from CATA
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

        // pull config
        uint32_t is_enabled = sConfigMgr->GetOption<int>("NerfHerder.Enable", 0);

        // catch errors
        if (!is_enabled) return;

        // init
        uint32_t max_level;

        // determine faction (not really)
        //uint32_t faction = NerfHerder::GetCreatureFactionByFaction(creature->GetFaction());
        uint32_t faction = NerfHerder::GetCreatureFactionByRace(creature->getRace());

        // if max zone level is enabled...
        uint32_t is_zone_level_enabled = sConfigMgr->GetOption<int>("NerfHerder.MaxZoneLevelEnable", 0);
        if (is_zone_level_enabled)
        {
            if (faction == 1 || faction == 2)
            {
                // get max level for zone
                max_level = NerfHerder::GetZoneLevel(creature->GetZoneId());

                // if valid
                if (max_level && max_level >= 10)
                {
                    // if creature is too high...
                    if (creature->GetLevel() > max_level)
                    {
                        // nerf em
                        NerfHerder::UpdateCreature(creature, max_level);
                    }
                }
            }
        }

        // if max player level is enabled...
        uint32_t is_player_level_enabled = sConfigMgr->GetOption<int>("NerfHerder.MaxPlayerLevelEnable", 0);
        if (is_player_level_enabled)
        {
            // get max level for players
            max_level = sConfigMgr->GetOption<int>("MaxPlayerLevel", 80); // <-- from worldserver.conf

            // if valid
            if (max_level && max_level >= 10)
            {
                // if creature is too high...
                if (creature->GetLevel() > max_level)
                {
                    // calc new max level
                    max_level = creature->isElite() ? max_level : max_level - 5;

                    // nerf em
                    NerfHerder::UpdateCreature(creature, max_level);
                }
            }
        }

        // if force pvp is enabled...
        uint32_t is_force_pvp = sConfigMgr->GetOption<int>("NerfHerder.ForceFactionPvPEnable", 0);
        if (is_force_pvp)
        {
            if (faction == 1 || faction == 2)
            {
                // force them to be pvp
                creature->SetPvP(1);
            }
        }
    }
};

void AddNerfHerderScripts()
{
    new NerfHerder();
    new NerfHerderCreature();
}