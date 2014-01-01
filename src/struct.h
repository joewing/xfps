
#ifndef STRUCT_H
#define STRUCT_H

typedef struct {
	float x, y, z;
} Vector;

typedef struct {
	Vector loc;
	int rotatex, rotatey;
	int health;
	int jumpCount;
	float jumpAcceleration;
	MotionFlagType motion;
} PlayerNode;

typedef struct ObjectNode {
	ObjectType type;
	ObjectPropertyType propertyFlags;
	int group;
	GLuint texture;

	Vector loc;
	Vector size;

	int rotatex, rotatey;
	int drotatex, drotatey;

	char *nextMap;

	struct ObjectNode *prev;
	struct ObjectNode *next;
} ObjectNode;

typedef struct AttributeNode {
	char *name;
	char *value;
	struct AttributeNode *next;
} AttributeNode;

typedef struct TokenNode {
	TokenType type;
	char *value;
	AttributeNode *attributes;	
	struct TokenNode *subnodeHead;
	struct TokenNode *subnodeTail;
	struct TokenNode *parent;
	struct TokenNode *next;
} TokenNode;

typedef struct ImageNode {
	GLuint id;
	char *name;
	png_uint_32 width, height;
	unsigned char *data;
	struct ImageNode *next;
} ImageNode;


#endif

