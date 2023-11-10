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

  /* Initialise our colours and dates to something basic. */
  this->mDayOfYear = 0;
  this->mSkyFG.h = this->mSkyFG.s = this->mSkyFG.v = 0.0f;
  this->mSkyBG.h = this->mSkyBG.s = this->mSkyBG.v = 0.0f;
  this->mGroundFG.h = this->mGroundFG.s = this->mGroundFG.v = 0.0f;
  this->mGroundBG.h = this->mGroundBG.s = this->mGroundBG.v = 0.0f;

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
 * ground_colour; returns an HSV value for the current ground colour, based
 *                on the current day of the year.
 */

const hsv_t *World::ground_colour( void )
{
  static hsv_t  l_colour;

  /* Work out the right kind of colour, based on the time of year. */
  l_colour.h = 0.38f;
  l_colour.s = 1.00f;
  l_colour.v = 0.45f;

  /* Return a pointer to our static colour. */
  return &l_colour;
}


/*
 * sky_colour; returns an HSV value for the current sky colour, based on the
 *             current day of the year.
 */

const hsv_t *World::sky_colour( void )
{
  static hsv_t  l_colour;

  /* Work out the right kind of colour, based on the time of year. */
  l_colour.h = 0.63f;
  l_colour.s = 0.65f;
  l_colour.v = 0.35f;

  /* Return a pointer to our static colour. */
  return &l_colour;
}


/*
 * update; called each frame, to update the state of the world. No changes 
 *         should be sent to the display here, as it will be called asynchronously
 *         with a frame update.
 */

void World::update( void )
{
  /* Every frame, move time forward a day... */
  if ( ++this->mDayOfYear > 365 )
  {
    this->mDayOfYear = 1;
  }

  /* Swap the current front buffer colours to the back. */
  hsv_t lTempColour;
  memcpy( &lTempColour, &this->mGroundBG, sizeof( hsv_t ) );
  memcpy( &this->mGroundBG, &this->mGroundFG, sizeof( hsv_t ) );
  memcpy( &this->mGroundFG, &lTempColour, sizeof( hsv_t ) );

  memcpy( &lTempColour, &this->mSkyBG, sizeof( hsv_t ) );
  memcpy( &this->mSkyBG, &this->mSkyFG, sizeof( hsv_t ) );
  memcpy( &this->mSkyFG, &lTempColour, sizeof( hsv_t ) );

  /* Scroll the title across the top of the screen. */
  this->mTitleOffset--;
  if ( ( this->mTitleOffset + this->mTitleLength ) < 0 )
  {
    this->mTitleOffset = SCREEN_WIDTH;
  }

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
  const hsv_t *lCurrentColour;

  /* Handle the ground first; see what colour it should be. */
  lCurrentColour = this->ground_colour();

  /* And if the front buffer isn't using this colour, update it. */
  if ( ( lCurrentColour->h != this->mGroundFG.h ) || 
       ( lCurrentColour->s != this->mGroundFG.s ) || 
       ( lCurrentColour->v != this->mGroundFG.v ) )
  {
    /* Redraw the ground in this colour. */
    this->mGraphics->set_pen( 
      pimoroni::RGB::from_hsv(
        lCurrentColour->h,
        lCurrentColour->s,
        lCurrentColour->v
      ).to_rgb555()
    );
    this->mGraphics->rectangle(
      pimoroni::Rect( 0, SCREEN_HEIGHT-GROUND_HEIGHT, SCREEN_WIDTH, GROUND_HEIGHT )
    );

    /* And remember that it's changed. */
    memcpy( &this->mGroundFG, lCurrentColour, sizeof( hsv_t ) );
  }

  /* Now do the same for the sky. */
  lCurrentColour = this->sky_colour();

  /* And if the front buffer isn't using this colour, update it. */
  if ( ( lCurrentColour->h != this->mSkyFG.h ) || 
       ( lCurrentColour->s != this->mSkyFG.s ) || 
       ( lCurrentColour->v != this->mSkyFG.v ) )
  {
    /* Redraw the ground in this colour. */
    this->mGraphics->set_pen( 
      pimoroni::RGB::from_hsv(
        lCurrentColour->h,
        lCurrentColour->s,
        lCurrentColour->v
      ).to_rgb555()
    );
    this->mGraphics->rectangle(
      pimoroni::Rect( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT-GROUND_HEIGHT )
    );

    /* And remember that it's changed. */
    memcpy( &this->mSkyFG, lCurrentColour, sizeof( hsv_t ) );
  }

  /* Now the title bar, which runs along the top of the screen. */

  /* First we need to blank what's there. */
  this->mGraphics->set_pen( 
    pimoroni::RGB::from_hsv( this->mSkyFG.h, this->mSkyFG.s, this->mSkyFG.v ).to_rgb555()
  );
  this->mGraphics->rectangle(
    pimoroni::Rect( this->mTitleOffset-1, 1, this->mTitleLength+2, 16 )
  );

  /* And then draw the text. */
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
