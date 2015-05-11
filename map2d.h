#ifndef MAP2D_H
#define MAP2D_H

#include <stdint.h>

typedef struct {
    uint32_t x;
    uint32_t y;
} point_t;

typedef struct map2d_t map2d_t;

map2d_t * map2d_new(int * walls, uint32_t width, uint32_t height);

void map2d_free(map2d_t *);

point_t ** map2d_find_path(map2d_t * this, point_t start, point_t end,
        uint32_t * path_size, double * path_distance);

uint32_t map2d_get_width(map2d_t * this);

uint32_t map2d_get_height(map2d_t * this);

int map2d_is_wall(map2d_t * this, uint32_t x, uint32_t y);

#endif
