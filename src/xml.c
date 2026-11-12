#include <wave/_debug.h>
#include <wave/char_location.h>
#include <wave/context.h>
#include <wave/def.h>
#include <wave/lexer.h>
#include <wave/string.h>
#include <wave/token_string.h>
#include <wave/token.h>
#include <wave/unicode.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
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

#if 1
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
	Wave_Debug_dump_dp_params(params);
	printf("`%s`\n", params.source + params.n);
    return params.source[params.n] == '}' && Wave_Token_do_counters_match(params.token_counters);
}

#define KEY_CHARS(ch) (((ch) >= 'a' && (ch) <= 'z') || ((ch) >= 'A' && (ch) <= 'Z') || ((ch) >= '0' && (ch) <= '9') || (ch) == '_')

// mk: individual xml prop key/value constructors & destructors will be `static` becase they aren't that needed outside this context
static struct Wave_Xml_LexResult Wave_Xml_Prop_lex_key(
	//struct Wave_Context *const context,
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
	struct Wave_Xml_LexResult result = Wave_Xml_LexResult_null;
	const char *src = source + offset;

	*output = Wave_String_view_new(src, 0);

	while (true) {
		Wave_Lexer_dp_check_derived(loc, NULL, source, offset + result.n) {
			result.flags |= WAVE_LEXER_TOKENIZE_RESULT_INTERRUPTED_BY_PREDICATE;
			//result.error.type = Wave_LogSignalType_UnexpectedBoundary; // mk: removed the error because it's not in the right level of handling, tag should handle unexpected termination
			return result;
		}

		const char ch = src[result.n];

		if (ch == '\0') {
			//result.error.type = Wave_LogSignalType_UnexpectedEndOfFile; // mk: same here
			return result;
		}

		if (!KEY_CHARS(ch)) {
			break;
		}

		Wave_CharLocation_advance_by(loc, source + offset + result.n, 1);
		Wave_String_view_expand_utf8(output, 1);
		result.n++;
	}
	
	return result;
}

static struct Wave_Xml_LexResult Wave_Xml_Text_lex_esc(
	struct Wave_Context *const context,
	struct Wave_TokenString_Esc *wave_restrict const output,
	struct Wave_CharLocation *wave_restrict const loc,
	const char *wave_restrict const source,
	const size_t offset,
	#ifndef _MSC_VER
	const Wave_Lexer_DelimiterPredicate delimiter_predicates[wave_restrict const static 1]
	#else
	const Wave_Lexer_DelimiterPredicate *wave_restrict const delimiter_predicates
	#endif
) {
	*output = (struct Wave_TokenString_Esc){0};
	struct Wave_Xml_LexResult result = Wave_Xml_LexResult_null;

	if (source[offset] != '&') {
		result.error.type = Wave_LogSignalType_UnexpectedCharacter;
		result.error.info_type = Wave_LogSignalInfoType_ExpectedSequence;
		strcpy(result.error.info.expected_sequence, "&");
		return result;
	}
	
	result.n += Wave_CharLocation_advance_by(loc, source + offset, 1);
	
	typedef enum {
		EscapeType_None,
		EscapeType_Dec,
		EscapeType_Hex,
		EscapeType_Id
	} EscapeType;

	EscapeType esc_type = EscapeType_None;
	size_t start_esc_value_index = 0; // mk: very first char offset of the `amp` or number, `0` would be invalid because of the `&` at the very first character
	output->sequence = Wave_String_view_new(source + offset + result.n, 0);

	while (true) {
		char ch = source[offset + result.n];

		if (Wave_CharLocation_advance_by(loc, source + offset + result.n, 1) != 1) {
			result.error.type = Wave_LogSignalType_UnexpectedCharacter;
			return result;
		}

		Wave_Lexer_dp_check_derived(loc, NULL, source, offset + result.n) {
			result.error.type = Wave_LogSignalType_UnexpectedBoundary;
			return result;
		}

		if (ch == '\0') {
			result.error.type = Wave_LogSignalType_UnexpectedEndOfFile;
			return result;
		}

		if (ch == '#' && result.n == 1) {
			result.n++;
			ch = source[offset + result.n];
			if (Wave_CharLocation_advance_by(loc, source + offset + result.n, 1) != 1) {
				result.error.type = Wave_LogSignalType_UnexpectedCharacter;
				return result;
			}
			Wave_Lexer_dp_check_derived(loc, NULL, source, offset + result.n) {
				result.error.type = Wave_LogSignalType_UnexpectedBoundary;
				return result;
			}

			if (ch >= '0' && ch <= '9') {
				esc_type = EscapeType_Dec;
				start_esc_value_index = 2;
			} else if (ch == 'x') {
				esc_type = EscapeType_Hex;
				start_esc_value_index = 3;
				result.n++;
				continue;
			} else {
				result.error.type = Wave_LogSignalType_UnexpectedCharacter;
				return result;
			}
		} else if (result.n == 1) {
			if (!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9'))) {
				result.error.type = Wave_LogSignalType_UnexpectedCharacter;
				return result;
			}
			esc_type = EscapeType_Id;
			start_esc_value_index = 1;
		} else if (((ch >= 'g' && ch <= 'z') || (ch >= 'G' && ch <= 'Z')) && esc_type == EscapeType_Id) {
			//
		} else if (((ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')) && esc_type != EscapeType_Dec) {
			//
		} else if (ch >= '0' && ch <= '9' && esc_type != EscapeType_Id) {
			//
		} else if (ch == ';' && !(start_esc_value_index == 0 || start_esc_value_index == result.n)) {
			break;
		} else {
			result.error.type = Wave_LogSignalType_UnexpectedCharacter;
			return result;
		}

		// mk: no need to check utf-8 chars and everything since only a restricted set of ascii chars get past the checks
		Wave_String_view_expand_utf8(&output->sequence, 1);
		result.n++;
	}
	result.n++;

	static const char *const entities[][2] = {
		{"plusmn", "±"},
		{"trade", "™"},
		{"pound", "£"},
		{"euro", "€"},
		{"cent", "¢"},
		{"sect", "§"},
		{"para", "¶"},
		{"nbsp", " "},
		{"copy", "©"},
		{"quot", "\""},
		{"apos", "'"},
		{"amp", "&"},
		{"reg", "®"},
		{"yen", "¥"},
		{"deg", "°"},
		{"lt", "<"},
		{"gt", ">"},
	};
	size_t char_value = 0;
	char *endptr = NULL;

	switch (esc_type) {
	case EscapeType_Dec:
		char_value = strtoul(source + offset + start_esc_value_index, &endptr, 10);
		break;
	case EscapeType_Hex:
		char_value = strtoul(source + offset + start_esc_value_index, &endptr, 16);
		break;
	case EscapeType_Id:
		for (size_t i = 0; i < (sizeof(entities) / sizeof(entities[0])); i++) {
			const size_t len = result.n - start_esc_value_index - 1;
			if(!strncmp(entities[i][0], source + offset + start_esc_value_index, len)) {
				output->output = Wave_String_view_new(entities[i][1], strlen(entities[i][1]));
				break;
			}
		}
		break;
	case EscapeType_None:
	default:
		result.error.type = Wave_LogSignalType_UnknownError;
		return result;
	}

	if (Wave_String_is_null(output->output)) {
		output->output = Wave_String_new(context, 8);
		if (output->output.data.str == NULL) {
			result.error.type = Wave_LogSignalType_InvalidAllocation;
			return result;
		}
		Wave_String_push_utf8(context, &output->output, Wave_Unicode_CharUtf8_get_at((uint32_t)char_value));
	}

	return result;
}

// mk: dynamically allocates the string with the data
static bool Wave_Xml_Text_to_owned(
	struct Wave_Context *const context,
	struct Wave_Xml_Text *const text
) {
	if (text == NULL)
		return false;
	if (Wave_String_is_null(text->data.string_data))
		return false;
	
	if (Wave_String_is_view(text->data.string_data) && !Wave_String_view_to_owned(context, &text->data.string_data)) {
		return false;
	}

	// mk: is that even needed if the escape sequences strings are only used during the tokenization or for lsp?
	// mk: not doing this for now
	/*for (size_t i = 0; i < text->data.esc_vector.size; i++) {
		struct Wave_String esc_sequence
	}*/
	return true;
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
	struct Wave_Xml_LexResult result = Wave_Xml_LexResult_null;
	*output = (struct Wave_Xml_Text){
		.data = {
			.string_data = Wave_String_view_new(source + offset, 0),
			.esc_vector = {0}
		},
		.loc = {0}
	};
	while (true) {
		const char *src = source + offset + result.n;
		Wave_Lexer_dp_check_derived(loc, NULL, source, offset + result.n) {
			break;
		}

		const struct Wave_CharLocation_AdvanceResult ch_result = Wave_CharLocation_advance(loc, src);
		if (Wave_LogSignal_is_error(ch_result.error)) {
			result.error.type = Wave_LogSignalType_InvalidAllocation;
			Wave_TokenString_delete(context, &output->data);
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
					result.error.type = Wave_LogSignalType_InvalidAllocation;
					//Wave_String_delete(context, &output->data.string_data);
					//Wave_TokenString_EscVector_delete(context, &output->data.esc_vector);
					Wave_TokenString_delete(context, &output->data);
					return result;
				}
			}

			struct Wave_TokenString_Esc escape = {0};
			struct Wave_Xml_LexResult lex_result = Wave_Xml_Text_lex_esc(context, &escape, loc, source, offset + result.n, delimiter_predicates);

			if (lex_result.error.type != Wave_LogSignalType_None) {
				result.error = lex_result.error;
				Wave_TokenString_delete(context, &output->data);
				return result;
			}

			escape.start_index = result.n;
			if (!Wave_TokenString_EscVector_push(context, &output->data.esc_vector, escape)) {
				result.error.type = Wave_LogSignalType_InvalidAllocation;
				Wave_TokenString_delete(context, &output->data);
				return result;
			}

			if (!Wave_String_append(context, &output->data.string_data, escape.output.data.str)) {
				result.error.type = Wave_LogSignalType_InvalidAllocation;
				Wave_TokenString_delete(context, &output->data);
				return result;
			}
			
			result.n += lex_result.n;
			continue;
		} else if (*src == '\0') {
			result.error.type = Wave_LogSignalType_UnexpectedEndOfFile;
			return result;
		} else {
			if (Wave_String_is_view(output->data.string_data)) {
				Wave_String_view_expand_utf8(&output->data.string_data, 1);
			} else {
				if (!Wave_String_push_utf8(context, &output->data.string_data, ch)) {
					result.error.type = Wave_LogSignalType_InvalidAllocation;
					Wave_TokenString_delete(context, &output->data);
					return result;
				}
			}
		}
		result.n += ch_result.n;
	}

	result.n++;

	return result;
}

static struct Wave_Xml_LexResult Wave_Xml_Prop_lex_value(
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
	struct Wave_Xml_LexResult result = {
		.error = Wave_LogSignal_none,
		.n = 0
	};

	const char *src = source + offset;
	Wave_Lexer_DelimiterPredicate predicate = NULL;

	if (src[0] == '"') {
		output->boundaries = src[0];
		predicate = Wave_Xml_Prop_double_quoted_text_delimeter_predicate;
	} else if (src[0] == '\'') {
		output->boundaries = src[0];
		predicate = Wave_Xml_Prop_single_quoted_text_delimeter_predicate;
	} else if (src[0] == '{') {
		output->boundaries = '$';
		predicate = Wave_Xml_Prop_expr_delimeter_predicate;
	} else {
		result.error.type = Wave_LogSignalType_UnexpectedCharacter;
		return result;
	}

	result.n++;
	Wave_Lexer_DelimiterPredicate *dp = Wave_Lexer_dp_new_vector_with_concatenation(
		context,
		Wave_Lexer_dp_array(predicate),
		delimiter_predicates
	);
	if (dp == NULL) {
		result.error.type = Wave_LogSignalType_InvalidAllocation;
		Wave_Lexer_dp_delete_vector(context, &dp);
		return result;
	}

	if (src[0] == '\'' || src[0] == '"') {
		struct Wave_Xml_LexResult lex_result = Wave_Xml_Text_lex(context, &output->data.text, loc, source, offset + 1, dp);
		if (lex_result.error.type != Wave_LogSignalType_None) {
			result.error = lex_result.error;
			Wave_Lexer_dp_delete_vector(context, &dp);
			return result;
		}

		result.n += lex_result.n;
	} else {
		output->data.expr.loc = *loc;
		struct Wave_Lexer_TokenizeResult expr_token_result = Wave_Lexer_tokenize(context, loc, source, offset + 1, dp);
		if (expr_token_result.error.type != Wave_LogSignalType_None) {
			result.error = expr_token_result.error;
			Wave_Lexer_dp_delete_vector(context, &dp);
			return result;
		}

		
		output->data.expr.length.bytes = output->data.expr.loc.index - loc->index;
		output->data.expr.length.utf8 = output->data.expr.loc.utf8_index - loc->utf8_index;
		output->data.expr.data = expr_token_result.token_vector;

		const char enclosing_boundary = source[offset + result.n + expr_token_result.n];
		if (enclosing_boundary == '\0') { // mk: 
			result.error.type = Wave_LogSignalType_UnexpectedEndOfFile;
			Wave_Lexer_dp_delete_vector(context, &dp);
			Wave_TokenVector_delete(context, &expr_token_result.token_vector);
			return result;
		}
		if (enclosing_boundary != '}') {
			result.error.type = Wave_LogSignalType_UnexpectedBoundary;
			Wave_Lexer_dp_delete_vector(context, &dp);
			Wave_TokenVector_delete(context, &expr_token_result.token_vector);
			return result;
		}

		result.n += expr_token_result.n + 2; // mk: includes the boundaries `{}`
	}
	Wave_Lexer_dp_delete_vector(context, &dp);

	return result;
}

struct Wave_Xml_LexResult Wave_Xml_Prop_lex(
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
	struct Wave_Xml_LexResult lex_result = Wave_Xml_LexResult_null;
	*prop = (struct Wave_Xml_Prop){
		.key = Wave_String_null,
		.value = {
			.boundaries = '\0',
			.data.none = 0
		},
		.eq_offset = 0,
		.value_offset = 0,
		.loc = *loc
	};

	// mk: constructing the key
	struct Wave_String key;
	struct Wave_Xml_LexResult key_lex_result = Wave_Xml_Prop_lex_key(&key, loc, source, offset, delimiter_predicates);

	if (Wave_LogSignal_is_error(key_lex_result.error)) {
		lex_result.error = key_lex_result.error;
		return lex_result;
	}
	if (key_lex_result.flags & WAVE_LEXER_TOKENIZE_RESULT_INTERRUPTED_BY_PREDICATE) {
		lex_result.flags |= WAVE_LEXER_TOKENIZE_RESULT_INTERRUPTED_BY_PREDICATE;
		return lex_result;
	}
	
	if (Wave_String_is_view(key) && !Wave_String_view_to_owned(context, &key)) {
		lex_result.error.type = Wave_LogSignalType_InvalidAllocation;
		return lex_result;
	}

	prop->key = key;
	lex_result.n += key_lex_result.n;

	size_t *current_offset = &prop->eq_offset;

	// mk: seeking `=` or next key or tag delimiter
	while (true) {
		Wave_Lexer_dp_check_derived(loc, NULL, source, offset + lex_result.n) {
			lex_result.flags |= WAVE_LEXER_TOKENIZE_RESULT_INTERRUPTED_BY_PREDICATE;
			return lex_result;
		}

		const char ch = source[offset + lex_result.n];

		if (ch == '\0') {
			return lex_result;
		}
		#if 0
		if (ch == '>') { // mk: end of the tag
			return lex_result;
		}
		#endif
		if (ch == '=') { // mk: `=` found!
			if (current_offset == &prop->eq_offset) {
				current_offset = &prop->value_offset;
				lex_result.n++;
				continue;
			} else { // mk: double `=` => error
				lex_result.error.type = Wave_LogSignalType_UnexpectedCharacter;
				return lex_result;
			}
		}

		if (KEY_CHARS(ch)) { // mk: new key without value
			if (current_offset == &prop->eq_offset)
				return lex_result;
			else {
				lex_result.error.type = Wave_LogSignalType_UnexpectedCharacter;
				return lex_result;
			}
		}

		if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
			// mk: ...
		} else {
			if (current_offset == &prop->eq_offset)
				return lex_result;
			else
				break;
		}

		(*current_offset)++;
		lex_result.n++;
	}

	// mk: constructing value
	//lex_result.n++;
	struct Wave_Xml_PropValue value = {
		.boundaries = '\0',
		.data.none = 0
	};
	struct Wave_Xml_LexResult value_lex_result = Wave_Xml_Prop_lex_value(context, &value, loc, source, offset + lex_result.n, delimiter_predicates);

	if (Wave_LogSignal_is_error(value_lex_result.error)) {
		lex_result.error = value_lex_result.error;
		return lex_result;
	}
	if (value_lex_result.flags & WAVE_LEXER_TOKENIZE_RESULT_INTERRUPTED_BY_PREDICATE) {
		lex_result.flags |= WAVE_LEXER_TOKENIZE_RESULT_INTERRUPTED_BY_PREDICATE;
		return lex_result;
	}

	if (value.boundaries == '"' || value.boundaries == '\'') {
		if (!Wave_Xml_Text_to_owned(context, &value.data.text)) {
			lex_result.error.type = Wave_LogSignalType_InvalidAllocation;
			return lex_result;
		}
	} else if (value.boundaries == '$') {
		// mk: forgot what i was supposed to do here
	}

	prop->value_offset = lex_result.n;
	prop->value = value;
	lex_result.n += value_lex_result.n;

	return lex_result;
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

	//(void)context;
	Wave_TokenVector_delete(context, &xml_expr->data);
}
