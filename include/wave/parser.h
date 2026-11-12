#ifndef WAVE_PARSER_H
#define WAVE_PARSER_H

#include <stdint.h>
#include <wave/log.h>
#include <stddef.h>
#include <wave/context.h>
#include <wave/token.h>
#include <wave/ast.h>

#define Wave_Parser_CRITICAL_ERROR      (1u << 0)       // mk: critical error occurred, requires emergency stop
#define Wave_Parser_LOG_ERROR           (1u << 1)       // mk: in case even the logs fail to allocate memory for their own data, this flag would be useful

struct Wave_ParseResult {
    size_t consumed_token_count;
    struct Wave_AstNode node;
    uint8_t flags;
};

#define Wave_ParseResult_null ((struct Wave_ParseResult){ 0, Wave_AstNode_null, 0 })

// mk: subparsers are parsers that are usually embedded in other parsers, like member lists (embedded in function or struct). they don't produce a node, but modify an existing one produced by the calling parser (usually aggregating new child nodes)
struct Wave_SubparseResult {
    size_t consumed_token_count;
    uint8_t flags;
};

#define Wave_SubparseResult_null ((struct Wave_SubparseResult){ 0, 0 })

struct Wave_ParseParams {
    struct Wave_Context *const context;
    struct Wave_LogVector *const messages;
    const struct Wave_TokenVector vector;
};

struct Wave_ParseResult Wave_Parser_parse(
    const struct Wave_ParseParams params,
    const size_t vector_indexing_offset
);

// mk: parse individual expressions, like `var = 2 + 2 * (var - 1 as f32)`, ends with semicolon or `<eof>`, `<eox>`, `)`, `]`, `{`, `}`
struct Wave_ParseResult Wave_Parser_parse_expr(
    const struct Wave_ParseParams params,
    const size_t vector_indexing_offset
);

struct Wave_ParseResult Wave_Parser_parse_fn(
    const struct Wave_ParseParams params,
    const size_t vector_indexing_offset
);

#define Wave_Parser_NO_VARIADIC     (1u << 0)
#define Wave_Parser_NO_DEFAULT      (1u << 1)

struct Wave_SubparseResult Wave_Parser_parse_member_list(
    const struct Wave_ParseParams params,
    const size_t vector_indexing_offset,
    struct Wave_AstNode *const node,
    const uint8_t flags,
    const enum Wave_TokenType terminator,
    size_t *const counter
);

struct Wave_SubparseResult Wave_Parser_parse_decl(
    const struct Wave_ParseParams params,
    const size_t vector_indexing_offset,
    struct Wave_AstNode *const node
);

// mk: the tuple include parenthesis, supported `()`, `[]`, `{}`
struct Wave_ParseResult Wave_Parser_parse_tuple(
    const struct Wave_ParseParams params,
    const size_t vector_indexing_offset
);

struct Wave_ParseScannerParams {
    struct Wave_Context *const context;
    const struct Wave_TokenVector vector;
    const size_t vector_indexing_offset;
    size_t *const shift;
};

const struct Wave_Token *Wave_Parser_peek(
    struct Wave_ParseScannerParams params
);

const struct Wave_Token *Wave_Parser_next(
    struct Wave_ParseScannerParams params
);

#endif
