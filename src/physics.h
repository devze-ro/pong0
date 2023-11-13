#ifndef PHYSICS_H

#include "defines.h"
#include "renderer.h"

internal QuadMinMax get_quad_min_max(Quad *quad);

internal b32 is_point_inside_quad(v2 *point, Quad *quad);

internal Quad minkowski_sum(Quad *a, Quad *b);

internal v2 get_abs_pos_from_rel_pos(f32 width, f32 height, v2 ref,
        v2 direction);

#define PHYSICS_H
#endif

