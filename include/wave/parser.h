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
#define Wave_Parser_END_REACHED         (1u << 2)       // mk: reached <eof> or <eox> (not used)
#define Wave_Parser_INTERRUPT           (1u << 3)       // mk: the first parser who reaches <eof> or <eox> and errors, it will throw this flag, so other parsers won't throw the same error again and immediately stops the execution

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

#define Wave_Parser_STOP_EXPR_AT_EQ     (1u << 0)

// mk: parse individual expressions, like `var = 2 + 2 * (var - 1 as f32)`, ends with semicolon or `<eof>`, `<eox>`, `)`, `]`, `{`, `}`
struct Wave_ParseResult Wave_Parser_parse_expr(
    const struct Wave_ParseParams params,
    const size_t vector_indexing_offset,
    const uint8_t flags // mk: if it should stop parsing whenever there's a `=` to support `let x: bool | string = true;`
);

struct Wave_ParseResult Wave_Parser_parse_fn(
    const struct Wave_ParseParams params,
    const size_t vector_indexing_offset
);

#define Wave_Parser_NO_VARIADIC     (1u << 0)
#define Wave_Parser_NO_DEFAULT      (1u << 1)
#define Wave_Parser_NO_SELF         (1u << 2)

struct Wave_SubparseResult Wave_Parser_parse_decl_list(
    const struct Wave_ParseParams params,
    const size_t vector_indexing_offset,
    struct Wave_AstNode *const node,
    const uint8_t flags,
    const enum Wave_TokenType terminator,
    size_t *const counter
);

#define Wave_Parser_NO_ASSIGN       (1u << 0)

struct Wave_ParseResult Wave_Parser_parse_decl(
    const struct Wave_ParseParams params,
    const size_t vector_indexing_offset,
    const uint8_t flags
);

// mk: the tuple include parenthesis, supported `()`, `[]`, `{}`
struct Wave_ParseResult Wave_Parser_parse_tuple(
    const struct Wave_ParseParams params,
    const size_t vector_indexing_offset
);

struct Wave_ParseScanner {
    struct Wave_Context *const context;
    const struct Wave_TokenVector vector;
    const size_t vector_indexing_offset;
    size_t *const shift;
};

const struct Wave_Token *Wave_ParseScanner_peek(
    const struct Wave_ParseScanner scanner
);

const struct Wave_Token *Wave_ParseScanner_next(
    const struct Wave_ParseScanner scanner
);

size_t Wave_Parser_binding_power_of(const enum Wave_TokenType token_type);

#endif
