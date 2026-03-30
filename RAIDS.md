# Dungeon & Raid Nerfs

This is about solving bosses that I can't defeat due to playing solo, or due to having a team of imbecile robots who don't know when it's time to duck and cover.  It's my way of removing the advanced mechanics so I can progress.

I'm playing w/ [AutoBalance](https://github.com/azerothcore/mod-AutoBalance), [NPCBots](https://github.com/trickerer/AzerothCore-wotlk-with-NPCBots), and [Vanilla Naxx](https://github.com/sogladev/mod-vanilla-naxxramas) mods.  While AutoBalance does 90% of the work in scaling enemy mobs, there are mechanical issues not addressed by AutoBalance.

I've been working my way thru the game tuning raid bosses as I go.  The goal is to nerf boss abilities just enough to keep them challenging.  If you think I've nerfed something too hard, or missed something, open a ticket and let me know.

**This is a work in progress and reflects the content I've been working thru on my own server.**

- Vanilla - 99% complete
- TBC - 80% complete (only Illidan, Zul'Aman, and Sunwell Plateau left)
- WOTLK - 0% complete

## Vanilla

### Darrowshire

- Captain Redpath
    - Takes no damage

This raid/quest is [bugged](https://github.com/chromiecraft/chromiecraft/issues/7844) in Azerothcore and cannot be completed.

### Scholomance

- Malicia
    - Dark Shade removed

### Black Wing Lair

- Vaelastrasz
    - Burning Adrenaline AOE reduced 70%

### Ahn'Qiraj

- Viscidus
    - Freezing phase removed

- Princess Huhuran
    - Acid Spit reduced 30%
    - Poison Bolt reduced 70%

- Twin Emperors
    - Healing disabled
    - [MySQL](https://github.com/whipowill/sh-azerothcore/tree/master/patches/FixClassicAQ.sql) needed to fix immunities

- Ouro
    - Quake less severe

- C'Thun
    - Eye Beam reduced 90%
    - Dark Glare reduced 90%
    - Phase 2 removed (undoable w/out team)

### Naxxramas

- Razuvious
    - Unbalancing Strike reduced 50%

- Four Horsemen
    - Damage reduced 50%

- Patchwerk
    - Hateful Strike reduced 50%

- Gluth
    - Zombie Chow removed

- Thaddius
    - Health reduced 50%
    - Tesla Coil reduced 50%
    - Chain Lightning reduced 50%
    - Charges reduced 50%

- Sapphiron
    - Frost Breath reduced 90%

### World Bosses

- Faction Leaders
    - Health reduced (depends on server max lvl)
    - Damage reduced (depends on server max lvl)

## The Burning Crusade

### Sethek Halls

- Time-Lost Controller (trash mob)
    - Charming Totem removed

- Talon King Ikiss
    - Arcane Explosion reduced 50%

### The Mechanar

- Nethermancer Sepethrea
    - Raging Flames removed

### Shattered Halls

- Warchief Kargath Bladefist
    - Blade Dance reduced 50%

### Karazhan

- Nightbane
    - Damage reduced 30%

- Netherspite
    - Nether Portal effects removed
    - Damage reduced 30%
    - Healing disabled

- Chess Event
    - Play as Horde King and you cannot die
    - Bloodlust kills off Horde pawns (so you can move)
    - Heroism kills off Alliance pawns (so you can reach opponent)
    - Cleave does 10x damage

- Prince Malchezaar
    - Enfeeble triggers a heal to help you recover

### Magtheridon's Lair

- Hellfire Channeler / Warder (trash mob)
    - Damage reduced 50%
    - Healing disabled

- Magtheridon
    - Blast Nova reduced 50%

### Tempest Keep

- Al'ar
    - Ember of Al'ar (adds) removed
    - Flame Quills reduced 100%
    - Bleeds down to 50% health (annoying encounter)

- Void Reaver
    - Saw Blade reduced 50% (from Crystalcore Mechanic adds)
    - Arcane Orb no longer silences

- High Astromancer Solarian
    - Domination charm effect removed (from Nether Scryer adds)

- Kael'thas Sunstrider
    - Mind Control removed
    - Flamestrike removed
    - Pyroblast reduced 50%
    - Phoenix Egg removed

### Serpentshrine Caverns

- Hydross the Unstable
    - Damage reduced 30%

- Leotheras the Blind
    - Consuming Madness removed

- Lady Vashj
    - Removed phase 2 (undoable w/out team)

### Black Temple

- Shade of Akama
    - Wing Clip removed (from adds)

- Reliquary of the Lost
    - Auras removed (buggy, so it pulses)

- Teron Gorefiend
    - Shadowy Constructs removed

- Illidari Council
    - Healing disabled
    - Lady Malande bleeds (bug report in code comments)

- Illidan
    - Physical damage reduced 20%
    - Magic damage reduced 60% (all AOEs)
    - Flames of Azzinoth are short lived and do less damage
    - Parasitic Shadowfiends removed (adds)
    - Shadow Demons removed (adds)

### Hyjal Summit

- Anetheron
    - Healing disabled
    - Carrion Swarm disabled

- Gargoyles (flying trash)
    - Removed

- Frost Wyrms (flying trash)
    - Removed

- Archimonde
    - Finger of Death removed

### Zul'Aman

- TO DO

### Sunwell Plateau

- TO DO

### World Bosses

- Doom Lord Kazzak (Hellfire Peninsula)
    - Damage reduced 30%
    - Healing disabled
    - Mark of Kazzak removed

- Doomwalker (Shadowmoon Valley)
    - Chain Lightning reduced 70%
    - Mark of Death removed

## Wrath of the Lich King

NOT HERE YET