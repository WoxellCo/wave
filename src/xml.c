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

bool Wave_Xml_Prop_lex(const char* const source) {
	return false;
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
		const struct Wave_XmlNode* const node = vector->nodes + i;
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
