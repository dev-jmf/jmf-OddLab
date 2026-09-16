#include "embedding.h"

#include <stddef.h>
#include <stdlib.h>

static int children_embed(const TreeNode *host, const TreeNode *pattern,
                          size_t pattern_index, int *used)
{
    size_t i;

    if (pattern_index == pattern->child_count) {
        return 1;
    }
    for (i = 0; i < host->child_count; ++i) {
        if (!used[i] && tree_embeds(host->children[i], pattern->children[pattern_index])) {
            used[i] = 1;
            if (children_embed(host, pattern, pattern_index + 1, used)) {
                return 1;
            }
            used[i] = 0;
        }
    }
    return 0;
}

int tree_embeds(const TreeNode *host, const TreeNode *pattern)
{
    int *used;
    int result;

    if (host == NULL || pattern == NULL || host->color != pattern->color ||
        host->child_count < pattern->child_count) {
        return 0;
    }
    if (pattern->child_count == 0) {
        return 1;
    }
    used = calloc(host->child_count, sizeof(*used));
    if (used == NULL) {
        return 0;
    }
    result = children_embed(host, pattern, 0, used);
    free(used);
    return result;
}