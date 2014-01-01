
#ifndef ENUM_H
#define ENUM_H

typedef enum {
	TOK_INVALID,
	TOK_MAP,
	TOK_SIZE,
	TOK_LOCATION,
	TOK_ROTATION,
	TOK_TEXTURE,
	TOK_OBJECT,
	TOK_CUBE,
	TOK_SPHERE,
	TOK_SQUARE,
	TOK_DISK,
	TOK_CYLINDER,
	TOK_NEXT
} TokenType;

typedef enum {
	MOTION_NONE     = 0,
	MOTION_FORWARD  = 1,
	MOTION_BACKWARD = 2,
	MOTION_LEFT     = 4,
	MOTION_RIGHT    = 8,
	MOTION_JUMP     = 16,
	MOTION_ONGROUND = 32
} MotionFlagType;

typedef enum {
	OBJ_NONE     = 0,
	OBJ_SQUARE   = 1,
	OBJ_CUBE     = 2,
	OBJ_DISK     = 3,
	OBJ_CYLINDER = 4,
	OBJ_SPHERE   = 5,
	OBJ_COUNT    = 6
} ObjectType;

typedef enum {
	PROP_NONE   = 0,
	PROP_SHOT   = 1,
	PROP_MOVED  = 2,
	PROP_CHECK  = 4,
	PROP_LIGHT  = 8
} ObjectPropertyType;

#endif

