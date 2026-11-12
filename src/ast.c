#include <wave/ast.h>

struct Wave_AstNode Wave_Ast_new_node(struct Wave_Context *const context, const size_t token_index, const size_t capacity) {
    struct Wave_AstNode node = {
        .branches = NULL,
        .count = 0,
        .capacity = 0,
        .token_index = token_index,
    };

    if (capacity > 0) {
        node.branches = w_malloc(
            context->allocator.alloc,
            context->allocator.userdata,
            capacity * sizeof(struct Wave_AstNode)
        );
        if (node.branches == NULL)
            return node;
        node.capacity = capacity;
    }

    return node;
}

bool Wave_Ast_reserve_branches(struct Wave_Context *const context, struct Wave_AstNode *const node, const size_t capacity) {
    if (node == NULL)
        return false;

    if (capacity > node->capacity) {
        // mk: the cool thing is even if the pointer is `NULL` the allocator automatically treats it as a `w_malloc`
        struct Wave_AstNode *const reallocated = w_realloc(
            context->allocator.alloc,
            context->allocator.userdata,
            node->branches,
            node->capacity * sizeof(struct Wave_AstNode),
            capacity * sizeof(struct Wave_AstNode)
        );
        if (reallocated == NULL)
            return false;
        node->branches = reallocated;
        node->capacity = capacity;
    }

    return true;
}

bool Wave_Ast_push_branch(struct Wave_Context *const context, struct Wave_AstNode *const node, const struct Wave_AstNode branch) {
    if (node == NULL)
        return false;

    if (node->count >= node->capacity) {
        if (!Wave_Ast_reserve_branches(context, node, node->capacity * 2))
            return false;
    }

    node->branches[node->count] = branch;
    node->count++;
    return true;
}

void Wave_Ast_delete_nodes(struct Wave_Context *const context, struct Wave_AstNode *const node) {
    if (node == NULL)
        return;

    for (size_t i = 0; i < node->count; i++) {
        Wave_Ast_delete_nodes(context, &node->branches[i]);
    }

    w_free(
        context->allocator.alloc,
        context->allocator.userdata,
        node->branches,
        node->capacity * sizeof(struct Wave_AstNode)
    );
    node->count = 0;
    node->capacity = 0;
    node->branches = NULL;
}
