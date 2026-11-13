#ifndef WAVE_AST_META_H
#define WAVE_AST_META_H

#include <stddef.h>
#include <stdint.h>

// mk: meta: fn
#define Wave_AstNodeMeta_Fn_ANON        (1u << 0)   // mk: the function doesn't have an identifier
#define Wave_AstNodeMeta_Fn_RET_TYPED   (1u << 1)   // mk: the function has a return type specified `fn f() -> i32`
#define Wave_AstNodeMeta_Fn_VARIADIC    (1u << 2)   // mk: function has variadic param `fn f(x, list...)`

// mk: with meta data we can determine how many child nodes are consumed for each part, for example a function with 2 params + 1 variadic params has diff layout compared to function with 5 params

struct Wave_AstNodeMeta_Fn {
    size_t param_list_count;    // mk: number of params excluding variadic
    uint8_t flags;
};
// mk: the very last child of the function node is the block of code which is the body
// mk: also if for example the function `fn f() -> i32` has a return type, the child node of the return part should be the index of the very first token of `-> i32`, in this case, it's an arrow, this also applies to other things, the very first token


#define Wave_AstNodeMeta_Interrupt_HAS_VALUE    (1u << 0) // mk: distinguish between `return;` and `return 2;` or `break;` and `break 2;`, if flag is set, the interrupt node will have one child node, which is the returning value or expression

// mk: interrupts are returns, breaks, or even the trailing returning expressions. they all stop execution and occasionally return a value
struct Wave_AstNodeMeta_Interrupt {
    uint8_t flags;
};

#define Wave_AstNodeMeta_Decl_IS_TYPED          (1u << 0)
#define Wave_AstNodeMeta_Decl_HAS_VALUE         (1u << 1)

struct Wave_AstNodeMeta_Decl {
    uint8_t flags;
};

#endif