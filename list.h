#ifndef LIST_H
#define LIST_H

typedef void * (* list_unary_f)(void *);
typedef int (* list_equality_f)(void *, void *);
typedef int (* list_cmp_f)(void *, void *);

typedef struct list_t list_t;

list_t * list_new();

void list_free(list_t *);

int list_is_empty(list_t *);

int list_push_front(list_t *, void *);

void * list_pop_front(list_t *);

void list_map(list_t *, list_unary_f);

int list_contains(list_t *, void *, list_equality_f);

void * list_find(list_t *, void *, list_equality_f);

int priority_list_push(list_t *, void *, list_cmp_f);

#endif
