# Nerf Herder

An AzerothCore module for nerfing various NPCs throughout the game for various reasons.

This mod is primarily intended for servers limited to Vanilla or TBC who need an easy fix for towns people over the maximum level.

THIS PROJECT IS IN IT'S INFANCY.  IT HAS NOT BEEN HEAVILY TESTED.

**What's Working**

- Town Guards
- Faction Leaders (maybe, they still say ??)

**Future Plans**

- PVP Vendors - remove vendors who don't belong
- Zone Limits - make guards have zone appropriate level (for village raiding)
- Capitol Nerfs - make capitol cities raidable somehow

On a server with only a few players, and no chance to do battlegrounds to accumulate honor, I want to make raiding enemy villages and towns a fun thing to do on a quiet evening.

I intend this mod to work with [HonorGuard](https://github.com/azerothcore/mod-gain-honor-guard).  This means enemy villages and towns need to be nerfed enough to make them raidable, to make honor farming work.

Some of the villages, even in starter zones, have super high level NPCs that were originally meant to discourage enemy players on PVP servers from ruining the starting zones for the other faction.

These guards will be nerfed to a level appropriate for their respective zones.

Finally, the coveted achievement of assassinating the enemy faction leaders is totally undoable without some significant tweaking.  I'm not sure how I will crack this nut yet but I will make an attempt.

This is the ultimate plan.

## Methodology

This code runs on every creature on spawn and finds out if they're over the max level limit.  If they are, it cuts non-elites to 5 levels below the limit and elites to the limit, and it nerfs their HP, stats, resistances, damage, and healing proportionately from what they had been before.

## Notes To Self

This module uses [55Honey's ZoneDebuff](https://github.com/55Honey/Acore_ZoneDebuff/blob/master/zoneDebuff.lua) technique to apply the buffs and debuffs.

An explanation of these auras and how to apply them is as follows:

```
// use CastCustomSpell() to apply an aura w/ required params
// player->CastCustomSpell(player, spellID, &param1, &param2, &param3, true, NULL, NULL, player->GetGUID());

uint32_t HpAura = 89501;
player->CastCustomSpell(player, HpAura, &hpModifier, NULL, NULL, true, NULL, NULL, player->GetGUID());

uint32_t DamageDoneTakenAura = 89502;
player->CastCustomSpell(player, DamageDoneTakenAura, &DamageTakenModifier, &DamageDoneModifier, NULL, true, NULL, NULL, player->GetGUID());

uint32_t BaseStatAPAura = 89503;
player->CastCustomSpell(player, BaseStatAPAura, &BaseStatModifier, &MeleeAPModifier, &RangedAPModifier, true, NULL, NULL, player->GetGUID());

uint32_t RageFromDamageAura = 89504;
player->CastCustomSpell(player, RageFromDamageAura, &RageFromDamageModifier, NULL, NULL, true, NULL, NULL, player->GetGUID());

uint32_t AbsorbAura = 89505;
player->CastCustomSpell(player, AbsorbAura, &AbsorbModifier, NULL, NULL, true, NULL, NULL, player->GetGUID());

uint32_t HealingDoneAura = 89506;
player->CastCustomSpell(player, HealingDoneAura, &HealingDoneModifier, NULL, NULL, true, NULL, NULL, player->GetGUID());

uint32_t PhysicalDamageTakenAura = 89507;
player->CastCustomSpell(player, PhysicalDamageTakenAura, &PhysicalDamageTakenModifier, NULL, NULL, true, NULL, NULL, player->GetGUID());

// When you pass the params be sure to use the "&" in front of the var name.
// The param value is a range from -100 to 100+, and represents a percentage change from normal.
```

## Credits

- ChatGPT
- 55Honey

## External Links

- [AzerothCore](https://github.com/azerothcore/azerothcore-wotlk)
- [ZoneDebuff](https://github.com/55Honey/Acore_ZoneDebuff/blob/master/zoneDebuff.lua)