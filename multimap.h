#ifndef MULTIMAP_H_
#define MULTIMAP_H_

#include <stdbool.h>
#include <stddef.h>

struct multimap_item;

struct multimap_vec_item {
    struct multimap_item *multimap_item;
    int                   value;
};

struct multimap_vec {
    struct multimap_vec_item *items;
    size_t                    cap;
    size_t                    size;
};

struct multimap_rbtree_node {
    struct multimap_rbtree_node *childs[2];
    struct multimap_rbtree_node *parent;
    struct multimap_vec          values;
    int                          key;
    bool                         is_red;
};

struct multimap_item {
    struct multimap_item        *next;
    struct multimap_rbtree_node *node;
    size_t                       val_idx;
};

struct multimap {
    struct multimap_item        *head;
    struct multimap_item        *tail;
    struct multimap_rbtree_node *root;
    size_t                       size;
};

void multimap_init(struct multimap *map);
bool multimap_add(struct multimap *map, int key, int val);
bool multimap_contains(struct multimap *map, int key);
struct multimap_item multimap_next_item(struct multimap *map);

#endif
