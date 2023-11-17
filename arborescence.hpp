/*
 * arborescence.hpp - part of Arborescence
 *
 * This header file declares all our constants and structures; this is
 * probably the place to look for any configuration!
 *
 * Copyright (c) 2023 Pete Favelle <ahnlak@ahnlak.com>
 * This file is licensed under the BSD 3-Clause License; see LICENSE for details.
 */

#pragma once

/* Constants and enums. */

#define SCREEN_WIDTH  720
#define SCREEN_HEIGHT 480
#define GROUND_HEIGHT 32
#define GROUND_LEVEL  (SCREEN_HEIGHT - GROUND_HEIGHT - GROUND_HEIGHT)
#define BRANCHES_MAX  3
#define TREES_MAX     8

#define AGE_GROWTH    20
#define AGE_DEATH     80

#define SPRITE_SUN    0
#define SPRITE_MOON   1
#define SPRITE_CLOUDL 2
#define SPRITE_CLOUDR 3


/* Structures. */

typedef struct
{
  float h;
  float s;
  float v;
} hsv_t;


/* Function prototypes. */


/* End of file arborescence.hpp */
