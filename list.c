#include <stdlib.h>

#include "list.h"

typedef struct list_node_t {
    void * data;
    struct list_node_t * next;
} list_node_t;

list_node_t * list_node_new();

void list_node_free(list_node_t * this);

list_node_t * list_node_new(void * data) {
    list_node_t * this = (list_node_t *) malloc(sizeof(list_node_t));
    if (this == NULL) {
        return NULL;
    }
    this->data = data;
    return this;
}

void list_node_free(list_node_t * this) {
    free(this);
}

list_t * list_new() {
    list_t * this = (list_t *) malloc(sizeof(list_t));
    if (this == NULL) {
        return this;
    }
    this->head = NULL;
    return this;
}

void list_free(list_t * this) {
    list_node_t * node;
    for (node = this->head; node != NULL; node = node->next) {
        list_node_free(node);
    }
    free(this);
}

int list_is_empty(list_t * this) {
    return this->head == NULL ? 1 : 0;
};

int list_push_front(list_t * this, void * data) {
    list_node_t * new_head = list_node_new(data);
    if (new_head == NULL) {
        return 1;
    }
    new_head->next = this->head;
    this->head = new_head;
    return 0;
}

void * list_pop_front(list_t * this) {
    if (this->head == NULL) {
        return NULL;
    }
    list_node_t * head = this->head;
    this->head = this->head->next;
    void * data = head->data;
    list_node_free(head);
    return data;
}

void list_map(list_t * this, list_unary_f function) {
    list_node_t * node;
    for (node = this->head; node != NULL; node = node->next) {
        function(node->data);
    }
}

int list_contains(list_t * this, void * data, list_equality_f eq) {
    list_node_t * node;
    for (node = this->head; node != NULL; node = node->next) {
        if (eq == NULL && node->data == data) {
            return 1;
        }
        else if (eq != NULL && eq(node->data, data)) {
            return 1;
        }
    }
    return 0;
}

