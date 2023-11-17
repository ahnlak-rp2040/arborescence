# Arborescence

Another demo for the PicoVision; this time exploring the C(++) API instead of
MicroPython.

This simply renders some randomly generated line trees, with a day/night cycle
as a background.

The moon and sun use hardware sprites; leaves brighten during the day to make
them stand out a bit better at midday. Stars come out at night.

Rendering is kept minimal - the trees are only redrawn when they grow, or when
the background has changed - because the PicoVision really doesn't like drawing
a lot of (non-horizontal) lines.

The graphics are all mine, and are ... terrible. Sorry. Apart from the bird,
that I stole from @Gadgetoid's "floppy birb" example.

The sprites are compiled into `cpp`/`hpp` files with the `pv_image.py` script,
which is cannibalised from a script doing a similar job for the PicoSystem.

This project follows the Boilerplate lead, and is released under the BSD 3-Clause
license - see LICENSE for details.
