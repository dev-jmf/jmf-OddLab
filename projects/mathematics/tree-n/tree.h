#ifndef TREE_H
#define TREE_H

#include <stddef.h>

typedef struct TreeNode {
    unsigned int color;
    struct TreeNode **children;
    size_t child_count;
    size_t child_capacity;
    int x;
    int y;
} TreeNode;

TreeNode *tree_node_create(unsigned int color);
int tree_node_add_child(TreeNode *parent, TreeNode *child);
TreeNode *tree_clone(const TreeNode *source);
void tree_free(TreeNode *root);
size_t tree_size(const TreeNode *root);
int tree_depth(const TreeNode *root);

#endif