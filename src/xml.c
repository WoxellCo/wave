#include "wave/lexer.h"
#include "wave/string.h"
#include <stdbool.h>
#include <stddef.h>
#include <wave/xml.h>
#include <stdlib.h>

void Wave_Xml_TagType_delete(struct Wave_Xml_TagType* const tag_type) {
	if (tag_type == NULL)
		return;
	Wave_String_delete(&tag_type->name);
}

void Wave_Xml_Tag_delete(struct Wave_Xml_Tag* const tag) {
	if (tag == NULL)
		return;
	Wave_Xml_TagType_delete(&tag->type);
}

/// xml lexers ///
static inline bool Wave_Xml_Prop_double_quoted_text_delimeter_predicate(
    const struct Wave_Lexer_DelimiterPredicateParams params
) {
    return params.source[params.n] == '"';
}

static inline bool Wave_Xml_Prop_single_quoted_text_delimeter_predicate(
    const struct Wave_Lexer_DelimiterPredicateParams params
) {
    return params.source[params.n] == '\'';
}

static inline bool Wave_Xml_Prop_expr_delimeter_predicate(
    const struct Wave_Lexer_DelimiterPredicateParams params
) {
    return params.source[params.n] == '}' && Wave_Token_do_counters_match(params.token_counters);
}

// mk: individual xml prop key/value constructors & destructors will be `static` becase they aren't that needed outside this context
static bool Wave_Xml_Prop_lex_key(
	struct Wave_String *wave_restrict const output,
	struct Wave_CharLocation *wave_restrict const loc,
	const char *restrict const source,
	const size_t offset,
	#ifndef _MSC_VER
	const Wave_Lexer_DelimiterPredicate delimiter_predicates[static wave_restrict const 1]
	#else
	const Wave_Lexer_DelimiterPredicate *wave_restrict const delimiter_predicates
	#endif
) {
	const char *src = source + offset;
}

static bool Wave_Xml_Text_lex(
	struct Wave_Xml_Text *wave_restrict const output,
	struct Wave_CharLocation *wave_restrict const loc,
	const char *wave_restrict const source,
	const size_t offset,
	#ifndef _MSC_VER
	const Wave_Lexer_DelimiterPredicate delimiter_predicates[wave_restrict static const 1]
	#else
	const Wave_Lexer_DelimiterPredicate *wave_restrict const delimiter_predicates
	#endif
) {
	struct Wave_Xml_Text text = {
		.data = {
			.string_data = Wave_String_null,
			.esc_vector = {0}
		},
		.loc = {0}
	};
	while (true) {

	}

	return true;
}

static bool Wave_Xml_Prop_lex_value(
	struct Wave_Xml_PropValue *wave_restrict const output,
	struct Wave_CharLocation *wave_restrict const loc,
	const char *wave_restrict const source,
	const size_t offset,
	#ifndef _MSC_VER
	const Wave_Lexer_DelimiterPredicate delimiter_predicates[wave_restrict static const 1]
	#else
	const Wave_Lexer_DelimiterPredicate *wave_restrict const delimiter_predicates
	#endif
) {
	const char *src = source + offset;
	if (src[0] == '"') {
		output->boundaries = src[0];
		Wave_Lexer_DelimiterPredicate *dp = Wave_Lexer_dp_new_vector_with_concatenation(
			Wave_Lexer_dp_array(Wave_Xml_Prop_double_quoted_text_delimeter_predicate),
			delimiter_predicates
		);
		if (dp == NULL || !Wave_Xml_Text_lex(&output->data.text, loc, source, offset + 1, delimiter_predicates)) {
			Wave_Lexer_dp_delete_vector(&dp);
			return false;
		}
		Wave_Lexer_dp_delete_vector(&dp);
	} else if (src[0] == '\'') {
		output->boundaries = src[0];
		Wave_Lexer_DelimiterPredicate *dp = Wave_Lexer_dp_new_vector_with_concatenation(
			Wave_Lexer_dp_array(Wave_Xml_Prop_double_quoted_text_delimeter_predicate),
			delimiter_predicates
		);
		if (dp == NULL || !Wave_Xml_Text_lex(&output->data.text, loc, source, offset + 1, delimiter_predicates)) {
			Wave_Lexer_dp_delete_vector(&dp);
			return false;
		}
		Wave_Lexer_dp_delete_vector(&dp);
	} else if (src[0] == '{') {
		output->boundaries = '$';
		Wave_Lexer_DelimiterPredicate *dp = Wave_Lexer_dp_new_vector_with_concatenation(
			Wave_Lexer_dp_array(Wave_Xml_Prop_expr_delimeter_predicate),
			delimiter_predicates
		);
		if (dp == NULL || !Wave_Xml_Text_lex(&output->data.text, loc, source, offset + 1, delimiter_predicates)) {
			Wave_Lexer_dp_delete_vector(&dp);
			return false;
		}
		Wave_Lexer_dp_delete_vector(&dp);
	} else {
		return false;
	}
}

bool Wave_Xml_Prop_lex(
	struct Wave_Xml_Prop *wave_restrict const prop,
	struct Wave_CharLocation *wave_restrict const loc,
	const char* wave_restrict const source,
	const size_t offset,
	#ifndef _MSC_VER
	const Wave_Lexer_DelimiterPredicate delimiter_predicates[wave_restrict const]
	#else
	const Wave_Lexer_DelimiterPredicate *wave_restrict const delimiter_predicates
	#endif
) {

	return false;
}

static void Wave_Xml_PropValue_delete(struct Wave_Xml_PropValue *prop_value) {
	switch (prop_value->boundaries) {
		case '$':
			Wave_Xml_Expr_delete(&prop_value->data.expr);
			break;
		case '"':
		case '\'':
			Wave_Xml_Text_delete(&prop_value->data.text);
			break;
		default:
			break;
	}
}

void Wave_Xml_Prop_delete(struct Wave_Xml_Prop *const prop) {
	Wave_String_delete(&prop->key);
	Wave_Xml_PropValue_delete(&prop->value);
}

void Wave_Xml_delete(struct Wave_Xml* const element) {
	if (element == NULL)
		return;
	Wave_Xml_TagType_delete(&element->start_tag.type);
	if (Wave_Xml_is_self_closing(*element)) {
		Wave_Xml_TagType_delete(&element->end_tag.tag.type);
	}
	Wave_XmlVector_delete(&element->children);
}

bool Wave_XmlVector_push(struct Wave_XmlVector* const vector, const struct Wave_XmlNode node) {
	if (vector->nodes == NULL) {
		struct Wave_XmlNode* const block = (struct Wave_XmlNode*)malloc(4 * sizeof(struct Wave_XmlNode));
		if (block == NULL)
			return false;
		vector->nodes = block;
		vector->capacity = 4;
		vector->size = 0;
	}
	else if (vector->size >= vector->capacity) {
		const size_t cap = 2 * vector->capacity;
		struct Wave_XmlNode* const block = (struct Wave_XmlNode*)malloc(cap * sizeof(struct Wave_XmlNode));
		if (block == NULL)
			return false;
		vector->nodes = block;
		vector->capacity = cap;
	}

	vector->nodes[vector->size] = node;
	vector->size++;
}

void Wave_XmlVector_delete(struct Wave_XmlVector* const vector) {
	if (vector == NULL || vector->nodes == NULL || vector->capacity == 0)
		return;

	for (size_t i = 0; i < vector->size; i++) {
		struct Wave_XmlNode* const node = vector->nodes + i;
		switch (node->type) {
		case Wave_XmlNodeType_Element:
			break;
		case Wave_XmlNodeType_Text:
			Wave_Xml_Text_delete(&node->data.text);
			break;
		case Wave_XmlNodeType_Expr:
			Wave_Xml_Expr_delete(&node->data.expr);
			break;
		default:
			break;
		}
	}

	free(vector->nodes);
	vector->capacity = 0;
	vector->size = 0;
}

// mk: wave::xml::text.delete()
void Wave_Xml_Text_delete(struct Wave_Xml_Text* const xml_text) {
	if (xml_text == NULL)
		return;

	Wave_TokenString_EscVector_delete(&xml_text->data.esc_vector);
	Wave_String_delete(&xml_text->data.string_data);
}

// mk: wave::xml::expr.delete()
void Wave_Xml_Expr_delete(struct Wave_Xml_Expr* const xml_expr) {
	if (xml_expr == NULL)
		return;

	Wave_TokenVector_delete(&xml_expr->data);
}
