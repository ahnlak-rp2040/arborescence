/*
 * main.cpp - part of Arborescence
 *
 * This is the entry point and main logic loop of the demo.
 *
 * Copyright (c) 2023 Pete Favelle <ahnlak@ahnlak.com>
 * This file is licensed under the BSD 3-Clause License; see LICENSE for details.
 */

/* System header files. */

#include <stdlib.h>
#include <time.h>


/* Local header files. */

#include "pico/rand.h"
#include "pico/stdlib.h"
#include "drivers/dv_display/dv_display.hpp"
#include "libraries/pico_graphics/pico_graphics_dv.hpp"

#include "arborescence.hpp"
#include "world.hpp"


/* Functions. */


/*
 * main - the entry point to the program; this initialises the display,
 *        and provides the main render / update logic.
 */

int main()
{
  pimoroni::DVDisplay                  *lDisplay;
  pimoroni::PicoGraphics_PenDV_RGB555  *lGraphics;
  World                                *lWorld;

  /* Normal Pico initialisation. */
  stdio_init_all();

  /* Create the display, and the graphics driver. */
  lDisplay = new pimoroni::DVDisplay();
  lGraphics = new pimoroni::PicoGraphics_PenDV_RGB555( SCREEN_WIDTH, SCREEN_HEIGHT, *lDisplay );

  /* Now initialise the display. */
  lDisplay->preinit();
  lDisplay->init( SCREEN_WIDTH, SCREEN_HEIGHT, pimoroni::DVDisplay::MODE_RGB555 );

  /* Initialise the random number generator (which ... will only be a bit random) */
  srand( get_rand_32() );

  /* And finally, we need a World to handle everything. */
  lWorld = new World( lDisplay, lGraphics );

  /* And enter into the display loop, forever! */
  while(true)
  {
    /* We render first. */
    lWorld->render();

    /* Flip the display asynchronously. */
    lDisplay->flip_async();

    /* And we can update in parallel with that work. */
    lWorld->update();

    /* Last thing, wait for the flip to complete and keep us sync'd to VSYNC */
    lDisplay->wait_for_flip();
  }
}

/* End of file main.cpp */
