#ifndef WAVE_PARSER_H
#define WAVE_PARSER_H

#include <wave/context.h>
#include <wave/token.h>
#include <wave/ast.h>

struct Wave_AstNode Wave_Parser_parse(const struct Wave_TokenVector vector);

#endif