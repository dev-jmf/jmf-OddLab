#include "tree.h"

#include <stdlib.h>

TreeNode *tree_node_create(unsigned int color)
{
    TreeNode *node = calloc(1, sizeof(*node));
    if (node != NULL) {
        node->color = color;
    }
    return node;
}

int tree_node_add_child(TreeNode *parent, TreeNode *child)
{
    TreeNode **children;
    size_t capacity;

    if (parent == NULL || child == NULL) {
        return 0;
    }
    if (parent->child_count == parent->child_capacity) {
        capacity = parent->child_capacity == 0 ? 4 : parent->child_capacity * 2;
        children = realloc(parent->children, capacity * sizeof(*children));
        if (children == NULL) {
            return 0;
        }
        parent->children = children;
        parent->child_capacity = capacity;
    }
    parent->children[parent->child_count++] = child;
    return 1;
}

TreeNode *tree_clone(const TreeNode *source)
{
    TreeNode *copy;
    size_t i;

    if (source == NULL) {
        return NULL;
    }
    copy = tree_node_create(source->color);
    if (copy == NULL) {
        return NULL;
    }
    for (i = 0; i < source->child_count; ++i) {
        TreeNode *child = tree_clone(source->children[i]);
        if (child == NULL || !tree_node_add_child(copy, child)) {
            tree_free(child);
            tree_free(copy);
            return NULL;
        }
    }
    return copy;
}

void tree_free(TreeNode *root)
{
    size_t i;

    if (root == NULL) {
        return;
    }
    for (i = 0; i < root->child_count; ++i) {
        tree_free(root->children[i]);
    }
    free(root->children);
    free(root);
}

size_t tree_size(const TreeNode *root)
{
    size_t total = 0;
    size_t i;

    if (root == NULL) {
        return 0;
    }
    for (i = 0; i < root->child_count; ++i) {
        total += tree_size(root->children[i]);
    }
    return 1 + total;
}

int tree_depth(const TreeNode *root)
{
    int deepest = 0;
    size_t i;

    if (root == NULL) {
        return 0;
    }
    for (i = 0; i < root->child_count; ++i) {
        int depth = tree_depth(root->children[i]);
        if (depth > deepest) {
            deepest = depth;
        }
    }
    return deepest + 1;
}