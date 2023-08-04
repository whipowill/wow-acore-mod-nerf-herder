# Nerf Herder

An AzerothCore module for nerfing various NPCs throughout the game for various reasons.

THIS PROJECT HAS NOT BEEN HEAVILY TESTED.  MY IDEAS MIGHT NOT BE GOODss.

This mod is intended for private servers limited to Vanilla or TBC who need an easy fix for NPCs throughout Azeroth, such as town guards, over the maximum level.

But more than that, it's for servers who want to engage in world PvP by flagging all NPCs of both factions as PvP and granting you honor for each one that you honorably kill.

No MySQL queries are used in this mod, everything is handled dynamically.

## Configuration

- ``NerfHerder.PlayerLevelEnabled`` - nerf all NPCs over the max player level (from ``worldserver.conf``)
- ``NerfHerder.ZoneLevelEnabled`` - nerf faction NPCs over the zone recommended level
- ``NerfHerder.ForcePvPEnabled`` - force all faction NPCs to be flagged as pvp
- ``NerfHerder.HonorPvPEnabled`` - grant honor rewards on pvp flagged NPC kills
- ``NerfHerder.HonorPvPRate`` - adjust the rate of honor gain
- ``NerfHerder.HonorGreyEnabled`` - grey pvp flagged NPCs give honor
- ``NerfHerder.HonorGreyRate`` - adjust the rate of grey honor gain

Note that this mod has no effect on any creatures in dungeons or raids.

## The Vision

On a server with only a few players, and no chance to do battlegrounds to accumulate honor, I want to make raiding enemy villages and towns a fun thing to do on a quiet evening.

I intend this mod to work with [HonorGuard](https://github.com/azerothcore/mod-gain-honor-guard).  This means enemy villages and towns need to be nerfed enough to make them raidable, to make honor farming work.

Some of the villages, even in starter zones, have super high level NPCs that were originally meant to discourage enemy players on PVP servers from ruining things for the other faction.

These guards will be nerfed to a level appropriate for their respective zones.

Finally, the coveted achievement of assassinating the enemy faction leaders is totally undoable without some significant tweaking.  I'm not sure how I will crack this nut yet but I will make an attempt.

This is the ultimate plan.

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
- HonorGuard

## External Links

- [AzerothCore](https://github.com/azerothcore/azerothcore-wotlk)
- [ZoneDebuff](https://github.com/55Honey/Acore_ZoneDebuff/blob/master/zoneDebuff.lua)
- [HonorGuard](https://github.com/azerothcore/mod-gain-honor-guard)