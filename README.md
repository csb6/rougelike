# rougelike

A fairly basic rougelike game written using C++ and termbox. You are the '@' and explore the map in order
to pick up items and kill monsters by running into them repeatedly. You can also teleport to anywhere
visible and even teleport items into your inventory!

**Note:** There is currently no support for platforms other than macOS. This should expand in the future to include
all terminal emulators supported by ![termbox](https://github.com/nsf/termbox), regardless of platform.

## Building

### Requirements

- macOS (tested on 10.14.5, but should work on 10.13 and earlier)
- C++11 compiler (tested on clang Apple LLVM version 10.0.1 (clang-1001.0.46.4), but any other C++11 compiler should work)
- Python >= 2.5 (any Python 3 version will also work) (runs waf build script)
- *Optional:* Ruby (any version; tested on 2.5.1) (for running linter tool)
- *Optional:* clang-tidy (LLVM linting tool)

### Scripts

There are three different build scripts:

- `./build-full.sh` (run this right after cloning or after updating termbox submodule; subsequent builds use `./build.sh`)
- `./build.sh` (incremental build; run this to recompile after any changes made to game code)
- `./run-tests.sh` (builds/runs test suite; cleans up after itself)

`./run-tests.sh` builds/runs a modified build centered around `test-suite.cpp`, which automatically runs several functionality tests. Note that this script won't produce a new executable.

## Playing

run `./rpg2` or optionally double-click `rpg2` from the Finder.

## Controls

- **arrow keys** Movement; running directly into monsters will melee attack them, with damage to you and
the monster dependent on a skill check
- **i** View inventory
- **@** View character sheet (mostly empty for now)
- **e** View equipped item slots (also mostly empty)
- **E** Equip an item (enter its inventory number, then its equipment slot number); any item
  can be equipped as armor, even a knife!
- **D** Deequip an item (enter its equipment slot number, e.g. Head is slot 1, Ranged Weapon is slot 6, etc.)
- **ESC** Redraw screen. Use this to close inventory/character sheet/hide teleportation cursor
- **r** Range attack a monster. Pressing **r** will show a cursor on the player's position. After
moving the cursor to the monster you want to attack, press **r** again to attack it. Only works if
a ranged weapon is equipped in the proper slot (if non-ranged item equipped in ranged weapon slot,
it will be thrown at the monster). Also, you can't attack items/walls.
- **t** Teleport the player. Pressing **t** will show a cursor on the player's position. After
moving the cursor to the desired location (see below), press **t** again to teleport there. You cannot
teleport to a cell where a Wall is present. However, if you set your cursor over an item (**i**) and
teleport, it will be added to your inventory. Teleporting to a Monster's position causes you to melee the monster
without moving to its position.
- **arrow keys** Move teleportation/ranged attack cursor once it is shown
- **Ctrl-x** or **Ctrl-c** Exit game

## Features

- Custom character creation or quickstart
- A movable/teleportable player character ('@')
- RNG melee/ranged combat
- Viewable player inventory/character sheet
- An event log
- Equippable items
- Several monster types including Mutant Bears, Marxist Marmots, and Red Imps
- Monsters attempt to find/kill the player
- Items ('i'), which can be picked up
- Walls ('#')
- Dynamic screen resizing/camera tracking
- Gorgeous ASCII graphics

## Future

I hope to add a larger variety of monsters, items, and combat to the game. Additionally, I aim to
implement a procedural map generator and more levels (right now there is just 1 test map, which is pretty boring),
as well as some sort of goal as the game's end condition.
