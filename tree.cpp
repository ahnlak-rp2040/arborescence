/*
 * tree.cpp - part of Arborescence
 *
 * Implements the Tree class; the Tree knows how to grow, and how to render
 * itself.
 *
 * Copyright (c) 2023 Pete Favelle <ahnlak@ahnlak.com>
 * This file is licensed under the BSD 3-Clause License; see LICENSE for details.
 */

/* System header files. */

#include <stdlib.h>
#include <time.h>


/* Local header files. */

#include "pico/rand.h"
#include "libraries/pico_graphics/pico_graphics_dv.hpp"

#include "arborescence.hpp"
#include "tree.hpp"


/* Functions. */


/*
 * constructor; generates a random origin, and the initial (single) branch.
 */

Tree::Tree( pimoroni::PicoGraphics_PenDV_RGB555 *pGraphics )
{
  /* Save the references we're given. */
  this->mGraphics = pGraphics;

  /* So, the tree always originates on the ground, obviously. */
  this->mOrigin.y = SCREEN_HEIGHT - ( GROUND_HEIGHT / 2 ) - ( get_rand_32() % GROUND_HEIGHT );
  this->mOrigin.x = 1 + ( get_rand_32() % (SCREEN_WIDTH-2) );

  /* The trunk should be pretty much vertical. */
  this->mTrunk.end_point.x = this->mOrigin.x;
  this->mTrunk.end_point.y = this->mOrigin.y - (SCREEN_HEIGHT/6) - ( get_rand_32() %(SCREEN_HEIGHT/8) );

  /* And for now, without any branches. */
  for ( uint_fast8_t lIndex = 0; lIndex < BRANCHES_MAX; lIndex++ )
  {
    this->mTrunk.branches[lIndex] = nullptr;
  }
  this->mHeight = 1;
  this->mAge = 1;

  /* All done. */
  return;
}


/*
 * destructor; tidy up memory allocated to any branches.
 */

Tree::~Tree( void )
{
  free_branch( &this->mTrunk );
  return;
}

void Tree::free_branch( branch_t *pBranch )
{
  /* For any branches, free their content and then them. */
  for ( uint_fast8_t lIndex = 0; lIndex < BRANCHES_MAX; lIndex++ )
  {
    if ( pBranch->branches[lIndex] != nullptr )
    {
      free_branch( pBranch->branches[lIndex] );
      free( pBranch->branches[lIndex] );
      pBranch->branches[lIndex] = nullptr;
    }
  }  
}


/*
 * update; updates the age (and growth) of the tree. Called every second or so,
 *         so it's up to us to decide growth rates.
 */

void Tree::update( void )
{
  /* Firstly, keep track of our age... */
  this->mAge++;

  /* Deal with ageing and death. */

  /* And growth, if it seems to be the right time. */
  if ( (this->mAge < AGE_GROWTH ) && ( this->mAge % 4 == 0 ) )
  {
    /* Work through all branches; if any don't have sub-branches, grow there. */
    this->grow_branch( &this->mTrunk, 1 );
  }

  return;
}


/*
 * grow_branch; either adds sub-branches to a virgin branch, or recurses into
 *              the sub-branches that are already there.
 */

void Tree::grow_branch( branch_t *pBranch, uint_fast8_t pHeight )
{
  bool lFoundBranch = false;

  /* Work through any branches we have. */
  for ( uint_fast8_t lIndex = 0; lIndex < BRANCHES_MAX; lIndex++ )
  {
    if ( pBranch->branches[lIndex] != nullptr )
    {
      /* Remember we found it. */
      lFoundBranch = true;

      /* Then grow it. */
      this->grow_branch( pBranch->branches[lIndex], pHeight+1 );
    }
  }

  /* So, if none were found, we grow some! */
  if ( !lFoundBranch )
  {
    pBranch->branches[0] = alloc_branch( pBranch->end_point, pHeight );
    pBranch->branches[0]->end_point.x -= ( 60 / pHeight );
    pBranch->branches[1] = alloc_branch( pBranch->end_point, pHeight );
    pBranch->branches[1]->end_point.x += ( 30 / pHeight );
//    if ( get_rand_32()%2 == 0 )
//    {
//      pBranch->branches[2] = alloc_branch( pBranch->end_point, pHeight );
//    }

    if ( pHeight > this->mHeight )
    {
      this->mHeight = pHeight;
    }
  }

  /* All done. */
  return;
}


/*
 * alloc_branch; creates a new branch, based on the provided origin and
 *               current tree height.
 */

branch_t *Tree::alloc_branch( pimoroni::Point pOrigin, uint_fast8_t pHeight )
{
  branch_t *lNewBranch = (branch_t *)malloc( sizeof( branch_t ) );

  /* So, make sure the sub branches are properly nulled. */
  for ( uint_fast8_t lIndex = 0; lIndex < BRANCHES_MAX; lIndex++ )
  {
    lNewBranch->branches[lIndex] = nullptr;
  }

  /* Set the end-point to something ... suitable. */
  lNewBranch->end_point.x = pOrigin.x + get_rand_32() % ( 60 / pHeight );
  lNewBranch->end_point.y = pOrigin.y - ( SCREEN_HEIGHT / 16 ) / pHeight - 
                            get_rand_32() % ( ( SCREEN_HEIGHT / 4 ) / pHeight );

  /* And return the new branch. */
  return lNewBranch;
}


/*
 * render; draws the tree onto the current buffer. As we're only ever drawing
 *         over previous growth, we don't need to clear anything. This is only
 *         called when we're sure something needs drawing.
 */

void Tree::render( uint_fast16_t pTimeOfDay )
{
  /* Fairly simple this; we just draw lines until we run out... */
  this->render_branch( &this->mTrunk, &this->mOrigin, pTimeOfDay, 1 );

  /* All done. */
  return;
}

/*
 * render_branch; draws an individual branch; called recursively.
 */

void Tree::render_branch( const branch_t *pBranch, const pimoroni::Point *pOrigin,
                          uint_fast16_t pTimeOfDay, uint_fast8_t pHeight )
{
  /* Fairly simple then - draw a line from the origin to the endpoint. */
  this->mGraphics->set_pen( 92, 64, 51 );

  /* The thickness of the branch depends on the height. */
  if ( ( this->mHeight < 2 ) || ( pHeight > ( this->mHeight - 1 ) ) )
  {
    this->mGraphics->line( *pOrigin, pBranch->end_point );
  }
  else
  {
    this->mGraphics->thick_line( *pOrigin, pBranch->end_point, ( this->mHeight - pHeight ) * 2 );
  }

  /* If we're not at the bottom of the tree, add some leaves. */
  if ( pHeight >= 2 )
  {
    this->mGraphics->set_pen( 68, 95+(pHeight*3)+(sin(pTimeOfDay*3.14159f/1800.0f)*20), 21 );
    this->mGraphics->circle( pBranch->end_point, 20 - (pHeight*3) );
  }

  /* And then recurse on ourselves for any deeper branches. */
  for ( uint_fast8_t lIndex = 0; lIndex < BRANCHES_MAX; lIndex++ )
  {
    if ( pBranch->branches[lIndex] != nullptr )
    {
      this->render_branch( pBranch->branches[lIndex], &pBranch->end_point, pTimeOfDay, pHeight+1 );
    }
  }

  /* All done. */
  return;
}


/*
 * is_dead; simple test do decide if the current tree is still alive.
 */

bool Tree::is_dead( void )
{
  return this->mAge > AGE_DEATH;
}

/* End of file tree.cpp */
