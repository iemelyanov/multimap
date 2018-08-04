#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "multimap.h"

#define COUNT 10

static void
print_tree(struct multimap_rbtree_node *root, int space)
{
    if (root == NULL) {
        return;
    }

    space += COUNT; 

    print_tree(root->childs[1], space);
 
    printf("\n");
    for (int i = COUNT; i < space; i++) {
        printf(" ");
    }

    if (root->is_red) {
        printf("%dR\n", root->key);
    } else {
        printf("%dB\n", root->key);
    }
 
    print_tree(root->childs[0], space);
}

void
benchmark(int n)
{
    struct multimap map;
    multimap_init(&map);

    for (int i = 0; i < n; i++) {
    }
}

void
test()
{
    struct multimap map;
    multimap_init(&map);

    multimap_add(&map, 1, 1);
    multimap_add(&map, 1, 2);
    multimap_add(&map, 2, 1);
    multimap_add(&map, 3, 1);
    multimap_add(&map, 2, 2);

    assert(multimap_contains(&map, 1));
    assert(multimap_contains(&map, 2));
    assert(multimap_contains(&map, 3));
    assert(!multimap_contains(&map, 4));

    // Iterate over items
    struct multimap_item *it = map.head;
    for (; it != NULL; it = it->next) {
        printf("key: %d val: %d\n", it->node->key,
               it->node->values.items[it->val_idx].value);
    }
    
    print_tree(map.root, 0);
}

int
main(int argc, char **argv)
{
    /*
    int n = 0;
    if (argc == 2) {
        n = atoi(argv[1]);
    }
    printf("%d\n", n);
    clock_t begin = clock();
    benchmark(n);
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%f\n", time_spent);
    */
    test();
    return 0;
}
