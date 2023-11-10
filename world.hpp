/*
 * world.hpp - part of Arborescence
 *
 * This header file declares the World class; this wraps up the world view,
 * and also knows how to render it.
 *
 * Copyright (c) 2023 Pete Favelle <ahnlak@ahnlak.com>
 * This file is licensed under the BSD 3-Clause License; see LICENSE for details.
 */

#pragma once

#include "drivers/dv_display/dv_display.hpp"
#include "libraries/pico_graphics/pico_graphics_dv.hpp"

#include "arborescence.hpp"


/* Class declaration. */

class World
{
private:
  pimoroni::DVDisplay                  *mDisplay;
  pimoroni::PicoGraphics_PenDV_RGB555  *mGraphics;
  pimoroni::Pen                         mBlackPen;
  pimoroni::Pen                         mWhitePen;

  int_fast16_t  mTitleLength;
  int_fast16_t  mTitleOffset;
  const char   *mTitleText = "ARBORESCENCE";

  uint_fast16_t mDayOfYear;

  hsv_t         mGroundFG, mGroundBG;
  hsv_t         mSkyFG, mSkyBG;

  const hsv_t  *ground_colour( void );
  const hsv_t  *sky_colour( void );

public:
                World( pimoroni::DVDisplay *, pimoroni::PicoGraphics_PenDV_RGB555 * );
               ~World( void );

  void          update( void );
  void          render( void );
};

/* End of file world.hpp */
