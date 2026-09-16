#include "tree_n.h"

#include "embedding.h"

#include <stdlib.h>

static int simulation_add(TreeSimulation *simulation, TreeNode *tree)
{
    TreeNode **trees;
    size_t capacity;

    if (simulation->count == simulation->capacity) {
        capacity = simulation->capacity == 0 ? 8 : simulation->capacity * 2;
        trees = realloc(simulation->trees, capacity * sizeof(*trees));
        if (trees == NULL) {
            return 0;
        }
        simulation->trees = trees;
        simulation->capacity = capacity;
    }
    simulation->trees[simulation->count++] = tree;
    return 1;
}

static TreeNode *make_candidate(size_t round, int color, int colors)
{
    TreeNode *root = tree_node_create((unsigned int)color);
    TreeNode *current = root;
    size_t depth;

    if (root == NULL) {
        return NULL;
    }
    for (depth = 1; depth < round; ++depth) {
        TreeNode *child = tree_node_create((unsigned int)((color + (int)depth) % colors));
        if (child == NULL || !tree_node_add_child(current, child)) {
            tree_free(child);
            tree_free(root);
            return NULL;
        }
        current = child;
    }
    return root;
}

void simulation_init(TreeSimulation *simulation)
{
    *simulation = (TreeSimulation){0};
}

void simulation_clear(TreeSimulation *simulation)
{
    size_t i;

    for (i = 0; i < simulation->count; ++i) {
        tree_free(simulation->trees[i]);
    }
    free(simulation->trees);
    simulation_init(simulation);
}

int simulation_run(TreeSimulation *simulation, int n)
{
    int color;

    simulation_clear(simulation);
    if (n < 1 || n > 12) {
        return 0;
    }
    simulation->colors = n;

    /* One greedy representative per color and per round. */
    for (simulation->rounds = 1; simulation->rounds <= (size_t)n; ++simulation->rounds) {
        for (color = 0; color < n; ++color) {
            TreeNode *candidate = make_candidate(simulation->rounds, color, n);
            size_t previous;
            int forbidden = 0;

            if (candidate == NULL) {
                return 0;
            }
            for (previous = 0; previous < simulation->count; ++previous) {
                simulation->candidates_checked++;
                if (tree_embeds(simulation->trees[previous], candidate)) {
                    forbidden = 1;
                    break;
                }
            }
            if (!forbidden && !simulation_add(simulation, candidate)) {
                tree_free(candidate);
                return 0;
            }
            if (forbidden) {
                tree_free(candidate);
            }
        }
    }
    return 1;
}