#include "pathfinding.h"
#include "map2d.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* #define WIDTH 5 */
/* #define HEIGHT 3 */
/*  */
/* int map[WIDTH * HEIGHT] = { */
/*     0, 1, 0, 0, 0, */
/*     0, 1, 0, 1, 0, */
/*     0, 0, 0, 1, 0 */
/* }; */
#define WIDTH 49
#define HEIGHT 17

int map_walls[WIDTH * HEIGHT] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1,
    1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1,
    1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
    1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1,
    1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1,
    1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1,
    1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

typedef struct Point {
    uint32_t x;
    uint32_t y;
} Point;

/* static Point start; */
/* static Point end; */

static Point * point_set[WIDTH * HEIGHT] = {NULL};

static Point * point_new(uint32_t x, uint32_t y) {
    Point * p = (Point *) malloc(sizeof(Point));
    if (p == NULL) {
        return NULL;
    }
    p->x = x;
    p->y = y;
    return p;
}

static void point_free(Point * this) {
    free(this);
}

static void point_set_free() {
    uint32_t i;
    for (i = 0; i < WIDTH * HEIGHT; i++) {
        point_free(point_set[i]);
    }
}

static Point * get_point(uint32_t x, uint32_t y) {
    uint32_t p = x + y * WIDTH;
    if (point_set[p] == NULL) {
        point_set[p] = point_new(x, y);
    }
    return point_set[p];
}

static int is_wall(uint32_t x, uint32_t y) {
    return x >= WIDTH || y >= HEIGHT || map_walls[x + y * WIDTH] != 0 ? 1 : 0;
}

static int position_eq(void * vp1, void * vp2) {
    Point * p1 = (Point *) vp1;
    Point * p2 = (Point *) vp2;
    if (p1->x == p2->x && p1->y == p2->y) {
        return 1;
    }
    return 0;
}

static double euclidean_distance(void * pa, void * pb) {
    Point * a = (Point *) pa;
    Point * b = (Point *) pb;
    return sqrt((a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y));
}

static void ** neighborhood(void * position,
        uint32_t * neighborhood_size) {
    Point * p = (Point *) position;

    *neighborhood_size = 0;
    if (is_wall(p->x - 1, p->y) == 0)
        (*neighborhood_size)++;
    if (is_wall(p->x + 1, p->y) == 0)
        (*neighborhood_size)++;
    if (is_wall(p->x, p->y - 1) == 0)
        (*neighborhood_size)++;
    if (is_wall(p->x, p->y + 1) == 0)
        (*neighborhood_size)++;

    if (*neighborhood_size == 0) {
        return NULL;
    }

    Point  ** neighbors = calloc(*neighborhood_size, sizeof(Point));
    if (neighbors  == NULL) {
        *neighborhood_size = 0;
        return NULL;
    }

    int i = 0;
    if (is_wall(p->x - 1, p->y) == 0) {
        neighbors[i] = get_point(p->x - 1, p->y);
        if (neighbors[i] == NULL) {
            (*neighborhood_size)--;
        }
        else {
            i++;
        }
    }
    if (is_wall(p->x + 1, p->y) == 0) {
        neighbors[i] = get_point(p->x + 1, p->y);
        if (neighbors[i] == NULL) {
            (*neighborhood_size)--;
        }
        else {
            i++;
        }
    }
    if (is_wall(p->x, p->y - 1) == 0) {
        neighbors[i] = get_point(p->x, p->y - 1);
        if (neighbors[i] == NULL) {
            (*neighborhood_size)--;
        }
        else {
            i++;
        }
    }
    if (is_wall(p->x, p->y + 1) == 0) {
        neighbors[i] = get_point(p->x, p->y + 1);
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

    return (void **) neighbors;
}

static void print_map(map2d_t * map2d, point_t ** path, uint32_t path_size) {
    uint32_t x, y;
    for (y = 0; y < map2d_get_height(map2d); y++) {
        for (x = 0; x < map2d_get_width(map2d); x++) {
            if (map2d_is_wall(map2d, x, y)) {
                printf("#");
            }
            else {
                char c = ' ';
                if (path != NULL) {
                    uint32_t n;
                    for (n = 0; n < path_size; n++) {
                        if (path[n]->x == x && path[n]->y == y) {
                            c = '.';
                        }
                    }
                }
                printf("%c", c);
            }
        }
        if (y < map2d_get_height(map2d) - 1) {
            printf("\n");
        }
    }
}

int main(void) {
    point_t start = {x: 1, y: 3};
    point_t end = {x: WIDTH - 2, y: HEIGHT - 2};
    /* start.x = 1; */
    /* start.y = 3; */
    /* end.x = WIDTH - 2; */
    /* end.y = HEIGHT - 2; */
    /* start.x = 0; */
    /* start.y = 0; */
    /* end.x = WIDTH - 1; */
    /* end.y = HEIGHT - 1; */

    uint32_t path_size;
    double path_distance;
    /* Point ** path = (Point **) find_path(&start, &end, */
    /*         euclidean_distance, euclidean_distance, neighborhood, position_eq, */
    /*         &path_size, &path_distance); */

    map2d_t * map2d = map2d_new(map_walls, WIDTH, HEIGHT);
    point_t ** path = map2d_find_path(map2d, start, end, &path_size, &path_distance);
    if (path == NULL) {
        fprintf(stderr, "Erreur");
        return -1;
    }

    /* printf("%lu %lf", path_size, path_distance); */
    print_map(map2d, path, path_size);
    map2d_free(map2d);

    return 0;
}
