#include <wave/ast.h>
#include <wave/ast_meta.h>
#include <wave/log.h>
#include <wave/string.h>
#include <wave/token.h>
#include <stddef.h>
#include <stdint.h>
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

#define SUBPARSER_PUSH_OR_CRITICAL(onode, branch_node, or_else_message) \
    if (!Wave_Ast_push_branch( \
        params.context, \
        (onode), \
        (branch_node)\
    )) { \
        result.flags |= Wave_Parser_CRITICAL_ERROR; \
        PARSER_LOG( \
            Wave_LogType_Error, \
            Wave_LogSignalType_InvalidAllocation, \
            Wave_String_from(params.context, (or_else_message)) \
        ); \
        return result; \
    }

#define PARSER_PUSH_OR_CRITICAL(branch_node, or_else_message) \
    SUBPARSER_PUSH_OR_CRITICAL(&result.node, (branch_node), (or_else_message))

#define PARSER_INIT \
    struct Wave_ParseResult result = Wave_ParseResult_null; \
    const struct Wave_ParseScanner scanner = { \
        .context = params.context, \
        .vector = params.vector, \
        .vector_indexing_offset = vector_indexing_offset, \
        .shift = &result.consumed_token_count \
    };

#define SUBPARSER_INIT \
    struct Wave_SubparseResult result = Wave_SubparseResult_null; \
    const struct Wave_ParseScanner scanner = { \
        .context = params.context, \
        .vector = params.vector, \
        .vector_indexing_offset = vector_indexing_offset, \
        .shift = &result.consumed_token_count \
    };

#define PARSER_RES_FLAG_PROPAG(parse_result) \
    result.flags |= (parse_result).flags; \
    if (result.flags & Wave_Parser_CRITICAL_ERROR) { \
        return result; \
    } \
    if (result.flags & Wave_Parser_INTERRUPT) { \
        return result; \
    }

struct Wave_ParseResult Wave_Parser_parse(
    const struct Wave_ParseParams params,
    const size_t vector_indexing_offset
) {
    struct Wave_ParseResult result = Wave_ParseResult_null;
    const struct Wave_ParseScanner scanner = {
        .context = params.context,
        .vector = params.vector,
        .vector_indexing_offset = vector_indexing_offset,
        .shift = &result.consumed_token_count
    };

    while (true) {
        const struct Wave_Token *const token = Wave_ParseScanner_next(scanner);

        if (token->type == Wave_TokenType_SEMICOLON) {
            continue;
        } else if (
            token->type == Wave_TokenType_EOF ||
            token->type == Wave_TokenType_EOX ||
            token->type == Wave_TokenType_RBRACE
        ) { // mk: end
            break;
        } else if (token->category == Wave_TokenCat_Keyword) {
            if (token->type == Wave_TokenType_FN) {
                const struct Wave_ParseResult fn_parse_result = Wave_Parser_parse_fn(
                    params,
                    vector_indexing_offset + result.consumed_token_count
                );
            } else if (token->type == Wave_TokenType_STRUCT) {

            } else if (token->type == Wave_TokenType_LET) {
                
            } else if (token->type == Wave_TokenType_CONST) {
                
            }
        } else {
            const struct Wave_ParseResult expr_parse_result = Wave_Parser_parse_expr(
                params,
                vector_indexing_offset + result.consumed_token_count,
                0
            );
            result.consumed_token_count += expr_parse_result.consumed_token_count;
            PARSER_RES_FLAG_PROPAG(expr_parse_result);
        }
    }

    return result;
}

struct Wave_ParseResult Wave_Parser_parse_expr(
    const struct Wave_ParseParams params,
    const size_t vector_indexing_offset,
    const uint8_t flags
) {
    struct Wave_ParseResult result = Wave_ParseResult_null;
    const struct Wave_ParseScanner scanner = {
        .context = params.context,
        .vector = params.vector,
        .vector_indexing_offset = vector_indexing_offset,
        .shift = &result.consumed_token_count
    };
    //size_t *const i = scanner.shift;

    while (true) {
        const struct Wave_Token *const token = Wave_ParseScanner_next(scanner);

        if (token->type == Wave_TokenType_EQ && (flags & Wave_Parser_STOP_EXPR_AT_EQ)) {
            result.consumed_token_count--;
            break;
        }

        // mk: handle every single token that can be in an expression
        if (token->category == Wave_TokenCat_Operator) { // mk: operators

        } else if (token->type == Wave_TokenType_LIT_NUM) { // mk: numbers

        } else if (token->type == Wave_TokenType_ID) { // mk: variable or function call

        } else if (token->type == Wave_TokenType_LIT_STR) { // mk: variable or function call

        } else if (token->type == Wave_TokenType_LIT_FSTR) { // mk: variable or function call

        } else if (token->category == Wave_TokenCat_Keyword) {
            if (token->type == Wave_TokenType_AS) { // mk: `let a = 2 as f64;`

            } else if (token->type == Wave_TokenType_TYPEOF) { // mk: `let d: type = i32 | typeof(a)`

            } else if (token->type == Wave_TokenType_FN) { // mk: `let f: fn (i32) = fn (x: i32) { x + 5 };`
                
            } else if (token->type == Wave_TokenType_STRUCT) { // mk: `let t: type = i32 | struct { x: i32 };`
                
            } else if (token->type == Wave_TokenType_IF) { // mk: `let x = 2 + if true { 2 } else { 0 };`
                
            } else if (token->type == Wave_TokenType_MATCH) {
                
            } else if (token->type == Wave_TokenType_FOR) {
                
            } else if (token->type == Wave_TokenType_SELF) { // mk: `self.x = 2 + self.y;`
                
            } else if (
                token->type == Wave_TokenType_BOOL_FALSE ||
                token->type == Wave_TokenType_BOOL_TRUE
            ) {
                
            } else switch (token->type) { // mk: handle all types in expr
                case Wave_TokenType_TY_BOOL:
                case Wave_TokenType_TY_I8:
                case Wave_TokenType_TY_I16:
                case Wave_TokenType_TY_I32:
                case Wave_TokenType_TY_I64:
                //case Wave_TokenType_TY_I128:
                case Wave_TokenType_TY_U8:
                case Wave_TokenType_TY_U16:
                case Wave_TokenType_TY_U32:
                case Wave_TokenType_TY_U64:
                //case Wave_TokenType_TY_U128:
                case Wave_TokenType_TY_F32:
                case Wave_TokenType_TY_F64:
                case Wave_TokenType_TY_V0:
                case Wave_TokenType_TY_STRING:
                case Wave_TokenType_TYPE:
                    break;
                default:
                    // mk: error
                    break;
            }
        } else {
            // mk: error
        }


        const struct Wave_Token *const next = Wave_ParseScanner_peek(scanner);

        if (next->type == Wave_TokenType_EOF    ||
            next->type == Wave_TokenType_EOX    ||
            next->type == Wave_TokenType_COMMA  ||
            next->type == Wave_TokenType_RPAREN ||
            next->type == Wave_TokenType_RBRACK ||
            next->type == Wave_TokenType_RBRACE ||
            next->type == Wave_TokenType_SEMICOLON
        ) { // mk: mark the end of the expression
            break;
        }
    }

    return result;
}

struct Wave_ParseResult Wave_Parser_parse_fn(
    const struct Wave_ParseParams params,
    const size_t vector_indexing_offset
) {
    PARSER_INIT;
    const struct Wave_Token *token = Wave_ParseScanner_next(scanner);
    
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

    token = Wave_ParseScanner_next(scanner);

    if (token->type == Wave_TokenType_LPAREN) { // mk: anon function `fn() -> i32`
        fn_data.flags |= Wave_AstNodeMeta_Fn_ANON;
    } else if (token->type == Wave_TokenType_ID) { // mk: `fn func()`
        token = Wave_ParseScanner_next(scanner);
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

struct Wave_ParseResult Wave_Parser_parse_decl(
    const struct Wave_ParseParams params,
    const size_t vector_indexing_offset,
    const uint8_t flags
) {
    PARSER_INIT;

    const struct Wave_Token *token = Wave_ParseScanner_next(scanner);

    if (token->type != Wave_TokenType_ID) {
        PARSER_LOG(
            Wave_LogType_Error,
            Wave_LogSignalType_UnexpectedToken,
            Wave_String_from(params.context, "identifier expected at the beginning of declaration")
        );
        return result;
    }

    result.node = Wave_Ast_new_node(
        params.context,
        vector_indexing_offset + result.consumed_token_count,
        2
    );
    if (result.node.branches == NULL) {
        result.flags |= Wave_Parser_CRITICAL_ERROR;
        PARSER_LOG(
            Wave_LogType_Error,
            Wave_LogSignalType_InvalidAllocation,
            Wave_String_from(params.context, "failed to allocate memory for decl")
        );
        return result;
    }

    if (Wave_ParseScanner_peek(scanner)->type == Wave_TokenType_COLON) {
        result.consumed_token_count++;
    } else {
        goto jmp_parse_decl_assignment; // mk: i think it's the cleanest way to do it, using `goto` isn't inherently bad, there's just very few cases on where to do this
    }

    token = Wave_ParseScanner_next(scanner);

    // mk: `x: <expr>;` it's actually an expression that must return a type, but this expr should stop at `=`
    const struct Wave_ParseResult expr_parse_result = Wave_Parser_parse_expr(
        params,
        vector_indexing_offset + result.consumed_token_count,
        Wave_Parser_STOP_EXPR_AT_EQ
    );
    result.consumed_token_count += expr_parse_result.consumed_token_count;
    PARSER_RES_FLAG_PROPAG(expr_parse_result);

    // mk: not checked if pushed succesfully because we initially reserved 2 places, so it should be guaranteed to be executed correctly
    Wave_Ast_push_branch(params.context, &result.node, expr_parse_result.node);

    jmp_parse_decl_assignment:

    if (!(flags & Wave_Parser_NO_ASSIGN) && Wave_ParseScanner_peek(scanner)->type == Wave_TokenType_EQ) {
        result.consumed_token_count++;
    } else {
        return result;
    }

    // mk: if the flow reached here, that means the statement also makes an assignmend `let x: i32 = 7` or `let x = 7`
    const struct Wave_ParseResult ass_expr_parse_result = Wave_Parser_parse_expr(
        params,
        vector_indexing_offset + result.consumed_token_count,
        Wave_Parser_STOP_EXPR_AT_EQ
    );
    result.consumed_token_count += ass_expr_parse_result.consumed_token_count;
    PARSER_RES_FLAG_PROPAG(ass_expr_parse_result);

    PARSER_PUSH_OR_CRITICAL(
        ass_expr_parse_result.node,
        "failed to allocate memory for decl ast node pushback"
    );

    return result;
}

struct Wave_SubparseResult Wave_Parser_parse_decl_list(
    const struct Wave_ParseParams params,
    const size_t vector_indexing_offset,
    struct Wave_AstNode *const node,
    const uint8_t flags,
    const enum Wave_TokenType terminator,
    size_t *const counter
) {
    SUBPARSER_INIT;
    
    while (true) {
        const struct Wave_Token *const token = Wave_ParseScanner_next(scanner);

        if (token->type == terminator) {
            result.consumed_token_count--;
            break;
        }

        if (token->type == Wave_TokenType_EOF) {
            result.flags |= Wave_Parser_INTERRUPT;
            PARSER_LOG(
                Wave_LogType_Error,
                Wave_LogSignalType_UnexpectedEndOfFile,
                Wave_String_from(params.context, "identifier or closing pattern for declaration list expected but found `<eof>`")
            );
            return result;
        }
        if (token->type == Wave_TokenType_EOX) {
            result.flags |= Wave_Parser_INTERRUPT;
            PARSER_LOG(
                Wave_LogType_Error,
                Wave_LogSignalType_UnexpectedEndOfExecution,
                Wave_String_from(params.context, "identifier or closing pattern for declaration list expected but found `<eox>`")
            );
            return result;
        }
        
        if (token->type == Wave_TokenType_COMMA) {
            PARSER_LOG(
                Wave_LogType_Error,
                Wave_LogSignalType_UnexpectedToken,
                Wave_String_from(params.context, "empty definition elements are not allowed")
            );
            continue;
        }

        if (token->type == Wave_TokenType_ID) {

        } else if (token->type == Wave_TokenType_SELF) {
            if (flags & Wave_Parser_NO_SELF) {
                PARSER_LOG(
                    Wave_LogType_Error,
                    Wave_LogSignalType_UnexpectedToken,
                    Wave_String_from(params.context, "`self` is not allowed here")
                );
            } else if (*counter == 0) {
                PARSER_LOG(
                    Wave_LogType_Error,
                    Wave_LogSignalType_UnexpectedToken,
                    Wave_String_from(params.context, "`self` can be the first member only")
                );
            } else {
                const struct Wave_ParseResult decl_parse_result = Wave_Parser_parse_decl(
                    params,
                    vector_indexing_offset + result.consumed_token_count,
                    (flags & Wave_Parser_NO_DEFAULT) ? Wave_Parser_NO_ASSIGN : 0
                );
                result.consumed_token_count += decl_parse_result.consumed_token_count;
                PARSER_RES_FLAG_PROPAG(decl_parse_result);
                SUBPARSER_PUSH_OR_CRITICAL(
                    node,
                    decl_parse_result.node,
                    "an error occurred while pushing ast node for member list"
                );
            }
        } else {
            PARSER_LOG(
                Wave_LogType_Error,
                Wave_LogSignalType_UnexpectedToken,
                Wave_String_from(params.context, "definition list must begin with an identifier")
            );
            return result;
        }

        if (Wave_ParseScanner_peek(scanner)->type == Wave_TokenType_COMMA) {
            result.consumed_token_count++;
        }
        (*counter)++;
    }

    return result;
}

// mk: difference between a `tuple` and `decl_list`? well, they're similar but `decl_list` is embedded directly in another parser and doesn't care about boundaries `x: i32, y`, meanwhile `tuple` is one independent block of expressions `(x as f32, y, i32)`
struct Wave_ParseResult Wave_Parser_parse_tuple(
    const struct Wave_ParseParams params,
    const size_t vector_indexing_offset
) {
    PARSER_INIT;
    const struct Wave_Token *token = Wave_ParseScanner_next(scanner);

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

    while (true) {
        token = Wave_ParseScanner_next(scanner);
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
            const struct Wave_ParseResult expr_parse_result = Wave_Parser_parse_expr(
                params,
                vector_indexing_offset + result.consumed_token_count,
                0
            );
            result.flags = expr_parse_result.flags;
            if (result.flags & Wave_Parser_CRITICAL_ERROR) {
                return result;
            }

            PARSER_PUSH_OR_CRITICAL(
                expr_parse_result.node,
                "an error occurred during memory allocation of ast branch nodes for tuple"
            );

            if (Wave_ParseScanner_peek(scanner)->type == Wave_TokenType_COMMA) {
                result.consumed_token_count++;
            }
        }
    }

    return result;
}

inline const struct Wave_Token *Wave_ParseScanner_peek(
    const struct Wave_ParseScanner scanner
) {
    return &scanner.vector.tokens[scanner.vector_indexing_offset + (*scanner.shift)];
}

inline const struct Wave_Token *Wave_ParseScanner_next(
    const struct Wave_ParseScanner scanner
) {
    const struct Wave_Token *const token = &scanner.vector.tokens[scanner.vector_indexing_offset + (*scanner.shift)];
    (*scanner.shift)++;
    return token;
}

inline size_t Wave_Parser_binding_power_of(const enum Wave_TokenType token_type) {
    switch (token_type) {
        case Wave_TokenType_AWA:
            return 9000;
        case Wave_TokenType_MUL:
        case Wave_TokenType_DIV:
            return 120;
        case Wave_TokenType_ADD:
        case Wave_TokenType_MIN:
            return 110;
        default:
            return 0;
    }
}
