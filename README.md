# rougelike

A fairly basic rougelike game written using C++ and termbox. You are the '@' and explore the map in order
to pick up items ('i') and kill monsters ('M') by running into them repeatedly. You can also teleport to anywhere
visible and even teleport items into your inventory!

**Note:** There is currently no support for platforms other than macOS. This should expand in the future to include
all Unix-style terminal emulators supported by ![termbox](https://github.com/nsf/termbox).

## Building

### Requirements

- macOS (tested on 10.14.5, but should work on 10.13 or earlier)
- clang Apple LLVM version 10.0.1 (clang-1001.0.46.4)
- Python >= 2.5 (any Python 3 version will also work) (runs waf build script)

### Builds

There are three different build scripts:

- `build-full.sh` (run this right after cloning or after updating termbox submodule)
- `build.sh` (incremental build; run this for to recompile after any changes made to source)
- `run-tests.sh` (builds/runs test suite; cleans up after itself)

After cloning, run `build-full.sh` to build the included termbox code and the game's code. 
After running `build-full.sh` once, run `build.sh`, which will compile/link only the game's code 
from scratch, staticly linking to `src/libtermbox.a` instead of recompiling termbox. This makes
compile times shorter, since termbox's code never changes when modifying game code.

`run-tests.sh` builds/runs a modified build centered around `test-suite.cpp`, which automatically runs several functionality tests. Note that this script won't produce a new executable.

## Playing

run `./rpg` or optionally click on `rpg2` from the Finder.

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
teleport, it will be added to your inventory. Teleporting to a Monster's position causes you to attack the monster.
- **arrow keys** Move teleportation/ranged attack cursor once it is shown
- **Ctrl-x** or **Ctrl-c** Exit game

## Features

- A movable/teleportable player character ('@')
- RNG melee/ranged combat
- Viewable player inventory/character sheet
- An event log
- Equippable items
- Monsters ('M')
- Items ('i'), which can be picked up
- Walls ('1')
- Dynamic screen resizing/camera tracking
- Gorgeous ASCII graphics

## Future

I hope to add a larger variety of monsters, items, and combat to the game. Additionally, I aim to
implement a procedural map generator and more levels (right now there is just 1 test map, which is pretty boring),
as well as some sort of goal as the game's end condition.
