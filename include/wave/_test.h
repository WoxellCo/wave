#ifndef WAVE__TEST_H
#define WAVE__TEST_H

#include <stdbool.h>
#include <wave/char-location.h>

struct Wave_CharLocation Wave_Test_no_loc(void);
bool Wave_Test_escape_sequences(void);
bool Wave_Test_number_literals(void);

#endif
