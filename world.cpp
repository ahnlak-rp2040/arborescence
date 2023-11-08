/*
 * world.cpp - part of Arborescence
 *
 * Implements the World class, which wraps up the whole world view and provides
 * methods to render and update it.
 *
 * Copyright (c) 2023 Pete Favelle <ahnlak@ahnlak.com>
 * This file is licensed under the BSD 3-Clause License; see LICENSE for details.
 */

/* System header files. */


/* Local header files. */

#include "drivers/dv_display/dv_display.hpp"
#include "libraries/pico_graphics/pico_graphics_dv.hpp"

#include "arborescence.hpp"
#include "world.hpp"


/* Functions. */


/*
 * constructor; provided with the display and graphics objects, which we will
 *              use to render the world.
 */

World::World( pimoroni::DVDisplay *pDisplay, pimoroni::PicoGraphics_PenDV_RGB555 *pGraphics )
{
  /* Simply save the references we're given. */
  this->mDisplay = pDisplay;
  this->mGraphics = pGraphics;

  /* Set the default font. */
  this->mGraphics->set_font( "bitmap8" );

  /* And create some commonly used pens. */
  this->mBlackPen = this->mGraphics->create_pen(0, 0, 0);
  this->mWhitePen = this->mGraphics->create_pen(255, 255, 255);

  /* Work out the size of the title text. */
  this->mTitleLength = this->mGraphics->measure_text( this->mTitleText );

  /* And position it slightly off screen to start with. */
  this->mTitleOffset = (SCREEN_WIDTH-this->mTitleLength) / 2;

  /* All done. */
  return;
}


/*
 * destructor; just need to clean up any allocations done within the World.
 */

World::~World( void )
{

  /* All done. */
  return;
}


/*
 * update; called each frame, to update the state of the world. No changes 
 *         should be sent to the display here, as it will be called asynchronously
 *         with a frame update.
 */

void World::update( void )
{

  /* All done. */
  return;
}


/*
 * render; called each frame to render the current state of the world. As we're
 *         double buffered, we are always drawing on the *previous* frame
 *         content - ideally we don't want to waste cycles blanking the whole
 *         frame every time if we can think our way around it.
 */

void World::render( void )
{
  this->mGraphics->set_pen( this->mBlackPen );
  this->mGraphics->clear();

  /* First off the title bar, which runs along the top of the screen. */
  this->mGraphics->set_pen( this->mWhitePen );
  this->mGraphics->text( 
    this->mTitleText, 
    pimoroni::Point( this->mTitleOffset, 1 ),
    SCREEN_WIDTH
  );

  /* All done. */
  return;
}


/* End of file world.cpp */
