#include "pathfinding.h"
#include "map2d.h"
#include "map2d_walls.h"

#include <stdio.h>

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
        printf("\n");
    }
}

int main(void) {
    point_t start = {x: 1, y: 1};
    point_t end = {x: MAP2D_WIDTH - 2, y: MAP2D_HEIGHT - 2};

    uint32_t path_size;
    double path_distance;

    map2d_t * map2d = map2d_new(map_walls, MAP2D_WIDTH, MAP2D_HEIGHT);
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
