#ifndef WAVE_AST_H
#define WAVE_AST_H

#include <stddef.h>
#include <stdbool.h>
#include <wave/context.h>
#include <wave/token.h>

struct Wave_AstNode {
    struct Wave_AstNode *branches;
    size_t count;
    size_t capacity;
    size_t token_index; // mk: index in the token vector
};

struct Wave_AstNode Wave_Ast_new_node(struct Wave_Context *const context, const size_t token_index, const size_t capacity);
bool Wave_Ast_reserve_branches(struct Wave_Context *const context, struct Wave_AstNode *const node, const size_t capacity);
bool Wave_Ast_push_branch(struct Wave_Context *const context, struct Wave_AstNode *const ndoe, const struct Wave_AstNode branch);
// mk: deep cascade delete
void Wave_Ast_delete_nodes(struct Wave_Context *const context, struct Wave_AstNode *const node);

#endif
