#ifndef WAVE__TEST_H
#define WAVE__TEST_H

#include <wave/context.h>
#include <stdbool.h>
#include <wave/char-location.h>

struct Wave_CharLocation Wave_Test_no_loc(void);
bool Wave_Test_escape_sequences(struct Wave_Context *const context);
bool Wave_Test_number_literals(struct Wave_Context *const context);
bool Wave_Test_string_literals(struct Wave_Context *const context);
bool Wave_Test_symbols(struct Wave_Context *const context);

#endif
