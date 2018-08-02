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

#define RBTREE_NODE_VEC_CAP (1)

static inline void
rbtree_single_rotate(struct rbtree_node **root, struct rbtree_node *curr, int direct)
{
    assert(curr);

    struct rbtree_node *rnode = curr->childs[direct];

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
rbtree_fixup(struct rbtree_node **root, struct rbtree_node *node)
{
    assert(node);
    
    struct rbtree_node *t = node;
    struct rbtree_node *u = NULL;
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
rbtree_insert(struct multimap *map, struct rbtree_node *node)
{
    struct rbtree_node **_root = &map->root;
    struct rbtree_node *parent = NULL;
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

static struct rbtree_node *
rbtree_get(struct rbtree_node *root, int key)
{
    struct rbtree_node *_root = root;
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
rbtree_node_vec_init(struct rbtree_node_vec *vec)
{
    vec->vec = (int*)malloc(sizeof(int) * RBTREE_NODE_VEC_CAP);
    if (vec->vec == NULL) {
        return false;
    }
    
    vec->cap  = RBTREE_NODE_VEC_CAP;
    vec->size = 0;

    return true;
}

// TODO: use me!
static inline void
rbtree_node_vec_free(struct rbtree_node_vec *vec)
{
    assert(vec);
    assert(vec->vec);

    free(vec->vec);
    free(vec);
}

static long long
rbtree_node_vec_add(struct rbtree_node_vec *vec, int val)
{
    assert(vec);

    if (vec->size >= vec->cap) {
        size_t new_cap = vec->cap * 2;
        int *new_vec = realloc(vec->vec, sizeof(int) * new_cap);
        if (new_vec == NULL) {
            return -1;
        }
        vec->vec = new_vec;
        vec->cap = new_cap;
    }

    size_t idx = vec->size;
    
    vec->vec[idx] = val;
    vec->size +=1;

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

    struct rbtree_node *node = rbtree_get(map->root, key);
    if (node == NULL) {
        node = (struct rbtree_node*)malloc(sizeof(struct rbtree_node));
        if (node == NULL) {
            goto fail_2;
        }
        if (!rbtree_node_vec_init(&node->vals)) {
            goto fail_1;
        }

        node->key = key;
        
        rbtree_insert(map, node);
    }

    // TODO: this is not clear and should be refactored
    long long idx = rbtree_node_vec_add(&node->vals, val);
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
