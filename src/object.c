/****************************************************************************
 ****************************************************************************/

#include "xfps.h"

static GLUquadricObj *objects[OBJ_COUNT];

static void DrawCube(const ObjectNode *op);
static void DrawCylinder(const ObjectNode *op);
static void DrawSphere(const ObjectNode *op);
static void DrawDisk(const ObjectNode *op);
static void DrawSquare(const ObjectNode *op);

static int CheckCubeCollision(Vector pt, ObjectNode *op, float size);
static int CheckSphereCollision(Vector pt, ObjectNode *op, float size);

/****************************************************************************
 ****************************************************************************/
void InitializeObjects() {
	int x;

	for(x = 0; x < OBJ_COUNT; x++) {
		objects[x] = gluNewQuadric();
		gluQuadricDrawStyle(objects[x], GLU_FILL);
		gluQuadricNormals(objects[x], GLU_SMOOTH);
		gluQuadricTexture(objects[x], GL_TRUE);
	}

}

/****************************************************************************
 ****************************************************************************/
void ResetObjects() {
}

/****************************************************************************
 ****************************************************************************/
void DestroyObjects() {
	int x;

	for(x = 0; x < OBJ_COUNT; x++) {
		gluDeleteQuadric(objects[x]);
	}
}

/****************************************************************************
 ****************************************************************************/
void DrawCube(const ObjectNode *op) {

	glPushMatrix();
	glTranslatef(op->loc.x, op->loc.y, op->loc.z);
	glRotatef(op->rotatey, 0.0, 1.0, 0.0);
	glRotatef(op->rotatex, 1.0, 0.0, 0.0);

	glBegin(GL_POLYGON);
		glNormal3f(0.0, -op->loc.y, 0.0);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glTexCoord2f(0.0, op->size.z);
		glVertex3f(0.0, 0.0, op->size.z);
		glTexCoord2f(op->size.x, op->size.z);
		glVertex3f(op->size.x, 0.0, op->size.z);
		glTexCoord2f(op->size.x, 0.0);
		glVertex3f(op->size.x, 0.0, 0.0);
	glEnd();

	glBegin(GL_POLYGON);
		glNormal3f(0.0, -(op->loc.y + op->size.y), 0.0);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(0.0, op->size.y, 0.0);
		glTexCoord2f(0.0, op->size.z);
		glVertex3f(0.0, op->size.y, op->size.z);
		glTexCoord2f(op->size.x, op->size.z);
		glVertex3f(op->size.x, op->size.y, op->size.z);
		glTexCoord2f(op->size.x, 0.0);
		glVertex3f(op->size.x, op->size.y, 0.0);
	glEnd();

	glBegin(GL_POLYGON);
		glNormal3f(0.0, 0.0, -op->loc.z);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glTexCoord2f(0.0, op->size.y);
		glVertex3f(0.0, op->size.y, 0.0);
		glTexCoord2f(op->size.x, op->size.y);
		glVertex3f(op->size.x, op->size.y, 0.0);
		glTexCoord2f(op->size.x, 0.0);
		glVertex3f(op->size.x, 0.0, 0.0);
	glEnd();
	glBegin(GL_POLYGON);
		glNormal3f(0.0, 0.0, -(op->loc.z + op->size.z));
		glTexCoord2f(0.0, 0.0);
		glVertex3f(0.0, 0.0, op->size.z);
		glTexCoord2f(0.0, op->size.y);
		glVertex3f(0.0, op->size.y, op->size.z);
		glTexCoord2f(op->size.x, op->size.y);
		glVertex3f(op->size.x, op->size.y, op->size.z);
		glTexCoord2f(op->size.x, 0.0);
		glVertex3f(op->size.x, 0.0, op->size.z);
	glEnd();

	glBegin(GL_POLYGON);
		glNormal3f(-op->loc.x, 0.0, 0.0);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glTexCoord2f(0.0, op->size.z);
		glVertex3f(0.0, 0.0, op->size.z);
		glTexCoord2f(op->size.y, op->size.z);
		glVertex3f(0.0, op->size.y, op->size.z);
		glTexCoord2f(op->size.y, 0.0);
		glVertex3f(0.0, op->size.y, 0.0);
	glEnd();
	glBegin(GL_POLYGON);
		glNormal3f(-(op->loc.x + op->size.x), 0.0, 0.0);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(op->size.x, 0.0, 0.0);
		glTexCoord2f(0.0, op->size.z);
		glVertex3f(op->size.x, 0.0, op->size.z);
		glTexCoord2f(op->size.y, op->size.z);
		glVertex3f(op->size.x, op->size.y, op->size.z);
		glTexCoord2f(op->size.y, 0.0);
		glVertex3f(op->size.x, op->size.y, 0.0);
	glEnd();
	glPopMatrix();
}

/****************************************************************************
 ****************************************************************************/
void DrawCylinder(const ObjectNode *op) {
	glPushMatrix();
	glTranslatef(op->loc.x, op->loc.y, op->loc.z);
	glRotatef(op->rotatey, 0.0, 1.0, 0.0);
	glRotatef(op->rotatex, 1.0, 0.0, 0.0);
	glTranslatef(op->size.x / 2.0, op->size.y / 2.0, 0.0);
	gluCylinder(objects[OBJ_CYLINDER], op->size.x / 2.0,
		op->size.y / 2.0, op->size.z, 20, 10);
	glPopMatrix();
}

/****************************************************************************
 ****************************************************************************/
void DrawSphere(const ObjectNode *op) {
	glPushMatrix();
	glTranslatef(op->loc.x + op->size.x / 2.0,
		op->loc.y + op->size.y / 2.0,
		op->loc.z + op->size.z / 2.0);
	glRotatef(op->rotatey, 0.0, 1.0, 0.0);
	glRotatef(op->rotatex, 1.0, 0.0, 0.0);
	gluSphere(objects[OBJ_SPHERE], op->size.x / 2.0, 20, 20);
	glPopMatrix();
}

/****************************************************************************
 ****************************************************************************/
void DrawDisk(const ObjectNode *op) {
	glPushMatrix();
	glTranslatef(op->loc.x, op->loc.y, op->loc.z);
	glRotatef(op->rotatey, 0.0, 1.0, 0.0);
	glRotatef(op->rotatex, 1.0, 0.0, 0.0);
	glTranslatef(0.0, op->size.y / 2.0, 0.0);
	gluDisk(objects[OBJ_DISK], 0.0, op->size.x / 2.0, 20, 20);
	glPopMatrix();
}

/****************************************************************************
 ****************************************************************************/
void DrawSquare(const ObjectNode *op) {
	glPushMatrix();
	glTranslatef(op->loc.x, op->loc.y, op->loc.z);
	glRotatef(op->rotatey, 0.0, 1.0, 0.0);
	glRotatef(op->rotatex, 1.0, 0.0, 0.0);
	glBegin(GL_POLYGON);
		glNormal3f(0.0, 0.0, -op->loc.z);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glTexCoord2f(0.0, op->size.y);
		glVertex3f(0.0, op->size.y, 0.0);
		glTexCoord2f(op->size.x, op->size.y);
		glVertex3f(op->size.x, op->size.y, 0.0);
		glTexCoord2f(op->size.x, 0.0);
		glVertex3f(op->size.x, 0.0, 0.0);
	glEnd();
	glPopMatrix();
}

/****************************************************************************
 ****************************************************************************/
void DrawObject(const ObjectNode *op) {

	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, op->texture);

	if(!(op->propertyFlags & PROP_LIGHT)) {
		glDisable(GL_LIGHTING);
	}

	switch(op->type) {
	case OBJ_CUBE:
		DrawCube(op);
		break;
	case OBJ_CYLINDER:
		DrawCylinder(op);
		break;
	case OBJ_SPHERE:
		DrawSphere(op);
		break;
	case OBJ_DISK:
		DrawDisk(op);
		break;
	case OBJ_SQUARE:
		DrawSquare(op);
		break;
	default:
		fprintf(stderr, "warning: invalid object type: %d\n", op->type);
		break;
	}

	if(!(op->propertyFlags & PROP_LIGHT)) {
		glEnable(GL_LIGHTING);
	}

}

/****************************************************************************
 ****************************************************************************/
int UpdateObjects() {
	ObjectNode *np;
	int updated = 0;

	for(np = map; np; np = np->next) {
		if(np->drotatex != 0) {
			updated = 1;
			np->rotatex = (np->rotatex + np->drotatex) % 360;
		}
		if(np->drotatey != 0) {
			updated = 1;
			np->rotatey = (np->rotatey + np->drotatey) % 360;
		}
	}

	return updated;

}

/****************************************************************************
 ****************************************************************************/
Vector ComputeCrossProduct(const Vector *a, const Vector *b) {
	Vector n;

	n.x = a->x * b->z - a->z * b->y;
	n.y = a->z * b->x - a->x * b->z;
	n.z = a->x * b->y - a->y * b->x;

	return n;
}

/****************************************************************************
 ****************************************************************************/
float ComputeDotProduct(const Vector *a, const Vector *b) {
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

/****************************************************************************
 ****************************************************************************/
float ComputeNorm(const Vector *v) {
	return sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

/****************************************************************************
 ****************************************************************************/
Vector AddVectors(const Vector *a, const Vector *b) {
	Vector v;

	v.x = a->x + b->x;
	v.y = a->y + b->y;
	v.z = a->z + b->z;

	return v;
}

/****************************************************************************
 ****************************************************************************/
Vector SubtractVectors(const Vector *a, const Vector *b) {
	Vector v;

	v.x = a->x - b->x;
	v.y = a->y - b->y;
	v.z = a->z - b->z;

	return v;
}

/****************************************************************************
 ****************************************************************************/
ObjectNode *CheckCollision(Vector pt, float size) {
	ObjectNode *op;

	for(op = map; op; op = op->next) {

		if(!(op->propertyFlags & PROP_CHECK)) {
			continue;
		}

		switch(op->type) {
		case OBJ_CUBE:
		case OBJ_CYLINDER:
			if(CheckCubeCollision(pt, op, size)) {
				return op;
			}
			break;
		case OBJ_SPHERE:
			if(CheckSphereCollision(pt, op, size)) {
				return op;
			}
			break;
		case OBJ_DISK:
		case OBJ_SQUARE:
			break;
		default:
			fprintf(stderr, "warning: unknown object type: %d\n", op->type);
			break;
		}
	}

	return NULL;
}

/****************************************************************************
 ****************************************************************************/
int CheckCubeCollision(Vector pt, ObjectNode *op, float size) {
	Vector a, b;
	int rx, ry;

	/* Translate to the origin of the object */
	a.x = pt.x - op->loc.x;
	a.y = pt.y - op->loc.y;
	a.z = pt.z - op->loc.z;

	/* Rotate */
	ry = -op->rotatey;
	if(ry < 0) {
		ry += 360;
	}
	b.x = a.z * sine[ry] + a.x * cosine[ry];
	b.y = a.y;
	b.z = a.z * cosine[ry] - a.x * sine[ry];

	rx = -op->rotatex;
	if(rx < 0) {
		rx += 360;
	}
	a.x = b.x;
	a.y = b.y * cosine[rx] - b.z * sine[rx];
	a.z = b.y * sine[rx] + b.z * cosine[rx];

	/* Check if the point is contained */
	if(a.x + size >= 0.0 && a.x - size <= op->size.x) {
		if(a.y + size >= 0.0 && a.y - size <= op->size.y) {
			if(a.z + size >= 0.0 && a.z - size <= op->size.z) {
				return 1;
			}
		}
	}

	return 0;
}

/****************************************************************************
 ****************************************************************************/
int CheckSphereCollision(Vector pt, ObjectNode *op, float size) {
	Vector a;

	a.x = pt.x - op->loc.x;
	a.y = pt.y - op->loc.y;
	a.z = pt.z - op->loc.z;

	if(a.x + size >= 0.0 && a.x - size <= op->size.x) {
		if(a.y + size >= 0.0 && a.y - size <= op->size.y) {
			if(a.z + size >= 0.0 && a.z - size <= op->size.z) {
				return 1;
			}
		}
	}

	return 0;
}




