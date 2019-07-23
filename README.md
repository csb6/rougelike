# rougelike

A fairly basic rougelike game written using C++ and termbox. You are the '@' and explore the map in order
to pick up items ('i') and kill monsters ('M') by running into them repeatedly. You can also teleport to anywhere
visible and even teleport items into your inventory!

**Note:** There is currently no support for platforms other than macOS. This should expand in the future to include
all Unix-style terminal emulators supported by ![termbox](https://github.com/nsf/termbox).

## Controls

- **arrow keys** Movement
- **i** View inventory
- **c** View character sheet (mostly empty for now)
- **ESC** Redraw screen. Use this to close inventory/character sheet/hide teleportation cursor
- **t** Teleport the player. Pressing **t** will show a cursor on the player's position. After
moving the cursor to the desired location (see below), press **t** again to teleport there
- **wasd** Move teleportation cursor. After the cursor becomes shown, use these keys to move it
to some location on the screen. You cannot teleport to a cell where a Monster/Wall is present.
However, if you set your cursor over an item (**i**) and teleport, it will be added to your
inventory

## Features

- A movable/teleportable player character ('@')
- Viewable player inventory/character sheet
- An event log
- Monsters ('M')
- Items ('i'), which can be picked up
- Walls ('1')
- Dynamic screen resizing/camera tracking
- Gorgeous ASCII graphics

## Future

I hope to add a larger variety of monsters, items, and combat to the game. Additionally, I aim to
implement a procedural map generator and more levels (right now there is just 1 test mpa, which is pretty boring),
as well as some sort of goal as the game's end condition.
