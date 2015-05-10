#ifndef ASTAR_H
#define ASTAR_H

#include <stdint.h>

typedef double (* distance_f)(void *, void *);
typedef double (* heuristic_distance_f)(void *, void *);
typedef void ** (* neighborhood_f)(void *, uint32_t *);
typedef int (* position_eq_f)(void *, void *);

extern void * find_path(void * start, void * end,
        distance_f distance,
        heuristic_distance_f heuristic_distance,
        neighborhood_f neighborhood,
        position_eq_f position_eq,
        uint32_t space_size, uint32_t * path_size, double * path_distance);

#endif
