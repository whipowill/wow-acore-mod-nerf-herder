# Nerf Herder

An AzerothCore module for nerfing creatures en masse for various purposes.

Whereas [AutoBalance](https://github.com/azerothcore/mod-AutoBalance) is useful for managing the difficulty of dungeons and raids, this mod is primarily focused on managing the difficulty of the open world.

Additional purposes are to nerf pvp encounters w/ changes to world and bg rewards, to nerf raid encounters so they can be completed solo (or w/ small group), and to nerf players when partied w/ low level friends.

## Features

Each of these features is optional.  Does not affect world database.  Uninstalling the mod and restarting the server will erase any actions taken by this mod.

- World
    - Nerf capitol guards to match max player level
    - Nerf town guards to match zone recommended level
    - Nerf faction leaders to be defeatable w/ small party
    - Nerf world bosses to be defeatable w/ small party
- PvP
    - Nerf battleground damage and healing as a percentage
    - Customize battleground rewards w/ win, lose, and per kill settings
    - Flag all world faction NPCs as PvP who grant honor (and gold) on kill
    - Incentivize faction town raids by giving a chance for world buff
- Heroic
    - Nerf heroic dungeons with a flat percentage damage cut
- Raid
    - Nerf specific encounters to be completable solo or w/ small group (see [details](https://github.com/whipowill/wow-acore-mod-nerf-herder/tree/master/RAIDS.md)) **<-- most development is here now**
- Party
    - Nerf high level players when partied w/ low level friends

## AutoBalance

Below are my recommended "curve floors" for [AutoBalance](https://github.com/azerothcore/mod-AutoBalance) settings.  I share this just as a helpful guide for what has worked really well for me in tuning my server to work with few players while not making instances rediculously easy:

```
5 man = 0.26
10 man = 0.16
15 man = 0.14
20 man = 0.12
25 man = 0.08
40 man = 0.03
```

Essentially, this works out to making dungeons have a minimum difficulty floor of 2 players for dungeons and 5 players for raids (meaning, the strength of the mobs is as if you entered the dungeon w/ this many players, even if you enter with one).

So you can't just waltz into a end game raid and dominate by yourself, bc these settings won't allow mobs in a raid or dungeon to be easier than mobs in the open world.  These settings maintain a difficulty curve that makes sense and feels consistent w/ how difficult mobs are in the open world.

I also recommend limiting crowd control abilities to 2 seconds.

## Additional Notes

- If you are standing right next to a respawn of a previously nerfed creature, they might show the wrong level.  But if you run ``.npc info`` on them (or leave and come back) it will show correct.

- To control the existance or non-existence of certain PvP vendors -- such as vendors not belonging in your particular expansion -- I use MySQL commands found [here](https://github.com/whipowill/sh-azerothcore/tree/master/patches).

## Credits

Special thanks to 55Honey for his [ZoneDebuff](https://github.com/55Honey/Acore_ZoneDebuff) method.

## External Links

- [AzerothCore](https://github.com/azerothcore/azerothcore-wotlk)
- [AutoBalance](https://github.com/azerothcore/mod-AutoBalance)
- [NPCBots](https://github.com/trickerer/AzerothCore-wotlk-with-NPCBots)
- [Vanilla Naxx](https://github.com/sogladev/mod-vanilla-naxxramas)