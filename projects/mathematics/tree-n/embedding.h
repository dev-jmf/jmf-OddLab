#ifndef EMBEDDING_H
#define EMBEDDING_H

#include "tree.h"

/* Tests rooted, color-preserving subtree embedding. */
int tree_embeds(const TreeNode *host, const TreeNode *pattern);

#endif