#include "pathfinding.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define WIDTH 49
#define HEIGHT 17

int map[WIDTH * HEIGHT] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,
        1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
        1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1,
        1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1,
        1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
        1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1,
        1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
        1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1,
        1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1,
        1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1,
        1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

typedef struct Point {
    uint32_t x;
    uint32_t y;
} Point;

static Point start;
static Point end;
static Point * start_ptr = &start;

static Point * map_point[WIDTH * HEIGHT] = {NULL};

static Point * get_point(uint32_t x, uint32_t y) {
    uint32_t p = x + y * HEIGHT;
    if (map_point[p] == NULL) {
        map_point[p] = (Point *) malloc(sizeof(Point));
    }
    return map_point[p];
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
    if (map[p->x - 1 + p->y * HEIGHT] == 0)
        *neighborhood_size++;
    if (map[p->x + 1 + p->y * HEIGHT] == 0)
        *neighborhood_size++;
    if (map[p->x + (p->y - 1) * HEIGHT] == 0)
        *neighborhood_size++;
    if (map[p->x + (p->y + 1) * HEIGHT] == 0)
        *neighborhood_size++;

    if (*neighborhood_size == 0) {
        return NULL;
    }

    Point  ** neighbors = calloc(*neighborhood_size, sizeof(Point));
    if (neighbors  == NULL) {
        *neighborhood_size = 0;
        return NULL;
    }

    int i = 0;
    if (map[p->x - 1 + p->y * HEIGHT] == 0) {
        neighbors[i] = get_point(p->x - 1, p->y);
        if (neighbors[i] == NULL) {
            *neighborhood_size--;
        }
        else {
            i++;
        }
    }
    if (map[p->x + 1 + p->y * HEIGHT] == 0) {
        neighbors[i] = get_point(p->x + 1, p->y);
        if (neighbors[i] == NULL) {
            *neighborhood_size--;
        }
        else {
            i++;
        }
    }
    if (map[p->x + (p->y - 1) * HEIGHT] == 0) {
        neighbors[i] = get_point(p->x, p->y - 1);
        if (neighbors[i] == NULL) {
            *neighborhood_size--;
        }
        else {
            i++;
        }
    }
    if (map[p->x + (p->y + 1) * HEIGHT] == 0) {
        neighbors[i] = get_point(p->x, p->y + 1);
        if (neighbors[i] == NULL) {
            *neighborhood_size--;
        }
        else {
            i++;
        }
    }

    return (void **) neighbors;
}

int main(void) {
    start.x = 1;
    start.y = 3;
    end.x = WIDTH - 2;
    end.y = HEIGHT - 2;

    uint32_t path_size;
    double path_distance;
    Point * path = (Point *) find_path(&start, &end,
            euclidean_distance, euclidean_distance, neighborhood, position_eq,
            &path_size, &path_distance);
    if (path == NULL) {
        fprintf(stderr, "Erreur");
        return -1;
    }

    printf("%u %lf", path_size, path_distance);
    return 0;
}
