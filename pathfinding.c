#include <stdlib.h>
#include <string.h>

#include "pathfinding.h"
#include "list.h"

typedef struct Node {
    double distance_from_start;
    double distance_to_end;
    double distance;
    void * position;
    struct Node * parent;
} Node;

typedef struct AStar {
    void * start;
    void * end;
    list_t * close_set;
    Node ** open_set;
    uint32_t open_set_size;
    uint32_t open_set_max_size;
    distance_f distance;
    heuristic_distance_f heuristic_distance;
    neighborhood_f neighborhood;
    position_eq_f position_eq;
} AStar;

static Node * node_new(void * position, double distance_from_start,
        double distance_to_end, Node * parent);

static void node_free(Node * n);

static int node_cmp(Node * a, Node * b);

static AStar * astar_new(void * start, void * end,
        distance_f distance,
        heuristic_distance_f heuristic_distance,
        neighborhood_f neighborhood,
        position_eq_f position_eq,
        uint32_t space_size);

static void astar_free();

static int astar_push_to_open_set(AStar * this, Node * node);

static Node * astar_pop_from_open_set(AStar * this);

static int astar_add_to_close_set(AStar * this, Node * node);

static int astar_in_close_set(AStar * this, void * position);

static Node * astar_compute(AStar * this);


Node * node_new(void * position, double distance_from_start,
        double distance_to_end, Node * parent) {
    Node * this = (Node *) malloc(sizeof(Node));
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

void node_free(Node * n) {
    if (n == NULL)
        return;
    free(n);
}

int node_cmp(Node * a, Node * b) {
    return a->distance - b->distance;
}

AStar * astar_new(void * start, void * end,
        distance_f distance,
        heuristic_distance_f heuristic_distance,
        neighborhood_f neighborhood,
        position_eq_f position_eq,
        uint32_t space_size) {
    AStar * this = malloc(sizeof(AStar));
    if (this == NULL)
        return NULL;

    this->start = start;
    this->end = end;
    this->close_set = list_new();
    if (this->close_set == NULL) {
      free(this);
      return NULL;
    }
    this->open_set = calloc(sizeof(Node *), space_size);
    if (this->open_set == NULL) {
        free(this->close_set);
        free(this);
        return NULL;
    }
    this->open_set_size = 0;
    this->open_set_max_size = space_size;
    this->distance = distance;
    this->heuristic_distance = heuristic_distance;
    this->neighborhood = neighborhood;
    this->position_eq = position_eq;
    return this;
}

void astar_free() {
    // TODO
}

int astar_push_to_open_set(AStar * this, Node * node) {
    if (this->open_set_size >= this->open_set_max_size)
        return -1;

    uint32_t i;
    for (i = 0; i < this->open_set_size; i++) {
        if (node_cmp(node, this->open_set[i]) <= 0) {
            break;
        }
    }

    memmove(&this->open_set[i], &this->open_set[i + 1],
            sizeof(Node *) * (this->open_set_size - i));
    this->open_set[i] = node;
    this->open_set_size++;
    return 0;
}

static Node * astar_pop_from_open_set(AStar * this) {
    if (this->open_set_size <= 0) {
        return NULL;
    }

    Node * node = this->open_set[0];
    memmove(&this->open_set[1], &this->open_set[0],
            sizeof(Node *) * (this->open_set_size - 1));
    this->open_set_size--;
    return node;
}

int astar_add_to_close_set(AStar * this, Node * node) {
    return list_push_front(this->close_set, (void *) &node->position);
}

int astar_in_close_set(AStar * this, void * position) {
    return list_contains(this->close_set, (void *) &position, this->position_eq);
}

Node * astar_compute(AStar * this) {
    double heuristic_distance = this->heuristic_distance(this->start, this->end);
    Node * node = node_new(this->end, 0.0, heuristic_distance, NULL);
    if (node == NULL)
        return NULL;
    if (astar_push_to_open_set(this, node) != 0)
        return NULL;

    while (this->open_set_size > 0) {
        Node * node = astar_pop_from_open_set(this);
        if (node == NULL)
            return NULL;
        if (astar_add_to_close_set(this, node) != 0)
            return NULL;

        // Success
        if (this->position_eq(node->position, this->start)) {
            return node;
        }

        uint32_t nb_neighbors;
        void ** neighbors = this->neighborhood(node->position, &nb_neighbors);
        if (neighbors == NULL)
            return NULL;

        uint32_t i;
        for (i = 0; i < nb_neighbors; i++) {
            if (astar_in_close_set(this, neighbors[i]) != 0) {
                continue;
            }

            double ds = node->distance_from_start \
                        + this->distance(node->position, neighbors[i]);
            double de = this->heuristic_distance(neighbors[i], this->start);

            Node * neighbor_node = node_new(neighbors[i], ds, de, node);
            if (neighbor_node == NULL)
                return NULL;
            astar_push_to_open_set(this, neighbor_node);
        }
    }

    return NULL;
}

void * find_path(void * start, void * end,
        distance_f distance,
        heuristic_distance_f heuristic_distance,
        neighborhood_f neighborhood,
        position_eq_f position_eq,
        uint32_t space_size, uint32_t * path_size, double * path_distance) {
    AStar * astar = astar_new(start, end,
            distance, heuristic_distance, neighborhood, position_eq, space_size);
    if (astar == NULL)
        return NULL;

    Node * node = astar_compute(astar);

    if (node == NULL) {
      return NULL;
    }

    if (path_distance != NULL)
        *path_distance = node->distance;

    // Rebuild the path
    Node * n = node;
    *path_size = 0;
    while (n->parent != NULL) {
        n = n->parent;
        *path_size++;
    }

    void ** path = (void **) calloc(sizeof(void *), *path_size); // FIXME
    if (path == NULL)
        return NULL;

    uint32_t i;
    for (i = 0; i < *path_size; i++, node = node->parent) {
        path[i] = node->position;
    }

    return path;
}
