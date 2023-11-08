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


/* Class declaration. */

class World
{
private:
  pimoroni::DVDisplay                  *mDisplay;
  pimoroni::PicoGraphics_PenDV_RGB555  *mGraphics;
  pimoroni::Pen                         mBlackPen;
  pimoroni::Pen                         mWhitePen;

  int16_t       mTitleLength;
  int16_t       mTitleOffset;
  const char   *mTitleText = "ARBORESCENCE";

public:
        World( pimoroni::DVDisplay *, pimoroni::PicoGraphics_PenDV_RGB555 * );
       ~World( void );

  void  update( void );
  void  render( void );
};

/* End of file world.hpp */
