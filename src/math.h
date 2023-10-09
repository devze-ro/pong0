#ifndef MATH_H

#include "defines.h"

typedef struct v2
{
    f32 x;
    f32 y;
} v2;

typedef struct v2u
{
    u32 x;
    u32 y;
} v2u;

typedef struct v3
{
    f32 x;
    f32 y;
    f32 z;
} v3;

typedef struct v3u
{
    u32 x;
    u32 y;
    u32 z;
} v3u;

typedef struct v4
{
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} v4;

#define MATH_H
#endif

