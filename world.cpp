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

#include "pico/rand.h"
#include "drivers/dv_display/dv_display.hpp"
#include "libraries/pico_graphics/pico_graphics_dv.hpp"

#include "arborescence.hpp"
#include "sprite_sun.hpp"
#include "sprite_moon.hpp"
#include "sprite_cloudl.hpp"
#include "sprite_cloudr.hpp"
#include "tree.hpp"
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
  this->mTimeOfDay = 0;
  this->mSkyFG.h = this->mSkyFG.s = this->mSkyFG.v = 0.0f;
  this->mSkyBG.h = this->mSkyBG.s = this->mSkyBG.v = 0.0f;
  this->mGroundFG.h = this->mGroundFG.s = this->mGroundFG.v = 0.0f;
  this->mGroundBG.h = this->mGroundBG.s = this->mGroundBG.v = 0.0f;

  /* Load up our sprite data; need to do it in both banks. */
  this->mDisplay->define_sprite( SPRITE_SUN, sprite_sun_width, sprite_sun_height, sprite_sun_data );
  this->mDisplay->define_sprite( SPRITE_MOON, sprite_moon_width, sprite_moon_height, sprite_moon_data );
  this->mDisplay->define_sprite( SPRITE_CLOUDL, sprite_cloudl_width, sprite_cloudl_height, sprite_cloudl_data );
  this->mDisplay->define_sprite( SPRITE_CLOUDR, sprite_cloudr_width, sprite_cloudr_height, sprite_cloudr_data );
  this->mDisplay->flip();
  this->mDisplay->define_sprite( SPRITE_SUN, sprite_sun_width, sprite_sun_height, sprite_sun_data );
  this->mDisplay->define_sprite( SPRITE_MOON, sprite_moon_width, sprite_moon_height, sprite_moon_data );
  this->mDisplay->define_sprite( SPRITE_CLOUDL, sprite_cloudl_width, sprite_cloudl_height, sprite_cloudl_data );
  this->mDisplay->define_sprite( SPRITE_CLOUDR, sprite_cloudr_width, sprite_cloudr_height, sprite_cloudr_data );

  /* Initialise our forest. */
  for ( uint_fast8_t lIndex = 0; lIndex < TREES_MAX; lIndex++ )
  {
    this->mForest[lIndex] = nullptr;
  }

  /* And any other init stuff... */
  this->mRedrawSkyFG = this->mRedrawForestFG = true;
  this->mRedrawSkyBG = this->mRedrawForestBG = true;
  this->mCloudActive = false;

  /* All done. */
  return;
}


/*
 * destructor; just need to clean up any allocations done within the World.
 */

World::~World( void )
{
  /* Free up any trees. */
  for ( uint_fast8_t lIndex = 0; lIndex < TREES_MAX; lIndex++ )
  {
    if ( this->mForest[lIndex] != nullptr )
    {
      delete this->mForest[lIndex];
      this->mForest[lIndex] = nullptr;
    }
  }

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
  l_colour.h = 0.63f - (sin(this->mTimeOfDay*3.14159f/1800.0f)/10.0f);
  l_colour.s = 0.65f;
  l_colour.v = 0.35f + (sin(this->mTimeOfDay*3.14159f/1800.0f)/5.0f);

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
  if ( ++this->mTimeOfDay > 3600 )
  {
    this->mTimeOfDay = 0;
  }

  /* Swap the current front buffer colours to the back. */
  hsv_t lTempColour;
  memcpy( &lTempColour, &this->mGroundBG, sizeof( hsv_t ) );
  memcpy( &this->mGroundBG, &this->mGroundFG, sizeof( hsv_t ) );
  memcpy( &this->mGroundFG, &lTempColour, sizeof( hsv_t ) );

  memcpy( &lTempColour, &this->mSkyBG, sizeof( hsv_t ) );
  memcpy( &this->mSkyBG, &this->mSkyFG, sizeof( hsv_t ) );
  memcpy( &this->mSkyFG, &lTempColour, sizeof( hsv_t ) );

  /* Also, bring forward the rear redraw flags. */
  this->mRedrawSkyFG = this->mRedrawSkyBG;
  this->mRedrawForestFG = this->mRedrawForestBG;
  this->mRedrawSkyBG = this->mRedrawForestBG = false;

  /* Scroll the title across the top of the screen. */
  this->mTitleOffset--;
  if ( ( this->mTitleOffset + this->mTitleLength ) < 0 )
  {
    this->mTitleOffset = SCREEN_WIDTH;
  }

  /* Update any trees we have; this is ~1 per second */
  if ( this->mTimeOfDay % 60 == 0 )
  {
    for ( uint_fast8_t lIndex = 0; lIndex < TREES_MAX; lIndex++ )
    {
      if ( this->mForest[lIndex] != nullptr )
      {
        this->mForest[lIndex]->update();
        if ( this->mForest[lIndex]->is_dead() )
        {
          delete this->mForest[lIndex];
          this->mForest[lIndex] = nullptr;
          this->mRedrawSkyFG = this->mRedrawSkyBG = true;
        }
        this->mRedrawForestFG = this->mRedrawForestBG = true;
      }
    }

    /* Occasionally spawn a new tree, if we have a free spot. */
    if ( get_rand_32() % 15 == 0 )
    {
      /* See if there's a space in the forest. */
      for ( uint_fast8_t lIndex = 0; lIndex < TREES_MAX; lIndex++ )
      {
        if ( this->mForest[lIndex] == nullptr )
        {
          /* Found one, so grow a tree and exit. */
          this->mForest[lIndex] = new Tree( this->mGraphics );
          this->mRedrawForestFG = this->mRedrawForestBG = true;
          break;
        }
      }
    }
  }

  /* Figure out where the sun should be. */
  this->mSunLocation.x = ( SCREEN_WIDTH / 2 ) - ( cos(this->mTimeOfDay*3.14159f/1800.0f) * ( ( SCREEN_WIDTH / 2 ) - 16 ) ) - 16;
  this->mSunLocation.y = GROUND_LEVEL - ( sin(this->mTimeOfDay*3.14159f/1800.0f) * GROUND_LEVEL );

  /* And the moon (which basically follows the sun) */
  this->mMoonLocation.x = ( SCREEN_WIDTH / 2 ) - ( cos(this->mTimeOfDay*3.14159f/1800.0f) * ( ( SCREEN_WIDTH / 2 ) - 16 ) * -1 ) - 16;
  this->mMoonLocation.y = GROUND_LEVEL - ( sin(this->mTimeOfDay*3.14159f/1800.0f) * GROUND_LEVEL * -1 );

  /* If the cloud is active, move it. */
  if ( this->mCloudActive )
  {
    /* Randomly drift up and down, but not too much... */
    if ( get_rand_32() % 2 == 0 )
    {
      this->mCloudLocation.y -= get_rand_32() % 2;
      if ( this->mCloudLocation.y < 0 )
      {
        this->mCloudLocation.y = 0;
      }
    }
    else
    {
      this->mCloudLocation.y += get_rand_32() % 2;
      if ( this->mCloudLocation.y > SCREEN_HEIGHT/2 )
      {
        this->mCloudLocation.y = SCREEN_HEIGHT/2;
      }
    }

    /* Drift to the right, until we drop off the end. */
    this->mCloudLocation.x += 2;
    if ( this->mCloudLocation.x > SCREEN_WIDTH )
    {
      this->mCloudActive = false;
    }
  }
  else
  {
    /* If it's not active, a lowish chance to activate it. */
    if ( get_rand_32() % 300 == 0 )
    {
      this->mCloudActive = true;
      this->mCloudLocation.x = -64;
      this->mCloudLocation.y = get_rand_32() % ( SCREEN_HEIGHT/2 );
    }
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
  float        lOffset;

  /* Handle the ground first; see what colour it should be. */
  lCurrentColour = this->ground_colour();

  /* And if the front buffer isn't using this colour, update it. */
  if ( ( this->mRedrawSkyFG ) ||
       ( lCurrentColour->h != this->mGroundFG.h ) || 
       ( lCurrentColour->s != this->mGroundFG.s ) || 
       ( lCurrentColour->v != this->mGroundFG.v ) )
  {
    /* Redraw the ground in this colour. */
    this->mGraphics->set_depth( 1 );
    lOffset = 0.0f;
    for ( uint_fast16_t lRow = GROUND_LEVEL; lRow < SCREEN_HEIGHT; lRow++ )
    {
      this->mGraphics->set_pen( 
        pimoroni::RGB::from_hsv(
          lCurrentColour->h,
          lCurrentColour->s,
          lCurrentColour->v - lOffset
        ).to_rgb555()
      );
      this->mGraphics->line( pimoroni::Point( 0, lRow ), pimoroni::Point( SCREEN_WIDTH, lRow ) );
      lOffset += 0.003f;
    }

    /* And remember that it's changed. */
    memcpy( &this->mGroundFG, lCurrentColour, sizeof( hsv_t ) );
    this->mRedrawForestFG = this->mRedrawForestBG = true;
  }

  /* Now do the same for the sky. */
  lCurrentColour = this->sky_colour();

  /* And if the front buffer isn't using this colour, update it. */
  if ( ( this->mRedrawSkyFG ) ||
       ( lCurrentColour->h != this->mSkyFG.h ) || 
       ( lCurrentColour->s != this->mSkyFG.s ) || 
       ( lCurrentColour->v != this->mSkyFG.v ) )
  {
    /* Redraw the ground in this colour. */
    this->mGraphics->set_depth( 0 );
    this->mGraphics->set_pen( 
      pimoroni::RGB::from_hsv(
        lCurrentColour->h,
        lCurrentColour->s,
        lCurrentColour->v
      ).to_rgb555()
    );
    this->mGraphics->rectangle( pimoroni::Rect( 0, 0, SCREEN_WIDTH, GROUND_LEVEL ) );

    /* And remember that it's changed. */
    memcpy( &this->mSkyFG, lCurrentColour, sizeof( hsv_t ) );
    this->mRedrawForestFG = this->mRedrawForestBG = true;
    this->mRedrawSkyFG = false;
  }

  /*
   * Now the title bar, which runs along the top of the screen - first we
   * need to blank what's there.
   */
  this->mGraphics->set_pen( 
    pimoroni::RGB::from_hsv( this->mSkyFG.h, this->mSkyFG.s, this->mSkyFG.v ).to_rgb555()
  );
  this->mGraphics->set_depth( 0 );
  this->mGraphics->rectangle(
    pimoroni::Rect( this->mTitleOffset-1, 1, this->mTitleLength+2, 16 )
  );

  /* And then draw the text. */
  this->mGraphics->set_pen( this->mWhitePen );
  this->mGraphics->set_depth( 1 );
  this->mGraphics->text( 
    this->mTitleText, 
    pimoroni::Point( this->mTitleOffset, 1 ),
    SCREEN_WIDTH
  );

  /* Trees, can be re-drawn in situ if we need to. */
  if ( this->mRedrawForestFG )
  {
    this->mGraphics->set_depth( 1 );
    for ( uint_fast8_t lIndex = 0; lIndex < TREES_MAX; lIndex++ )
    {
      if ( this->mForest[lIndex] != nullptr )
      {
        this->mForest[lIndex]->render( this->mTimeOfDay );
      }
    }
    this->mRedrawForestFG = false;
  }

  /* And put the sun and moon where it should be. */
  this->mDisplay->set_sprite( SPRITE_SUN, SPRITE_SUN, this->mSunLocation );  
  this->mDisplay->set_sprite( SPRITE_MOON, SPRITE_MOON, this->mMoonLocation );

  /* And the clouds, if it's active. */
  if ( this->mCloudActive )
  {
    this->mDisplay->set_sprite( SPRITE_CLOUDL, SPRITE_CLOUDL, this->mCloudLocation );
    this->mDisplay->set_sprite(
      SPRITE_CLOUDR, SPRITE_CLOUDR, 
      this->mCloudLocation + pimoroni::Point( 32, 0 )
    );
  }

  /* All done. */
  return;
}


/* End of file world.cpp */
