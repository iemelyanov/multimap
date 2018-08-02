#ifndef MULTIMAP_H_
#define MULTIMAP_H_

#include <stdbool.h>
#include <stddef.h>

struct rbtree_node_vec {
    int    *vec;
    size_t  cap;
    size_t  size;
};

struct rbtree_node {
    struct rbtree_node     *childs[2];
    struct rbtree_node     *parent;
    struct rbtree_node_vec  vals;
    int                     key;
    bool                    is_red;
};

struct multimap_item {
    struct multimap_item *next;
    struct rbtree_node   *node;
    size_t                val_idx;
};

struct multimap {
    struct multimap_item *head;
    struct multimap_item *tail;
    struct rbtree_node   *root;
    size_t                size;
};

void multimap_init(struct multimap *map);
bool multimap_add(struct multimap *map, int key, int val);
bool multimap_contains(struct multimap *map, int key);
struct multimap_item multimap_next_item(struct multimap *map);

#endif
