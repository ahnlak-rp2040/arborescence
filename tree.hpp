/*
 * tree.hpp - part of Arborescence
 *
 * This header declares the Tree class, along with it's supporting Branch struct.
 *
 * Copyright (c) 2023 Pete Favelle <ahnlak@ahnlak.com>
 * This file is licensed under the BSD 3-Clause License; see LICENSE for details.
 */

#pragma once

#include "libraries/pico_graphics/pico_graphics_dv.hpp"

#include "arborescence.hpp"


/* Structures. */

typedef struct branch_t branch_t;
struct branch_t
{
  pimoroni::Point end_point;
  branch_t       *branches[BRANCHES_MAX];
};

/* Class declaration. */

class Tree
{
private:
  pimoroni::PicoGraphics_PenDV_RGB555  *mGraphics;
  pimoroni::Point                       mOrigin;
  branch_t                              mTrunk;
  uint_fast8_t                          mHeight;
  uint_fast8_t                          mAge;

  branch_t       *alloc_branch( pimoroni::Point, uint_fast8_t );
  void            free_branch( branch_t * );
  void            grow_branch( branch_t *, uint_fast8_t );
  void            render_branch( const branch_t *, const pimoroni::Point *,
                                 uint_fast16_t, uint_fast8_t );

public:
                  Tree( pimoroni::PicoGraphics_PenDV_RGB555 * );
                 ~Tree( void );

  void            update( void );
  void            render( uint_fast16_t );
  bool            is_dead( void );

};

/* End of file tree.hpp */
