# modified version

## zumi's changes

added `meson.build`. pulled in the stuff from furnace into this dir, because meson.

## tildearrow's changes
this is a modified version of Dear ImGui (docking branch) to suit Furnace.
the following changes have been made:

- fix UI scaling on macOS, Wayland and any other platform where HiDPI is implemented through logical pixels
- gradients on frames
- improved touch support (inertial scrolling in particular)
- disable text input undo/redo by default
- add ability to lock dockspace
