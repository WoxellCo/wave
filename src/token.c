#include <wave/token.h>

bool Wave_Token_do_counters_match(const struct Wave_TokenCounters *const counters) {
    return
        counters->lparen == counters->rparen &&
        counters->lbrace == counters->rbrace &&
        counters->lbrack == counters->rbrack;
}