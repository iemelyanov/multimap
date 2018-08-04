#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "multimap.h"

#define parent(node) ((node)->parent)

#define gparent(node) (parent(parent(node)))

#define uncle(node, direct) (gparent(node)->childs[direct])

#define sibling(node, direct) (parent(node)->childs[direct])

#define set_red(node) ((node)->is_red = true)

#define is_red(node) ((node)->is_red)

#define set_black(node) ((node)->is_red = false)

#define MULTIMAP_VEC_CAP (1)

static inline void
rbtree_single_rotate(struct multimap_rbtree_node **root,
                     struct multimap_rbtree_node *curr,
                     int direct)
{
    assert(curr);

    struct multimap_rbtree_node *rnode = curr->childs[direct];

    curr->childs[direct] = rnode->childs[!direct];

    if (rnode->childs[!direct] != NULL) {
        rnode->childs[!direct]->parent = curr;
    }

    rnode->parent = curr->parent;
    
    if (curr->parent != NULL) {
        if (curr == curr->parent->childs[!direct]) {
            curr->parent->childs[!direct] = rnode;
        } else {
            curr->parent->childs[direct] = rnode;
        }
    }

    if (curr == *root) {
        *root = rnode;
    }

    rnode->childs[!direct] = curr;
    curr->parent           = rnode;
}

static inline void
rbtree_fixup(struct multimap_rbtree_node **root,
             struct multimap_rbtree_node *node)
{
    assert(node);
    
    struct multimap_rbtree_node *t = node;
    struct multimap_rbtree_node *u = NULL;
    int direct            = 0;
    
    while (t != *root && is_red(parent(t))) {
        direct = (parent(t) == uncle(t, 0));
        u = uncle(t, direct);
        if (u != NULL && is_red(u)) {
            set_red(gparent(t));
            set_black(parent(t));
            set_black(u);
            t = gparent(t);
        } else {
            if (t == sibling(t, direct)) {
                t = parent(t);
                rbtree_single_rotate(root, t, direct);
            }
            set_red(gparent(t));
            set_black(parent(t));
            rbtree_single_rotate(root, gparent(t), !direct);
        }
    }
    
    set_black(*root);
}

static void
rbtree_insert(struct multimap *map, struct multimap_rbtree_node *node)
{
    struct multimap_rbtree_node **_root = &map->root;
    struct multimap_rbtree_node *parent = NULL;
    int direct                 = 0;

    while (*_root != NULL) {
        if (node->key == (*_root)->key) {
            return;
        }
        parent = *_root;
        direct = node->key > (*_root)->key;
        _root  = &(*_root)->childs[direct];
    }

    node->is_red = true;
    node->parent = parent;
    *_root       = node;

    rbtree_fixup(&map->root, node);
}

static struct multimap_rbtree_node *
rbtree_get(struct multimap_rbtree_node *root, int key)
{
    struct multimap_rbtree_node *_root = root;
    int direct                = 0;
    
    while (_root != NULL) {
        if (_root->key == key) {
            return _root;
        }
        direct = key > _root->key;
        _root  = _root->childs[direct]; 
    }
    return NULL;
}

static inline bool
vec_init(struct multimap_vec *vec)
{
    vec->items = (struct multimap_vec_item*)malloc(
        sizeof(struct multimap_vec_item) * MULTIMAP_VEC_CAP);
    if (vec->items == NULL) {
        return false;
    }
    
    vec->cap  = MULTIMAP_VEC_CAP;
    vec->size = 0;

    return true;
}

// Todo: use me!
static inline void
vec_free(struct multimap_vec *vec)
{
    assert(vec);
    assert(vec->items);

    free(vec->items);
    free(vec);
}

static long long
vec_add(struct multimap_vec *vec, int val, struct multimap_item *item)
{
    assert(vec);

    if (vec->size >= vec->cap) {
        size_t new_cap = vec->cap * 2;
        struct multimap_vec_item *new_items = realloc(
            vec->items, sizeof(struct multimap_vec_item) * new_cap);
        if (new_items == NULL) {
            return -1;
        }
        vec->items = new_items;
        vec->cap   = new_cap;
    }

    size_t idx = vec->size;

    struct multimap_vec_item vec_item = {
        .multimap_item = item,
        .value         = val
    };
    
    vec->items[idx]  = vec_item;
    vec->size       +=1;

    return idx;
}

void
multimap_init(struct multimap *map)
{
    assert(map);

    map->head = NULL;
    map->tail = NULL;
    map->root = NULL;
    map->size = 0;
}

bool
multimap_add(struct multimap *map, int key, int val)
{
    assert(map);

    struct multimap_item *item = (struct multimap_item*)malloc(
        sizeof(struct multimap_item));
    if (item == NULL) {
        return false;
    }

    struct multimap_rbtree_node *node = rbtree_get(map->root, key);
    if (node == NULL) {
        node = (struct multimap_rbtree_node*)malloc(
            sizeof(struct multimap_rbtree_node));
        if (node == NULL) {
            goto fail_2;
        }
        if (!vec_init(&node->values)) {
            goto fail_1;
        }

        node->key = key;
        
        rbtree_insert(map, node);
    }
    
    // TODO: this is not clear and should be refactored
    long long idx = vec_add(&node->values, val, item);
    if (idx == -1) {
        goto fail;
    }

    if (map->head == NULL) {
        map->head = item;
        map->tail = item;
    } else {
        map->tail->next = item;
        map->tail       = item;
    }

    item->next    = NULL;
    item->node    = node;
    item->val_idx = idx;

    return true;
    
fail_1:
    free(node);

fail_2:
    free(item);

fail:
    return false;
}
