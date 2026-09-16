#ifndef TREE_N_H
#define TREE_N_H

#include <stddef.h>

#include "tree.h"

typedef struct {
    TreeNode **trees;
    size_t count;
    size_t capacity;
    size_t rounds;
    size_t candidates_checked;
    int colors;
} TreeSimulation;

void simulation_init(TreeSimulation *simulation);
void simulation_clear(TreeSimulation *simulation);
int simulation_run(TreeSimulation *simulation, int n);

#endif