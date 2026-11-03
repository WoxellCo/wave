#ifndef WAVE_XML_H
#define WAVE_XML_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <wave/token.h>
#include <wave/string.h>
#include <wave/token-string.h>
#include <wave/char-location.h>

#define Wave_Xml_SELF_CLOSING		(1u << 0)
#define Wave_Xml_CLOSING			(1u << 1)

#define Wave_Xml_is_self_closing(xml_element) (((xml_element).start_tag.flags) & Wave_Xml_SELF_CLOSING)

// mk: kinda funny how visual studio 2022 enforces styles, but idc, if you see pointer operator stars sided on the type instead of the variable name, here's why

struct Wave_XmlVector {
	struct Wave_XmlNode* nodes;
	size_t size;
	size_t capacity;
};

struct Wave_Xml_PropVector {
	struct Wave_XmlNode* nodes;
	size_t size;
	size_t capacity;
};

// mk: information about the type inside the tag `<   type   />`
struct Wave_Xml_TagType {
	struct Wave_String name;
	struct Wave_CharLocation loc;
	//struct Wave_CharLength length;// mk: the length of the type (removed because the string already has this information)
};

// mk: information about the tag itself, which contains the tag type as member ofc
struct Wave_Xml_Tag {
	struct Wave_Xml_TagType type;
	struct Wave_Xml_PropVector props;
	struct Wave_CharLocation loc;
	struct Wave_CharLength length;	// mk: the length of the whole tag
	uint8_t flags;					// mk: closing or self-closing
};

// mk: information about an xml element (xml), which contains the tag information, children, and an optional ending tag (because it can be self-enclosing and have none)
struct Wave_Xml {
	struct Wave_Xml_Tag start_tag;
	union {
		int none;
		struct Wave_Xml_Tag tag;
	} end_tag;
	struct Wave_XmlVector children;	// mk: xml vectors won't have a `new` function, just `push` whenever it needs children
	struct Wave_CharLength length;	// mk: the length of the whole xml element
};

struct Wave_Xml_Text {
	struct Wave_TokenString data;	// mk: gonna reuse the struct in string token because it has a very similar layout
	struct Wave_CharLocation loc;
};

struct Wave_Xml_Expr {
	struct Wave_TokenVector data;
	struct Wave_CharLocation loc;
	struct Wave_CharLength length;
};

enum Wave_XmlNodeType {
	Wave_XmlNodeType_Element,
	Wave_XmlNodeType_Text,
	Wave_XmlNodeType_Expr
};

struct Wave_XmlNode {
	union {
		struct Wave_Xml element;
		struct Wave_Xml_Text text;
		struct Wave_Xml_Expr expr;
	} data;
	enum Wave_XmlNodeType type;
};

struct Wave_Xml_PropValue {
	char boundaries;				// mk: starts and ends with `'` or `"`? or even `$` (expr)? `\0` if there's no value
	union {
		int none;					// mk: ik it's weird, but that's how i mark optionals in c
		struct Wave_Xml_Text text;	// mk: it's not like a regular string because it has the xml (&<seq>;) escape sequence format
		struct Wave_Xml_Expr expr;
	} data;
	//enum Wave_XmlNodeType type;	// mk: expr or string? but actually it can be deduced by the boundaries, removing it then
};

// mk: xml property in tag
struct Wave_Xml_Prop {
	struct Wave_String key;
	struct Wave_Xml_PropValue value;
	size_t eq_offset;				// mk: the `=` sign distance from the key example `key="value"` would be `0`, `key ="value"` would be `1`
	size_t value_offset;			// mk: `key="value"` would be `0`, `key= "value"` would be `1`
	struct Wave_CharLocation loc;	// mk: location of the first character of the key, the whole length can be retrieved with the previous properties
};

void Wave_Xml_TagType_delete(struct Wave_Xml_TagType* const tag_type);
void Wave_Xml_Tag_delete(struct Wave_Xml_Tag* const tag);

// mk: alright to initialize this you have to pass the address of the uninitialized value
bool Wave_Xml_Prop_lex(const char *const source);

void Wave_Xml_delete(struct Wave_Xml *const element);

bool Wave_XmlVector_push(struct Wave_XmlVector* const vector, const struct Wave_XmlNode node);
void Wave_XmlVector_delete(struct Wave_XmlVector *const vector);

void Wave_Xml_Text_delete(struct Wave_Xml_Text *const xml_text);
void Wave_Xml_Expr_delete(struct Wave_Xml_Expr* const xml_expr);

#endif // !WAVE_XML_H
