#include "ModNerfHerder.h"
#include "ScriptMgr.h"
#include "CreatureData.h"
#include "Config.h"

class NerfHerder : public CreatureScript
{
public:
    NerfHerder() : CreatureScript("NerfHerder") {}

    void OnCreatureUpdate(Creature* creature, uint32 diff)
    {
        if (creature->IsPlayer())
        {
            return;
        }

        uint32_t is_enabled = sConfigMgr->GetOption<int>("NerfHerder.Enable", 0);
        uint32_t max_level = sConfigMgr->GetOption<int>("MaxPlayerLevel", 60); // <-- from worldserver.conf

        if (is_enabled)
        {
            if (creature->GetLevel() > max_level)
            {
                uint32_t new_level = creature->IsElite() ? max_level : max_level - 5;
                int32_t multiplier = -100 + ((new_level / creature->GetLevel()) * 100); // calc negative multiplier

                // set new level
                creature->SetLevel(new_level);

                // nerf auras
                uint32_t HpAura= 89501;
                uint32_t DamageDoneTakenAura = 89502;
                uint32_t BaseStatAPAura = 89503;
                uint32_t RageFromDamageAura = 89504;
                uint32_t AbsorbAura = 89505;
                uint32_t HealingDoneAura = 89506;
                uint32_t PhysicalDamageTakenAura = 89507;

                // nerf their abilities proportionately
                creature->CastCustomSpell(player, HpAura, &multiplier, NULL, NULL, true, NULL, NULL, creature->GetGUID());
                creature->CastCustomSpell(player, DamageDoneTakenAura, NULL, &multiplier, NULL, true, NULL, NULL, creature->GetGUID());
                creature->CastCustomSpell(player, BaseStatAPAura, &multiplier, &multiplier, &multiplier, true, NULL, NULL, creature->GetGUID());
                //creature->CastCustomSpell(player, RageFromDamageAura, &RageFromDamageModifier, NULL, NULL, true, NULL, NULL, creature->GetGUID());
                creature->CastCustomSpell(player, AbsorbAura, &multiplier, NULL, NULL, true, NULL, NULL, creature->GetGUID());
                creature->CastCustomSpell(player, HealingDoneAura, &multiplier, NULL, NULL, true, NULL, NULL, creature->GetGUID());
                //creature->CastCustomSpell(player, PhysicalDamageTakenAura, &PhysicalDamageTakenModifier, NULL, NULL, true, NULL, NULL, creature->GetGUID());
            }
        }
    }
};

void AddNerfHerderScripts()
{
    new NerfHerder();
}