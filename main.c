#include "pathfinding.h"

#include <stdio.h>
#include <math.h>

typedef struct Point {
    uint32_t x;
    uint32_t y;
} Point;

static Point start;
static Point end;
static Point * start_ptr = &start;

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
    *neighborhood_size = 1;
    return (void **) &start_ptr;
}

int main(void) {
    start.x = 0;
    start.y = 0;
    end.x = 10;
    end.y = 20;

    uint32_t path_size;
    double path_distance;
    Point * path = (Point *) find_path(&start, &end,
            euclidean_distance, euclidean_distance, neighborhood, position_eq,
            10 * 20, &path_size, &path_distance);
    if (path == NULL) {
        fprintf(stderr, "Erreur");
        return -1;
    }

    printf("%u %lf", path_size, path_distance);
    return 0;
}
