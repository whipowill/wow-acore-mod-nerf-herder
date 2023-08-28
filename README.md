# Nerf Herder

An AzerothCore module for nerfing faction NPCs.

This mod was initially intended for private servers limited to Vanilla or TBC as an easy fix for NPCs over the maximum player level, such as town guards in capitol cities.

But it also includes features for promoting factional warfare to the game by flagging all NPCs of both factions as PvP and granting honor, plunder, and world buffs for killing them.

Does not affect world database.  Does not affect dungeons or raids.

![Nerf Herder](https://i.imgur.com/gbW2964.jpg)

## Configuration

- ``NerfHerder.Enabled`` - enable this module
- ``NerfHerder.PlayerLevelEnabled`` - nerf all NPCs over the max player level (from ``worldserver.conf``)
- ``NerfHerder.ZoneLevelEnabled`` - nerf faction NPCs over the zone recommended level
- ``NerfHerder.HidePvPVendorsEnabled`` - hide PvP vendors who don't belong
- ``NerfHerder.ForcePvPEnabled`` - force all faction NPCs to be flagged as PvP
- ``NerfHerder.HonorPvPEnabled`` - grant honor rewards on PvP flagged NPC kills
- ``NerfHerder.HonorPvPRate`` - adjust the rate of honor gain
- ``NerfHerder.HonorGreyEnabled`` - grey PvP flagged NPCs give honor
- ``NerfHerder.HonorGreyRate`` - adjust the rate of grey honor gain
- ``NerfHerder.HonorPlunderEnabled`` - give plunder reward on honor kill
- ``NerfHerder.HonorPlunderAmountPerLevel`` - amount of money per level of creature
- ``NerfHerder.WorldBuff.Enabled`` - give world buff on kills
- ``NerfHerder.WorldBuff.KillCount`` - number of kills to accumulate to get world buff
- ``NerfHerder.WorldBuff.Cooldown`` - time cooldown between world buffs (minutes)
- ``NerfHerder.WorldBuff.SpellId.01`` - world buff to give option 1
- ``NerfHerder.WorldBuff.SpellId.02`` - world buff to give option 2
- ``NerfHerder.WorldBuff.SpellId.03`` - world buff to give option 3

## The Vision

On a server with only a few players, and no chance to do battlegrounds to accumulate honor, I want to make raiding enemy villages and towns a fun thing to do on a quiet evening.

Some of the villages, even in starter zones, have super high level NPCs that were originally meant to discourage players on PvP servers from ruining things for the other faction.

These guards will be nerfed to a level appropriate for their respective zones.  You'll be able to burn and pillage any town you have the strength to conquer, and you will be rewarded.

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