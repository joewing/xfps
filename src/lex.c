/*****************************************************************************
 *****************************************************************************/

#include "xfps.h"

static const int BLOCK_SIZE = 16;

static TokenNode *head, *current;

static TokenNode *CreateNode(TokenNode *parent);
static AttributeNode *CreateAttribute(TokenNode *np); 

static int IsElementEnd(char ch);
static int IsValueEnd(char ch);
static int IsAttributeEnd(char ch);
static int IsSpace(char ch);
static char *ReadElementName(const char *line);
static char *ReadElementValue(const char *line);
static char *ReadAttributeValue(const char *line);
static TokenType LookupType(const char *name);

static void ReleaseAttributes(AttributeNode *ap);

/*****************************************************************************
 *****************************************************************************/
TokenNode *Tokenize(const char *line) {
	TokenNode *np;
	AttributeNode *ap;
	char *temp;
	int inElement;
	int x;
	int found;

	head = NULL;
	current = NULL;
	inElement = 0;

	x = 0;
	/* Skip any initial white space */
	while(IsSpace(line[x])) ++x;

	/* Skip any XML stuff */
	if(!strncmp(line + x, "<?", 2)) {
		while(line[x]) {
			if(!strncmp(line + x, "?>", 2)) {
				x += 2;
				break;
			}
			++x;
		}
	}

	while(line[x]) {

		do {

			while(IsSpace(line[x])) ++x;

			/* Skip comments */
			found = 0;
			if(!strncmp(line + x, "<!--", 4)) {
				while(line[x]) {
					if(!strncmp(line + x, "-->", 3)) {
						x += 3;
						found = 1;
						break;
					}
					++x;
				}
			}
		} while(found);

		switch(line[x]) {
		case '<':
			++x;
			if(line[x] == '/') {
				if(current) {
					current = current->parent;
				} else {
					Warning("map error: end node without start");
				}
				++x;
				temp = ReadElementName(line + x);
				if(temp) {
					x += strlen(temp);
					Release(temp);
				} else {
					Warning("map error: invalid end node");
				}
			} else {
				np = current;
				current = NULL;
				np = CreateNode(np);
				temp = ReadElementName(line + x);
				if(temp) {
					x += strlen(temp);
					np->type = LookupType(temp);
					Release(temp);
				} else {
					Warning("map error: invalid start node");
				}
			}
			inElement = 1;
			break;
		case '/':
			++x;
			if(inElement) {
				if(line[x] == '>' && current) {
					++x;
					current = current->parent;
					inElement = 0;
				} else {
					Warning("map error: invalid node");
				}
			} else {
				Warning("map error: invalid token \'/\'");
			}
			break;
		case '>':
			++x;
			inElement = 0;
			break;
		default:
			if(inElement) {
				ap = CreateAttribute(current);
				ap->name = ReadElementName(line + x);
				if(ap->name) {
					x += strlen(ap->name);
					if(line[x] == '=') {
						++x;
					}
					if(line[x] == '\"') {
						++x;
					}
					ap->value = ReadAttributeValue(line + x);
					if(ap->value) {
						x += strlen(ap->value);
					}
					if(line[x] == '\"') {
						++x;
					}
				}
			} else {
				temp = ReadElementValue(line + x);
				if(temp) {
					if(current) {
						if(current->value) {
							current->value = Reallocate(current->value,
								strlen(current->value) + strlen(temp) + 1);
							strcat(current->value, temp);
							Release(temp);
						} else {
							current->value = temp;
						}
					} else {
						if(temp[0]) {
							Warning("map error: unexpected text: \"%s\"",
								temp);
						}
						Release(temp);
					}
					x += strlen(temp);
				}
			}
			break;
		}
	}

	return head;
}

/*****************************************************************************
 *****************************************************************************/
int IsElementEnd(char ch) {
	switch(ch) {
	case ' ':
	case '\t':
	case '\n':
	case '\r':
	case '\"':
	case '>':
	case '<':
	case '/':
	case '=':
	case 0:
		return 1;
	default:
		return 0;
	}
}

/*****************************************************************************
 *****************************************************************************/
int IsAttributeEnd(char ch) {
	switch(ch) {
	case 0:
	case '\"':
		return 1;
	default:
		return 0;
	}
}

/*****************************************************************************
 *****************************************************************************/
int IsValueEnd(char ch) {
	switch(ch) {
	case 0:
	case '<':
		return 1;
	default:
		return 0;
	}
}

/*****************************************************************************
 *****************************************************************************/
int IsSpace(char ch) {
	switch(ch) {
	case ' ':
	case '\t':
	case '\n':
	case '\r':
		return 1;
	default:
		return 0;
	}
}

/*****************************************************************************
 *****************************************************************************/
char *ReadElementName(const char *line) {
	char *buffer;
	int len, max;
	int x;

	len = 0;
	max = BLOCK_SIZE;
	buffer = Allocate(max + 1);

	for(x = 0; !IsElementEnd(line[x]); x++) {
		buffer[len++] = line[x];
		if(len >= max) {
			max += BLOCK_SIZE;
			buffer = Reallocate(buffer, max + 1);
		}
	}
	buffer[len] = 0;

	return buffer;
}

/*****************************************************************************
 *****************************************************************************/
char *ReadElementValue(const char *line) {
	char *buffer;
	int len, max;
	int x;

	len = 0;
	max = BLOCK_SIZE;
	buffer = Allocate(max + 1);

	for(x = 0; !IsValueEnd(line[x]); x++) {
		buffer[len++] = line[x];
		if(len >= max) {
			max += BLOCK_SIZE;
			buffer = Reallocate(buffer, max + 1);
		}
	}
	buffer[len] = 0;

	return buffer;
}

/*****************************************************************************
 *****************************************************************************/
char *ReadAttributeValue(const char *line) {
	char *buffer;
	int len, max;
	int x;

	len = 0;
	max = BLOCK_SIZE;
	buffer = Allocate(max + 1);

	for(x = 0; !IsAttributeEnd(line[x]); x++) {
		buffer[len++] = line[x];
		if(len >= max) {
			max += BLOCK_SIZE;
			buffer = Reallocate(buffer, max + 1);
		}
	}
	buffer[len] = 0;

	return buffer;
}

/*****************************************************************************
 *****************************************************************************/
TokenType LookupType(const char *name) {
	TokenType type = TOK_INVALID;

	switch(strlen(name)) {
	case 3:
		if(!strcmp(name, "Map")) {
			type = TOK_MAP;
		}
		break;
	case 4:
		if(!strcmp(name, "Size")) {
			type = TOK_SIZE;
		} else if(!strcmp(name, "Cube")) {
			type = TOK_CUBE;
		} else if(!strcmp(name, "Disk")) {
			type = TOK_DISK;
		} else if(!strcmp(name, "Next")) {
			type = TOK_NEXT;
		}
		break;
	case 6:
		if(!strcmp(name, "Sphere")) {
			type = TOK_SPHERE;
		} else if(!strcmp(name, "Square")) {
			type = TOK_SQUARE;
		} else if(!strcmp(name, "Object")) {
			type = TOK_OBJECT;
		}
		break;
	case 7:
		if(!strcmp(name, "Texture")) {
			type = TOK_TEXTURE;
		}
		break;
	case 8:
		if(!strcmp(name, "Location")) {
			type = TOK_LOCATION;
		} else if(!strcmp(name, "Rotation")) {
			type = TOK_ROTATION;
		} else if(!strcmp(name, "Cylinder")) {
			type = TOK_CYLINDER;
		}
		break;
	default:
		break;
	}
	return type;
}

/*****************************************************************************
 *****************************************************************************/
TokenNode *CreateNode(TokenNode *parent) {
	TokenNode *np;

	np = Allocate(sizeof(TokenNode));
	np->type = TOK_INVALID;
	np->value = NULL;
	np->attributes = NULL;
	np->subnodeHead = NULL;
	np->subnodeTail = NULL;
	np->parent = parent;
	np->next = NULL;

	if(!head) {
		head = np;
	}
	if(parent) {
		if(parent->subnodeHead) {
			parent->subnodeTail->next = np;
		} else {
			parent->subnodeHead = np;
		}
		parent->subnodeTail = np;
	} else if(current) {
		current->next = np;
	}
	current = np;

	return np;
}

/*****************************************************************************
 *****************************************************************************/
AttributeNode *CreateAttribute(TokenNode *np) {
	AttributeNode *ap;

	ap = Allocate(sizeof(AttributeNode));
	ap->name = NULL;
	ap->value = NULL;

	ap->next = np->attributes;
	np->attributes = ap;

	return ap;
}

/*****************************************************************************
 *****************************************************************************/
void ReleaseAttributes(AttributeNode *ap) {
	AttributeNode *tp;

	while(ap) {
		tp = ap->next;

		if(ap->name) {
			Release(ap->name);
		}
		if(ap->value) {
			Release(ap->value);
		}

		Release(ap);

		ap = tp;
	}
}

/*****************************************************************************
 *****************************************************************************/
void ReleaseTokens(TokenNode *np) {
	TokenNode *tp;

	while(np) {
		tp = np->next;

		if(np->subnodeHead) {
			ReleaseTokens(np->subnodeHead);
		}
		if(np->value) {
			Release(np->value);
		}
		ReleaseAttributes(np->attributes);
		Release(np);

		np = tp;
	}

}


