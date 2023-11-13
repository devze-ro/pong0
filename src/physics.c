#include "defines.h"
#include "math.h"
#include "pong0.h"
#include "renderer.h"

internal QuadMinMax get_quad_min_max(Quad *quad)
{
    QuadMinMax result = {0};
    result.min = (v2){
        quad->pos.x - quad->half_dim.x,
            quad->pos.y - quad->half_dim.y };
    result.max = (v2){
        quad->pos.x + quad->half_dim.x,
            quad->pos.y+ quad->half_dim.y };
    return result;
}

internal b32 is_point_inside_quad(v2 *point, Quad *quad)
{
    QuadMinMax min_max = get_quad_min_max(quad);
    return (point->x >= min_max.min.x &&
            point->x <= min_max.max.x &&
            point->y >= min_max.min.y &&
            point->y <= min_max.max.y);
}

internal Quad minkowski_sum(Quad *a, Quad *b)
{
    Quad result = {0};
    result.pos = (v2){ a->pos.x, a->pos.y };
    result.half_dim = (v2){ a->half_dim.x + b->half_dim.x,
        a->half_dim.y + b->half_dim.y };
    return result;
}

internal v2 get_abs_pos_from_rel_pos(f32 width, f32 height, v2 ref, v2 direction)
{
    v2 pos = (v2){ ref.x + (direction.x * width), ref.y + (direction.y * height) };
    return pos;
}

