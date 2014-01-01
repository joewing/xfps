/****************************************************************************
 ****************************************************************************/

#include "xfps.h"

static const unsigned int BLOCK_SIZE = 128;

ObjectNode *map;
int width, height, depth;

static ObjectNode *CreateObject(ObjectType type);
static void Parse(TokenNode *start);
static void ParseWorldSize(AttributeNode *ap);
static void ParseWorldLocation(AttributeNode *ap);
static void ParseObject(TokenNode *np);
static void ParseCube(TokenNode *np);
static void ParseSphere(TokenNode *np);
static void ParseSquare(TokenNode *np);
static void ParseDisk(TokenNode *np);
static void ParseCylinder(TokenNode *np);
static int ParseTrueFalse(const char *value);
static void ParseObjectAttributes(AttributeNode *ap, ObjectNode *op);
static void ParseSize(AttributeNode *ap, ObjectNode *op);
static void ParseLocation(AttributeNode *ap, ObjectNode *op);
static void ParseRotation(AttributeNode *ap, ObjectNode *op);
static void ParseTexture(TokenNode *np, ObjectNode *op);
static void ParseNextMap(TokenNode *np, ObjectNode *op);

/****************************************************************************
 ****************************************************************************/
void LoadMap(const char *filename) {
	FILE *fd;
	TokenNode *tokens;
	char *buffer;
	unsigned int len, max;
	int ch;

	fd = fopen(filename, "r");
	if(!fd) {
		map = NULL;
		return;
	}

	max = BLOCK_SIZE;
	len = 0;
	buffer = Allocate(max + 1);

	for(;;) {
		ch = fgetc(fd);
		if(ch == EOF) {
			break;
		}
		buffer[len++] = ch;
		if(len >= max) {
			max += BLOCK_SIZE;
			buffer = Reallocate(buffer, max + 1);
		}
	}
	buffer[len] = 0;

	tokens = Tokenize(buffer);

	Release(buffer);

	Parse(tokens);

	ReleaseTokens(tokens);

}

/****************************************************************************
 ****************************************************************************/
void Parse(TokenNode *start) {
	TokenNode *np;

	if(!start) {
		return;
	}

	if(start->type == TOK_MAP) {
		for(np = start->subnodeHead; np; np = np->next) {
			switch(np->type) {
			case TOK_SIZE:
				ParseWorldSize(np->attributes);
				break;
			case TOK_LOCATION:
				ParseWorldLocation(np->attributes);
				break;
			case TOK_OBJECT:
				ParseObject(np->subnodeHead);
				break;
			default:
				Warning("map error: invalid token in Map: %d", np->type);
				break;
			}
		}
	}

}

/****************************************************************************
 ****************************************************************************/
void ParseWorldSize(AttributeNode *ap) {
	while(ap) {
		if(!strcmp(ap->name, "x")) {
			width = atof(ap->value);
		} else if(!strcmp(ap->name, "y")) {
			height = atof(ap->value);
		} else if(!strcmp(ap->name, "z")) {
			depth = atof(ap->value);
		}
		ap = ap->next;
	}
}

/****************************************************************************
 ****************************************************************************/
void ParseWorldLocation(AttributeNode *ap) {
	Vector loc;

	loc.x = 0.0;
	loc.y = 0.0;
	loc.z = 0.0;

	while(ap) {
		if(!strcmp(ap->name, "x")) {
			loc.x = atof(ap->value);
		} else if(!strcmp(ap->name, "y")) {
			loc.y = atof(ap->value);
		} else if(!strcmp(ap->name, "z")) {
			loc.z = atof(ap->value);
		}
		ap = ap->next;
	}

	SetPlayerLocation(&loc);

}

/****************************************************************************
 ****************************************************************************/
void ParseObject(TokenNode *np) {
	while(np) {
		switch(np->type) {
		case TOK_CUBE:
			ParseCube(np->subnodeHead);
			break;
		case TOK_SPHERE:
			ParseSphere(np->subnodeHead);
			break;
		case TOK_SQUARE:
			ParseSquare(np->subnodeHead);
			break;
		case TOK_DISK:
			ParseDisk(np->subnodeHead);
			break;
		case TOK_CYLINDER:
			ParseCylinder(np->subnodeHead);
			break;
		default:
			Warning("map error: invalid token in Object: %d", np->type);
			break;
		}
		np = np->next;
	}
}

/****************************************************************************
 ****************************************************************************/
void ParseCube(TokenNode *np) {
	ObjectNode *op;

	op = CreateObject(OBJ_CUBE);
	ParseObjectAttributes(np->parent->attributes, op);

	while(np) {
		switch(np->type) {
		case TOK_SIZE:
			ParseSize(np->attributes, op);
			break;
		case TOK_LOCATION:
			ParseLocation(np->attributes, op);
			break;
		case TOK_ROTATION:
			ParseRotation(np->attributes, op);
			break;
		case TOK_TEXTURE:
			ParseTexture(np, op);
			break;
		case TOK_NEXT:
			ParseNextMap(np, op);
			break;
		default:
			Warning("map error: invalid token in Cube: %d", np->type);
			break;
		}
		np = np->next;
	}
}

/****************************************************************************
 ****************************************************************************/
void ParseSphere(TokenNode *np) {
	ObjectNode *op;

	op = CreateObject(OBJ_SPHERE);
	ParseObjectAttributes(np->parent->attributes, op);

	while(np) {
		switch(np->type) {
		case TOK_SIZE:
			ParseSize(np->attributes, op);
			break;
		case TOK_LOCATION:
			ParseLocation(np->attributes, op);
			break;
		case TOK_ROTATION:
			ParseRotation(np->attributes, op);
			break;
		case TOK_TEXTURE:
			ParseTexture(np, op);
			break;
		case TOK_NEXT:
			ParseNextMap(np, op);
			break;
		default:
			Warning("map error: invalid token in Sphere: %d", np->type);
			break;
		}
		np = np->next;
	}
}

/****************************************************************************
 ****************************************************************************/
void ParseSquare(TokenNode *np) {
	ObjectNode *op;

	op = CreateObject(OBJ_SQUARE);
	ParseObjectAttributes(np->parent->attributes, op);

	while(np) {
		switch(np->type) {
		case TOK_SIZE:
			ParseSize(np->attributes, op);
			break;
		case TOK_LOCATION:
			ParseLocation(np->attributes, op);
			break;
		case TOK_ROTATION:
			ParseRotation(np->attributes, op);
			break;
		case TOK_TEXTURE:
			ParseTexture(np, op);
			break;
		case TOK_NEXT:
			ParseNextMap(np, op);
			break;
		default:
			Warning("map error: invalid token in Square: %d", np->type);
			break;
		}
		np = np->next;
	}
}

/****************************************************************************
 ****************************************************************************/
void ParseDisk(TokenNode *np) {
	ObjectNode *op;

	op = CreateObject(OBJ_DISK);
	ParseObjectAttributes(np->parent->attributes, op);

	while(np) {
		switch(np->type) {
		case TOK_SIZE:
			ParseSize(np->attributes, op);
			break;
		case TOK_LOCATION:
			ParseLocation(np->attributes, op);
			break;
		case TOK_ROTATION:
			ParseRotation(np->attributes, op);
			break;
		case TOK_TEXTURE:
			ParseTexture(np, op);
			break;
		case TOK_NEXT:
			ParseNextMap(np, op);
			break;
		default:
			Warning("map error: invalid token in Disk: %d", np->type);
			break;
		}
		np = np->next;
	}
}

/****************************************************************************
 ****************************************************************************/
void ParseCylinder(TokenNode *np) {
	ObjectNode *op;

	op = CreateObject(OBJ_CYLINDER);
	ParseObjectAttributes(np->parent->attributes, op);

	while(np) {
		switch(np->type) {
		case TOK_SIZE:
			ParseSize(np->attributes, op);
			break;
		case TOK_LOCATION:
			ParseLocation(np->attributes, op);
			break;
		case TOK_ROTATION:
			ParseRotation(np->attributes, op);
			break;
		case TOK_TEXTURE:
			ParseTexture(np, op);
			break;
		case TOK_NEXT:
			ParseNextMap(np, op);
			break;
		default:
			Warning("map error: invalid token in Disk: %d", np->type);
			break;
		}
		np = np->next;
	}
}

/****************************************************************************
 ****************************************************************************/
int ParseTrueFalse(const char *value) {
	if(value[0] == 't' || value[0] == 'T' || value[0] == '1') {
		return 1;
	} else {
		return 0;
	}
}

/****************************************************************************
 ****************************************************************************/
void ParseObjectAttributes(AttributeNode *ap, ObjectNode *op) {
	while(ap) {
		if(!strcmp(ap->name, "checked")) {
			if(ParseTrueFalse(ap->value)) {
				op->propertyFlags |= PROP_CHECK;
			} else {
				op->propertyFlags &= ~PROP_CHECK;
			}
		} else if(!strcmp(ap->name, "lighted")) {
			if(ParseTrueFalse(ap->value)) {
				op->propertyFlags |= PROP_LIGHT;
			} else {
				op->propertyFlags &= ~PROP_LIGHT;
			}
		}
		ap = ap->next;
	}
}

/****************************************************************************
 ****************************************************************************/
void ParseSize(AttributeNode *ap, ObjectNode *op) {
	while(ap) {
		if(!strcmp(ap->name, "x")) {
			op->size.x = atof(ap->value);
		} else if(!strcmp(ap->name, "y")) {
			op->size.y = atof(ap->value);
		} else if(!strcmp(ap->name, "z")) {
			op->size.z = atof(ap->value);
		} else if(!strcmp(ap->name, "radius")) {
			op->size.x = atof(ap->value);
			op->size.y = atof(ap->value);
			op->size.z = atof(ap->value);
		}
		ap = ap->next;
	}
}

/****************************************************************************
 ****************************************************************************/
void ParseLocation(AttributeNode *ap, ObjectNode *op) {
	while(ap) {
		if(!strcmp(ap->name, "x")) {
			op->loc.x = atof(ap->value);
		} else if(!strcmp(ap->name, "y")) {
			op->loc.y = atof(ap->value);
		} else if(!strcmp(ap->name, "z")) {
			op->loc.z = atof(ap->value);
		}
		ap = ap->next;
	}
}

/****************************************************************************
 ****************************************************************************/
void ParseRotation(AttributeNode *ap, ObjectNode *op) {
	while(ap) {
		if(!strcmp(ap->name, "x")) {
			op->rotatex = atof(ap->value);
		} else if(!strcmp(ap->name, "y")) {
			op->rotatey = atof(ap->value);
		}
		ap = ap->next;
	}
}

/****************************************************************************
 ****************************************************************************/
void ParseNextMap(TokenNode *np, ObjectNode *op) {
	if(op->nextMap) {
		Release(op->nextMap);
	}
	if(np->value) {
		op->nextMap = Allocate(strlen(np->value) + 1);
		strcpy(op->nextMap, np->value);
	}
}

/****************************************************************************
 ****************************************************************************/
void ParseTexture(TokenNode *np, ObjectNode *op) {
	op->texture = InsertTexture(np->value);
}

/****************************************************************************
 ****************************************************************************/
ObjectNode *CreateObject(ObjectType type) {
	ObjectNode *obj;

	obj = Allocate(sizeof(ObjectNode));

	obj->type = type;
	obj->propertyFlags = PROP_CHECK | PROP_LIGHT;
	obj->group = 0;
	obj->texture = 0;
	obj->loc.x = 0.0;
	obj->loc.y = 0.0;
	obj->loc.z = 0.0;
	obj->size.x = 1.0;
	obj->size.y = 1.0;
	obj->size.z = 1.0;
	obj->rotatex = 0;
	obj->rotatey = 0;
	obj->drotatex = 0;
	obj->drotatey = 0;
	obj->nextMap = NULL;

	obj->prev = NULL;
	obj->next = map;
	if(map) {
		map->prev = obj;
	}
	map = obj;

	return obj;
}

/****************************************************************************
 ****************************************************************************/
void DestroyMap() {
	ObjectNode *np;
	while(map) {
		np = map->next;
		if(map->nextMap) {
			Release(map->nextMap);
		}
		Release(map);
		map = np;
	}
}

