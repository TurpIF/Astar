#include <stdlib.h>
#include <string.h>

#include "pathfinding.h"
#include "list.h"

typedef struct node_t {
    double distance_from_start;
    double distance_to_end;
    double distance;
    void * position;
    struct node_t * parent;
} node_t;

typedef struct astar_t {
    void * start;
    void * end;
    list_t * close_set;
    list_t * open_set;
    list_t * trash_set;
    distance_f distance;
    heuristic_distance_f heuristic_distance;
    neighborhood_f neighborhood;
    position_eq_f position_eq;
} astar_t;

typedef struct {
    astar_t * astar;
    void * position;
} astar_position_t;

static int astar_position_eq(void * vnode, void * vastar_node);

static node_t * node_new(void * position, double distance_from_start,
        double distance_to_end, node_t * parent);

static void node_free(node_t * n);

static void * vnode_free(void * vn);

static int node_cmp(void * a, void * b);

static astar_t * astar_new(void * start, void * end,
        distance_f distance,
        heuristic_distance_f heuristic_distance,
        neighborhood_f neighborhood,
        position_eq_f position_eq);

static void astar_free(astar_t * this);

static int astar_push_to_open_set(astar_t * this, node_t * node);

static node_t * astar_pop_from_open_set(astar_t * this);

static node_t * astar_find_in_open_set(astar_t * this, void * position);

static int astar_add_to_close_set(astar_t * this, void * position);

static int astar_in_close_set(astar_t * this, void * position);

static node_t * astar_compute(astar_t * this);


int astar_position_eq(void * vnode, void * vastar_position) {
    node_t * node = (node_t *) vnode;
    astar_position_t * astar_position = (astar_position_t *) vastar_position;
    return astar_position->astar->position_eq(node->position, astar_position->position);
}

node_t * node_new(void * position, double distance_from_start,
        double distance_to_end, node_t * parent) {
    node_t * this = (node_t *) malloc(sizeof(node_t));
    if (this == NULL) {
        return NULL;
    }
    this->position = position;
    this->distance_from_start = distance_from_start;
    this->distance_to_end = distance_to_end;
    this->distance = distance_from_start + distance_to_end;
    this->parent = parent;
    return this;
}

void node_free(node_t * n) {
    if (n == NULL)
        return;
    free(n);
}

void * vnode_free(void * vn) {
    node_free((node_t *) vn);
    return NULL;
}

int node_cmp(void * a, void * b) {
    node_t * na = (node_t *) a;
    node_t * nb = (node_t *) b;
    return na->distance - nb->distance;
}

astar_t * astar_new(void * start, void * end,
        distance_f distance,
        heuristic_distance_f heuristic_distance,
        neighborhood_f neighborhood,
        position_eq_f position_eq) {
    astar_t * this = malloc(sizeof(astar_t));
    if (this == NULL)
        return NULL;

    this->start = start;
    this->end = end;
    this->trash_set = list_new();
    if (this->trash_set == NULL) {
        astar_free(this);
        return NULL;
    }
    this->close_set = list_new();
    if (this->close_set == NULL) {
        astar_free(this);
        return NULL;
    }
    this->open_set = list_new();
    if (this->open_set == NULL) {
        astar_free(this);
        return NULL;
    }
    this->distance = distance;
    this->heuristic_distance = heuristic_distance;
    this->neighborhood = neighborhood;
    this->position_eq = position_eq;
    return this;
}

void astar_free(astar_t * this) {
    if (this->trash_set != NULL) {
        list_map(this->trash_set, vnode_free);
        list_free(this->trash_set);
    }
    if (this->open_set != NULL)
        list_free(this->open_set);
    if (this->close_set != NULL)
        list_free(this->close_set);

    this->trash_set = NULL;
    this->open_set = NULL;
    this->close_set = NULL;
    free(this);
}

int astar_push_to_open_set(astar_t * this, node_t * node) {
    return priority_list_push(this->open_set, (void *) node, node_cmp);
}

node_t * astar_pop_from_open_set(astar_t * this) {
    return list_pop_front(this->open_set);
}

node_t * astar_find_in_open_set(astar_t * this, void * position) {
    astar_position_t closure;
    closure.astar = this;
    closure.position = position;
    return (node_t *) list_find(this->open_set, (void *) &closure, astar_position_eq);
}

int astar_add_to_close_set(astar_t * this, void * position) {
    return list_push_front(this->close_set, position);
}

int astar_in_close_set(astar_t * this, void * position) {
    return list_contains(this->close_set, position, this->position_eq);
}

node_t * astar_compute(astar_t * this) {
    double heuristic_distance = this->heuristic_distance(this->start, this->end);
    node_t * node = node_new(this->end, 0.0, heuristic_distance, NULL);
    if (node == NULL)
        return NULL;
    if (list_push_front(this->trash_set, node) != 0) {
        astar_free(this);
        return NULL;
    }
    if (astar_push_to_open_set(this, node) != 0) {
        astar_free(this);
        return NULL;
    }

    while (!list_is_empty(this->open_set)) {
        node_t * node = astar_pop_from_open_set(this);
        if (node == NULL) {
            astar_free(this);
            return NULL;
        }
        if (astar_add_to_close_set(this, node->position) != 0) {
            astar_free(this);
            return NULL;
        }

        // Success
        if (this->position_eq(node->position, this->start)) {
            return node;
        }

        uint32_t nb_neighbors;
        void ** neighbors = this->neighborhood(node->position, &nb_neighbors);
        if (neighbors == NULL) {
            astar_free(this);
            return NULL;
        }

        uint32_t i;
        for (i = 0; i < nb_neighbors; i++) {
            if (astar_in_close_set(this, neighbors[i]) != 0) {
                continue;
            }

            double ds = node->distance_from_start \
                        + this->distance(node->position, neighbors[i]);
            double de = this->heuristic_distance(neighbors[i], this->start);

            node_t * neighbor_node = NULL;
            neighbor_node = astar_find_in_open_set(this, neighbors[i]);
            if (neighbor_node != NULL) {
                neighbor_node->distance_from_start = ds;
                neighbor_node->distance_to_end = de;
                neighbor_node->distance = ds + de;
                neighbor_node->position = neighbors[i];
                neighbor_node->parent = node;
            }
            else {
                neighbor_node = node_new(neighbors[i], ds, de, node);
                if (neighbor_node == NULL) {
                    free(neighbors);
                    astar_free(this);
                    return NULL;
                }
                astar_push_to_open_set(this, neighbor_node);
            }
        }
        free(neighbors);
    }

    return NULL;
}

void ** find_path(void * start, void * end,
        distance_f distance,
        heuristic_distance_f heuristic_distance,
        neighborhood_f neighborhood,
        position_eq_f position_eq,
        uint32_t * path_size, double * path_distance) {
    astar_t * astar = astar_new(start, end,
            distance, heuristic_distance, neighborhood, position_eq);
    if (astar == NULL)
        return NULL;

    node_t * node = astar_compute(astar);

    if (node == NULL) {
      return NULL;
    }

    if (path_distance != NULL)
        *path_distance = node->distance;

    // Rebuild the path
    node_t * n = node;
    *path_size = 0;
    while (n->parent != NULL) {
        n = n->parent;
        (*path_size)++;
    }

    void ** path = (void **) calloc(*path_size, sizeof(void *));
    if (path == NULL)
        return NULL;

    uint32_t i;
    for (i = 0; i < *path_size; i++, node = node->parent) {
        path[i] = node->position;
    }

    astar_free(astar);

    return path;
}
