#include "map2d.h"

#include <stdlib.h>
#include <math.h>

#include "pathfinding.h"

typedef struct {
    map2d_t * map2d;
    point_t * point;
} map2d_point_t;

typedef struct map2d_t {
    uint32_t width;
    uint32_t height;
    point_t ** point_set;
    map2d_point_t ** map2d_point_set;
    int * walls;
} map2d_t;

static point_t * point_new(uint32_t x, uint32_t y);

static void point_free(point_t * this);

static int point_eq(point_t * p1, point_t * p2);

static map2d_point_t * map2d_point_new(map2d_t * map2d, point_t * point);

static void map2d_point_free(map2d_point_t * this);

static uint32_t map2d_hash_point(map2d_t * this, point_t * point);

static uint32_t map2d_hash_xy(map2d_t * this, uint32_t x, uint32_t y);

static point_t * map2d_build_point(map2d_t * this, uint32_t x, uint32_t y);

static map2d_point_t ** map2d_build_point_neighborhood(map2d_t * this, point_t * p,
        uint32_t * neighborhood_size);

static double map2d_find_path_distance(void * v1, void * v2);

static void ** map2d_find_path_neighborhood(void * vposition, uint32_t * nb_neighbors);

static int map2d_find_path_point_eq(void * v1, void * v2);

point_t * point_new(uint32_t x, uint32_t y) {
    point_t * this = (point_t *) malloc(sizeof(point_t));
    if (this == NULL) {
        return NULL;
    }
    this->x = x;
    this->y = y;
    return this;
}

void point_free(point_t * this) {
    free(this);
}

int point_eq(point_t * p1, point_t * p2) {
    return p1->x == p2->x && p1->y == p2->y ? 1 : 0;
}

map2d_point_t * map2d_point_new(map2d_t * map2d, point_t * point) {
    map2d_point_t * this = (map2d_point_t *) malloc(sizeof(map2d_point_t));
    if (this == NULL) {
        return NULL;
    }
    this->map2d = map2d;
    this->point = point;
    return this;
}

void map2d_point_free(map2d_point_t * this) {
    free(this);
}

map2d_t * map2d_new(int * walls, uint32_t width, uint32_t height) {
    map2d_t * this = (map2d_t *) malloc(sizeof(map2d_t));
    if (this == NULL) {
        return NULL;
    }

    this->point_set = calloc(width * height, sizeof(point_t *));
    if (this->point_set == NULL) {
        map2d_free(this);
        return NULL;
    }
    this->map2d_point_set = calloc(width * height, sizeof(map2d_point_t *));
    if (this->map2d_point_set == NULL) {
        map2d_free(this);
        return NULL;
    }
    this->walls = walls;
    this->width = width;
    this->height = height;
    return this;
}

void map2d_free(map2d_t * this) {
    if (this->point_set != NULL) {
        uint32_t i;
        for (i = 0; i < this->width * this->height; i++) {
            point_free(this->point_set[i]);
        }
        free(this->point_set);
    }
    this->point_set = NULL;

    if (this->map2d_point_set != NULL) {
        uint32_t i;
        for (i = 0; i < this->width * this->height; i++) {
            map2d_point_free(this->map2d_point_set[i]);
        }
        free(this->map2d_point_set);
    }
    this->map2d_point_set = NULL;

    free(this);
}

double map2d_distance(point_t p1, point_t p2) {
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

point_t ** map2d_find_path(map2d_t * this, point_t start, point_t end,
        uint32_t * path_size, double * path_distance) {
    map2d_point_t map2d_point_start = {map2d: this, point: &start};
    map2d_point_t map2d_point_end = {map2d: this, point: &end};
    map2d_point_t ** map2d_point_path = (map2d_point_t **) find_path(
            &map2d_point_start,
            &map2d_point_end,
            map2d_find_path_distance,
            map2d_find_path_distance,
            map2d_find_path_neighborhood,
            map2d_find_path_point_eq,
            path_size, path_distance);

    if (map2d_point_path == NULL) {
        return NULL;
    }

    point_t ** path = calloc(*path_size, sizeof(point_t *));
    uint32_t i;
    for (i = 0; i < *path_size; i++) {
        path[i] = map2d_point_path[i]->point;
    }

    free(map2d_point_path);

    return path;
}

uint32_t map2d_hash_point(map2d_t * this, point_t * point) {
    return map2d_hash_xy(this, point->x, point->y);
}

uint32_t map2d_hash_xy(map2d_t * this, uint32_t x, uint32_t y) {
    return x + y * this->width;
}

point_t * map2d_build_point(map2d_t * this, uint32_t x, uint32_t y) {
    uint32_t hash = map2d_hash_xy(this, x, y);
    if (this->point_set[hash] == NULL) {
        this->point_set[hash] = point_new(x, y);
    }
    return this->point_set[hash];
}

map2d_point_t * map2d_build_map2d_point(map2d_t * this, uint32_t x, uint32_t y) {
    uint32_t hash = map2d_hash_xy(this, x, y);
    if (this->map2d_point_set[hash] == NULL) {
        point_t * point = point_new(x, y);
        if (point == NULL) {
            return NULL;
        }
        this->map2d_point_set[hash] = map2d_point_new(this, point);
    }
    return this->map2d_point_set[hash];
}

int map2d_is_wall(map2d_t * this, uint32_t x, uint32_t y) {
    uint32_t hash = map2d_hash_xy(this, x, y);
    return x >= this->width || y >= this->height || this->walls[hash] != 0 ? 1 : 0;
}

map2d_point_t ** map2d_build_point_neighborhood(map2d_t * this, point_t * p,
        uint32_t * neighborhood_size) {
    *neighborhood_size = 0;

    if (p == NULL) {
        return NULL;
    }

    if (map2d_is_wall(this, p->x - 1, p->y) == 0)
        (*neighborhood_size)++;
    if (map2d_is_wall(this, p->x + 1, p->y) == 0)
        (*neighborhood_size)++;
    if (map2d_is_wall(this, p->x, p->y - 1) == 0)
        (*neighborhood_size)++;
    if (map2d_is_wall(this, p->x, p->y + 1) == 0)
        (*neighborhood_size)++;

    if (*neighborhood_size == 0) {
        return NULL;
    }

    map2d_point_t ** neighbors = calloc(*neighborhood_size, sizeof(map2d_point_t *));
    if (neighbors  == NULL) {
        *neighborhood_size = 0;
        return NULL;
    }

    int i = 0;
    if (map2d_is_wall(this, p->x - 1, p->y) == 0) {
        neighbors[i] = map2d_build_map2d_point(this, p->x - 1, p->y);
        if (neighbors[i] == NULL) {
            (*neighborhood_size)--;
        }
        else {
            i++;
        }
    }
    if (map2d_is_wall(this, p->x + 1, p->y) == 0) {
        neighbors[i] = map2d_build_map2d_point(this, p->x + 1, p->y);
        if (neighbors[i] == NULL) {
            (*neighborhood_size)--;
        }
        else {
            i++;
        }
    }
    if (map2d_is_wall(this, p->x, p->y - 1) == 0) {
        neighbors[i] = map2d_build_map2d_point(this, p->x, p->y - 1);
        if (neighbors[i] == NULL) {
            (*neighborhood_size)--;
        }
        else {
            i++;
        }
    }
    if (map2d_is_wall(this, p->x, p->y + 1) == 0) {
        neighbors[i] = map2d_build_map2d_point(this, p->x, p->y + 1);
        if (neighbors[i] == NULL) {
            (*neighborhood_size)--;
        }
        else {
            i++;
        }
    }

    if (*neighborhood_size == 0) {
        free(neighbors);
        return NULL;
    }

    return neighbors;
}

double map2d_find_path_distance(void * v1, void * v2) {
    point_t * p1 = ((map2d_point_t *) v1)->point;
    point_t * p2 = ((map2d_point_t *) v2)->point;
    return map2d_distance(*p1, *p2);
}

void ** map2d_find_path_neighborhood(void * vposition, uint32_t * nb_neighbors) {
    map2d_point_t * map_point = (map2d_point_t *) vposition;
    return (void **) map2d_build_point_neighborhood(map_point->map2d,
            map_point->point, nb_neighbors);
}

int map2d_find_path_point_eq(void * v1, void * v2) {
    point_t * p1 = ((map2d_point_t *) v1)->point;
    point_t * p2 = ((map2d_point_t *) v2)->point;
    return point_eq(p1, p2);
}

uint32_t map2d_get_width(map2d_t * this) {
    return this->width;
}

uint32_t map2d_get_height(map2d_t * this) {
    return this->height;
}

