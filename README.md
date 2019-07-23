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
- **wasd** Move teleporting cursor. After the cursor becomes shown, use these keys to move it
to some location on the screen. You cannot teleport to a cell where a Monster/Wall is present.
However, if you set your cursor over an item (**i**) and teleport, it will be added to your
inventory
