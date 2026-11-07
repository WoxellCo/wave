#include "wave/char-location.h"
#include "wave/def.h"
#include "wave/lexer.h"
#include "wave/string.h"
#include "wave/token-string.h"
#include "wave/unicode.h"
#include <stdbool.h>
#include <stddef.h>
#include <wave/xml.h>
#include <stdlib.h>

void Wave_Xml_TagType_delete(struct Wave_Context *const context, struct Wave_Xml_TagType* const tag_type) {
	if (tag_type == NULL)
		return;
	Wave_String_delete(context, &tag_type->name);
}

void Wave_Xml_Tag_delete(struct Wave_Context *const context, struct Wave_Xml_Tag* const tag) {
	if (tag == NULL)
		return;
	Wave_Xml_TagType_delete(context, &tag->type);
}

#if 0
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
	struct Wave_Context *const context,
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

static struct Wave_Xml_LexResult Wave_Xml_Text_lex_esc(
	
) {

}

static struct Wave_Xml_LexResult Wave_Xml_Text_lex(
	struct Wave_Context *const context,
	struct Wave_Xml_Text *wave_restrict const output,
	struct Wave_CharLocation *wave_restrict const loc,
	const char *wave_restrict const source,
	const size_t offset,
	#ifndef _MSC_VER
	const Wave_Lexer_DelimiterPredicate delimiter_predicates[wave_restrict const static 1]
	#else
	const Wave_Lexer_DelimiterPredicate *wave_restrict const delimiter_predicates
	#endif
) {
	struct Wave_Xml_LexResult result = {
		.error = Wave_CompilationError_none,
		.n = 0,
	};
	*output = (struct Wave_Xml_Text){
		.data = {
			.string_data = Wave_String_view_new(source, 0),
			.esc_vector = {0}
		},
		.loc = {0}
	};
	while (true) {
		const char *src = source + offset + result.n;
		Wave_Lexer_dp_check_derived(loc, NULL, source, offset + result.n) {

		}

		const struct Wave_CharLocation_AdvanceResult ch_result = Wave_CharLocation_advance(loc, src);
		if (ch_result.error.type != Wave_CompilationErrorType_None) {
			result.error.type = Wave_CompilationErrorType_InvalidAllocation;
			Wave_String_delete(context, &output->data.string_data);
			Wave_TokenString_EscVector_delete(context, &output->data.esc_vector);
			return result;
		}
		struct Wave_Unicode_CharUtf8 ch = ch_result.ch;

		if (*src == '&') { // mk: xml escape sequence `&amp;`
			loc->column--;
			loc->utf8_column--;
			loc->index--;
			loc->utf8_index--;

			if (Wave_String_is_view(output->data.string_data)) {
				if (!Wave_String_view_to_owned(context, &output->data.string_data)) {
					result.error.type = Wave_CompilationErrorType_InvalidAllocation;
					Wave_String_delete(context, &output->data.string_data);
					Wave_TokenString_EscVector_delete(context, &output->data.esc_vector);
					return result;
				}
			}

			struct Wave_TokenString_Esc escape;

			continue;
		} else {
			if (Wave_String_is_view(output->data.string_data)) {
				Wave_String_view_expand_utf8(&output->data.string_data, 1);
			} else {
				if (!Wave_String_push_utf8(context, &output->data.string_data, ch)) {
					result.error.type = Wave_CompilationErrorType_InvalidAllocation;
					Wave_String_delete(context, &output->data.string_data);
					Wave_TokenString_EscVector_delete(context, &output->data.esc_vector);
					return result;
				}
			}
		}
		result.n += ch_result.n;
	}

	return result;
}

static bool Wave_Xml_Prop_lex_value(
	struct Wave_Context *const context,
	struct Wave_Xml_PropValue *wave_restrict const output,
	struct Wave_CharLocation *wave_restrict const loc,
	const char *wave_restrict const source,
	const size_t offset,
	#ifndef _MSC_VER
	const Wave_Lexer_DelimiterPredicate delimiter_predicates[wave_restrict const static 1]
	#else
	const Wave_Lexer_DelimiterPredicate *wave_restrict const delimiter_predicates
	#endif
) {
	const char *src = source + offset;
	if (src[0] == '"') {
		output->boundaries = src[0];
		Wave_Lexer_DelimiterPredicate *dp = Wave_Lexer_dp_new_vector_with_concatenation(
			context,
			Wave_Lexer_dp_array(Wave_Xml_Prop_double_quoted_text_delimeter_predicate),
			delimiter_predicates
		);
		if (dp == NULL || Wave_Xml_Text_lex(context, &output->data.text, loc, source, offset + 1, delimiter_predicates).error.type != Wave_CompilationErrorType_None) {
			Wave_Lexer_dp_delete_vector(context, &dp);
			return false;
		}
		Wave_Lexer_dp_delete_vector(context, &dp);
	} else if (src[0] == '\'') {
		output->boundaries = src[0];
		Wave_Lexer_DelimiterPredicate *dp = Wave_Lexer_dp_new_vector_with_concatenation(
			context,
			Wave_Lexer_dp_array(Wave_Xml_Prop_double_quoted_text_delimeter_predicate),
			delimiter_predicates
		);
		if (dp == NULL || Wave_Xml_Text_lex(context, &output->data.text, loc, source, offset + 1, delimiter_predicates).error.type != Wave_CompilationErrorType_None) {
			Wave_Lexer_dp_delete_vector(context, &dp);
			return false;
		}
		Wave_Lexer_dp_delete_vector(context, &dp);
	} else if (src[0] == '{') {
		output->boundaries = '$';
		Wave_Lexer_DelimiterPredicate *dp = Wave_Lexer_dp_new_vector_with_concatenation(
			context,
			Wave_Lexer_dp_array(Wave_Xml_Prop_expr_delimeter_predicate),
			delimiter_predicates
		);
		if (dp == NULL || Wave_Xml_Text_lex(context, &output->data.text, loc, source, offset + 1, delimiter_predicates).error.type != Wave_CompilationErrorType_None) {
			Wave_Lexer_dp_delete_vector(context, &dp);
			return false;
		}
		Wave_Lexer_dp_delete_vector(context, &dp);
	} else {
		return false;
	}
}

bool Wave_Xml_Prop_lex(
	struct Wave_Context *const context,
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
#endif

static void Wave_Xml_PropValue_delete(struct Wave_Context *const context, struct Wave_Xml_PropValue *prop_value) {
	switch (prop_value->boundaries) {
		case '$':
			Wave_Xml_Expr_delete(context, &prop_value->data.expr);
			break;
		case '"':
		case '\'':
			Wave_Xml_Text_delete(context, &prop_value->data.text);
			break;
		default:
			break;
	}
}

void Wave_Xml_Prop_delete(struct Wave_Context *const context, struct Wave_Xml_Prop *const prop) {
	Wave_String_delete(context, &prop->key);
	Wave_Xml_PropValue_delete(context, &prop->value);
}

void Wave_Xml_delete(struct Wave_Context *const context, struct Wave_Xml* const element) {
	if (element == NULL)
		return;
	Wave_Xml_TagType_delete(context, &element->start_tag.type);
	if (Wave_Xml_is_self_closing(*element)) {
		Wave_Xml_TagType_delete(context, &element->end_tag.tag.type);
	}
	Wave_XmlVector_delete(context, &element->children);
}

bool Wave_XmlVector_push(struct Wave_Context *const context, struct Wave_XmlVector* const vector, const struct Wave_XmlNode node) {
	if (vector->nodes == NULL) {
		struct Wave_XmlNode* const block = (struct Wave_XmlNode*)w_malloc(context->allocator.alloc, context->allocator.userdata, 4 * sizeof(struct Wave_XmlNode));
		if (block == NULL)
			return false;
		vector->nodes = block;
		vector->capacity = 4;
		vector->size = 0;
	}
	else if (vector->size >= vector->capacity) {
		const size_t cap = 2 * vector->capacity;
		struct Wave_XmlNode* const block = (struct Wave_XmlNode*)w_realloc(context->allocator.alloc, context->allocator.userdata, vector->nodes, vector->capacity * sizeof(struct Wave_XmlNode), cap * sizeof(struct Wave_XmlNode));
		if (block == NULL)
			return false;
		vector->nodes = block;
		vector->capacity = cap;
	}

	vector->nodes[vector->size] = node;
	vector->size++;
	return true;
}

void Wave_XmlVector_delete(struct Wave_Context *const context, struct Wave_XmlVector* const vector) {
	if (vector == NULL || vector->nodes == NULL || vector->capacity == 0)
		return;

	for (size_t i = 0; i < vector->size; i++) {
		struct Wave_XmlNode* const node = vector->nodes + i;
		switch (node->type) {
		case Wave_XmlNodeType_Element:
			break;
		case Wave_XmlNodeType_Text:
			Wave_Xml_Text_delete(context, &node->data.text);
			break;
		case Wave_XmlNodeType_Expr:
			Wave_Xml_Expr_delete(context, &node->data.expr);
			break;
		default:
			break;
		}
	}

	w_free(context->allocator.alloc, context->allocator.userdata, vector->nodes, vector->capacity * sizeof(struct Wave_XmlNode));
	vector->capacity = 0;
	vector->size = 0;
}

// mk: wave::xml::text.delete()
void Wave_Xml_Text_delete(struct Wave_Context *const context, struct Wave_Xml_Text* const xml_text) {
	if (xml_text == NULL)
		return;

	Wave_TokenString_EscVector_delete(context, &xml_text->data.esc_vector);
	Wave_String_delete(context, &xml_text->data.string_data);
}

// mk: wave::xml::expr.delete()
void Wave_Xml_Expr_delete(struct Wave_Context *const context, struct Wave_Xml_Expr* const xml_expr) {
	if (xml_expr == NULL)
		return;

	Wave_TokenVector_delete(context, &xml_expr->data);
}
