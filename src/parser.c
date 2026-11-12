#include "wave/ast.h"
#include "wave/ast_meta.h"
#include "wave/log.h"
#include "wave/string.h"
#include "wave/token.h"
#include <stddef.h>
#include <stdio.h>
#include <wave/parser.h>

#define PARSER_LOG(log_type, signal_type, message_string) \
    if (!Wave_LogVector_push( \
        params.context, \
        params.messages, \
        &(struct Wave_Log){ \
            .type = (log_type), \
            .signal.type = (signal_type), \
            .message = (message_string) \
        } \
    )) { \
        result.flags |= Wave_Parser_LOG_ERROR; \
    }

#define PARSER_PUSH_OR_CRITICAL(branch_node, or_else_message) \
    if (!Wave_Ast_push_branch( \
        params.context, \
        &result.node, \
        expr_parse_result.node \
    )) { \
        result.flags |= Wave_Parser_CRITICAL_ERROR; \
        PARSER_LOG( \
            Wave_LogType_Error, \
            Wave_LogSignalType_InvalidAllocation, \
            Wave_String_from(params.context, (or_else_message)) \
        ); \
        return result; \
    }

// mk: stream-based `i`
struct Wave_ParseResult Wave_Parser_parse(
    const struct Wave_ParseParams params,
    const size_t vector_indexing_offset
) {
    size_t i;
    for (i = vector_indexing_offset; i < params.vector.size; i++) {
        const struct Wave_Token token = params.vector.tokens[i];

        if (token.category == Wave_TokenCat_Keyword) {
            if (token.type == Wave_TokenType_FN) {
                Wave_Parser_parse_fn(params, i - 1);
            }
        }
    }
}

struct Wave_ParseResult Wave_Parser_parse_expr(
    const struct Wave_ParseParams params,
    const size_t vector_indexing_offset
) {
    struct Wave_ParseResult result = Wave_ParseResult_null;
    struct Wave_ParseScannerParams scanner = {
        .context = params.context,
        .vector = params.vector,
        .vector_indexing_offset = vector_indexing_offset,
        .shift = &result.consumed_token_count
    };
    size_t *const i = scanner.shift;

    while (true) {
        
    }
}

// mk: zero-based `i`
struct Wave_ParseResult Wave_Parser_parse_fn(
    const struct Wave_ParseParams params,
    const size_t vector_indexing_offset
) {
    const struct Wave_Token *token;
    struct Wave_ParseResult result = Wave_ParseResult_null;
    
    token = params.vector.tokens + vector_indexing_offset;
    
    if (token->type != Wave_TokenType_FN) {
        if (!Wave_LogVector_push(
            params.context, params.messages,
            &(struct Wave_Log){
                .type = Wave_LogType_Error,
                .signal.type = Wave_LogSignalType_UnexpectedToken,
                .message = Wave_String_null
            }
        )) {
            result.flags |= Wave_Parser_LOG_ERROR;
        }
        return result;
    }

    struct Wave_AstNodeMeta_Fn fn_data = {0};

    size_t *const i = &result.consumed_token_count;
    *i = 1;

    if (token->type == Wave_TokenType_LPAREN) { // mk: anon function `fn() -> i32`
        fn_data.flags |= Wave_AstNodeMeta_Fn_ANON;
    } else if (token->type == Wave_TokenType_ID) { // mk: `fn func()`
        (*i)++;
        token = params.vector.tokens + *i + vector_indexing_offset;
        if (token->type != Wave_TokenType_LPAREN) {
            if (!Wave_LogVector_push(
                params.context,
                params.messages,
                &(struct Wave_Log){
                    .type = Wave_LogType_Error,
                    .signal.type = Wave_LogSignalType_UnexpectedToken,
                    .message = Wave_String_from(params.context, "expected a `(` after function identifier")
                }
            )) {
                result.flags |= Wave_Parser_LOG_ERROR;
            }
            return result;
        }
    } else { // mk: error
        if (!Wave_LogVector_push(
            params.context,
            params.messages,
            &(struct Wave_Log){
                .type = Wave_LogType_Error,
                .signal.type = Wave_LogSignalType_UnexpectedToken,
                .message = Wave_String_from(params.context, "expected an identifier or a `(` after `fn`")
            }
        )) {
            result.flags |= Wave_Parser_LOG_ERROR;
        }
        return result;
    }

    result.node = Wave_Ast_new_node(
        params.context,
        vector_indexing_offset,
        4
    );

    if (result.node.branches == NULL) {
        if (!Wave_LogVector_push(
            params.context,
            params.messages,
            &(struct Wave_Log){
                .type = Wave_LogType_Error,
                .signal.type = Wave_LogSignalType_InvalidAllocation,
                .message = Wave_String_from(params.context, "an error occurred during allocation of ast branch nodes for a function")
            }
        )) {
            result.flags |= Wave_Parser_CRITICAL_ERROR | Wave_Parser_LOG_ERROR;
            return result;
        }
    }

    // mk: here we parse the argument list
    

    return result;
}

struct Wave_SubparseResult Wave_Parser_parse_member_list(
    const struct Wave_ParseParams params,
    const size_t vector_indexing_offset,
    struct Wave_AstNode *const node,
    const uint8_t flags,
    const enum Wave_TokenType terminator,
    size_t *const counter
) {
    struct Wave_SubparseResult result = Wave_SubparseResult_null;
    size_t *const i = &result.consumed_token_count;
    for (; *i < params.vector.size - vector_indexing_offset; (*i)++) {
        const struct Wave_Token *const token = params.vector.tokens + *i + vector_indexing_offset;

        if (token->type == Wave_TokenType_RPAREN) {
            break;
        }
        
        if (token->type != Wave_TokenType_ID) {
            if (!Wave_LogVector_push(
                params.context,
                params.messages,
                &(struct Wave_Log){
                    .type = Wave_LogType_Error,
                    .signal.type = Wave_LogSignalType_UnexpectedToken,
                    .message = Wave_String_from(params.context, "argument definition must begin with an identifier")
                }
            )) {
                result.flags |= Wave_Parser_LOG_ERROR;
            }
            return result;
        }

        (*counter)++;
    }

    return result;
}

// mk: difference between a `tuple` and `member_list`? well, they're similar but `member_list` is embedded directly in another parser and doesn't care about boundaries `x: i32, y`, meanwhile `tuple` is one independent block of expressions `(x as f32, y, i32)`
struct Wave_ParseResult Wave_Parser_parse_tuple(
    const struct Wave_ParseParams params,
    const size_t vector_indexing_offset
) {
    struct Wave_ParseResult result = Wave_ParseResult_null;
    const struct Wave_Token *token = &params.vector.tokens[vector_indexing_offset];
    if (!(
        token->type == Wave_TokenType_LPAREN ||
        token->type == Wave_TokenType_LBRACK ||
        token->type == Wave_TokenType_LBRACE
    )) { // mk: a tuple must begin with one of these: `(`, `[`, `{`
        PARSER_LOG(
            Wave_LogType_Error,
            Wave_LogSignalType_UnexpectedToken,
            Wave_String_from(params.context, "a tuple must be delimited by `()` or `[]` or `{}`")
        );
        return result;
    }

    result.node = Wave_Ast_new_node(
        params.context,
        vector_indexing_offset,
        2
    );

    if (result.node.branches == NULL) {
        result.flags |= Wave_Parser_CRITICAL_ERROR;
        PARSER_LOG(
            Wave_LogType_Error,
            Wave_LogSignalType_InvalidAllocation,
            Wave_String_from(params.context, "an error occurred during memory allocation of ast branch nodes for tuple")
        );
        return result;
    }

    result.consumed_token_count = 1;
    size_t *const i = &result.consumed_token_count;
    for (; *i < params.vector.size - vector_indexing_offset; (*i)++) {
        token = &params.vector.tokens[vector_indexing_offset + *i];
        if (token->type == Wave_TokenType_COMMA) { // mk: error: invalid comma `(,)`
            PARSER_LOG(
                Wave_LogType_Error,
                Wave_LogSignalType_UnexpectedToken,
                Wave_String_from(params.context, "unexpected comma in tuple")
            );
        } else if (
            token->type == Wave_TokenType_RPAREN ||
            token->type == Wave_TokenType_RBRACK ||
            token->type == Wave_TokenType_RBRACE
        ) { // mk: closing tuple, needs to check if the closing token matches with the starting token
            const struct Wave_Token *const opening = &params.vector.tokens[vector_indexing_offset];
            if ((opening->type == Wave_TokenType_LPAREN
                && token->type == Wave_TokenType_RPAREN) ||
                (opening->type == Wave_TokenType_LBRACK
                && token->type == Wave_TokenType_RBRACK) ||
                (opening->type == Wave_TokenType_LBRACE
                && token->type == Wave_TokenType_RBRACE)) {
                return result;
            }
            char expected_char = opening->type == Wave_TokenType_LPAREN ? ')'
                               : opening->type == Wave_TokenType_LBRACK ? ']'
                               :                                          '}';
            char msg[64];
            sprintf(msg, "unmatched closing token for tuple, did you mean `%c`?", expected_char);
            PARSER_LOG(
                Wave_LogType_Error,
                Wave_LogSignalType_UnexpectedToken,
                Wave_String_from(params.context, msg)
            );
        } else {
            const struct Wave_ParseResult expr_parse_result = Wave_Parser_parse_expr(params, vector_indexing_offset + *i);
            result.flags = expr_parse_result.flags;
            if (result.flags & Wave_Parser_CRITICAL_ERROR) {
                return result;
            }

            PARSER_PUSH_OR_CRITICAL(
                expr_parse_result.node,
                "an error occurred during memory allocation of ast branch nodes for tuple"
            );

            if (params.vector.tokens[vector_indexing_offset + *i + 1].type == Wave_TokenType_COMMA) {
                (*i)++;
            }
        }
    }

    return result;
}

inline const struct Wave_Token *Wave_Parser_peek(
    struct Wave_ParseScannerParams params
) {
    return &params.vector.tokens[params.vector_indexing_offset + (*params.shift)];
}

inline const struct Wave_Token *Wave_Parser_next(
    struct Wave_ParseScannerParams params
) {
    const struct Wave_Token *const token = &params.vector.tokens[params.vector_indexing_offset + (*params.shift)];
    (*params.shift)++;
    return token;
}
