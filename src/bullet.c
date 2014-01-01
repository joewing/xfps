/****************************************************************************
 ****************************************************************************/

#include "xfps.h"

typedef struct BulletNode {
	Vector loc;
	Vector dir;
	int rotatex, rotatey;
	void *owner;
	struct BulletNode *next;
} BulletNode;

static const float bulletDelta = 0.3;
static BulletNode *bullets;

static GLUquadricObj *bulletPart[2];
static unsigned int bulletList;

/****************************************************************************
 ****************************************************************************/
void InitializeBullets() {
	bullets = NULL;

	bulletPart[0] = gluNewQuadric();
	bulletPart[1] = gluNewQuadric();
	bulletList = glGenLists(2);

	gluQuadricDrawStyle(bulletPart[0], GLU_FILL);
	gluQuadricNormals(bulletPart[0], GLU_SMOOTH);
	gluQuadricDrawStyle(bulletPart[1], GLU_FILL);
	gluQuadricNormals(bulletPart[1], GLU_SMOOTH);

	glNewList(bulletList, GL_COMPILE);
		gluSphere(bulletPart[0], 0.01, 15, 10);
		gluCylinder(bulletPart[1], 0.01, 0.01, 0.025, 10, 10);
	glEndList();

}

/****************************************************************************
 ****************************************************************************/
void DestroyBullets() {
	BulletNode *np;

	while(bullets) {
		np = bullets->next;
		Release(bullets);
		bullets = np;
	}

	gluDeleteQuadric(bulletPart[0]);
	gluDeleteQuadric(bulletPart[1]);
	glDeleteLists(bulletList, 2);

}

/****************************************************************************
 ****************************************************************************/
void DrawBullets() {
	BulletNode *np;

	glColor3f(0.0, 0.0, 0.0);
	for(np = bullets; np; np = np->next) {
		glPushMatrix();
		glTranslatef(np->loc.x, np->loc.y, np->loc.z);
		glRotatef(np->rotatey, 0.0, 1.0, 0.0);
		glRotatef(np->rotatex, 1.0, 0.0, 0.0);
		glCallList(bulletList);
		glPopMatrix();
	}
	glColor3f(1.0, 1.0, 1.0);
}

/****************************************************************************
 ****************************************************************************/
int UpdateBullets() {
	BulletNode *np, *last;
	ObjectNode *op;
	int shouldDestroy;

	if(!bullets) {
		return 0;
	}

	last = NULL;
	for(np = bullets; np; np = np->next) {

		np->loc.x += np->dir.x;
		np->loc.y += np->dir.y;
		np->loc.z += np->dir.z;

		shouldDestroy = 0;

		if(np->loc.x < -width / 2.0 || np->loc.y > width / 2.0) {
			shouldDestroy = 1;
		}
		if(np->loc.y < 0.0 || np->loc.y > depth) {
			shouldDestroy = 1;
		}
		if(np->loc.z < -height / 2.0 || np->loc.z > height / 2.0) {
			shouldDestroy = 1;
		}

		if((op = CheckCollision(np->loc, bulletDelta / 2.0))) {
			shouldDestroy = 1;

			if(op->propertyFlags & PROP_SHOT) {
				if(op->next) {
					op->next->prev = op->prev;
				}
				if(op->prev) {
					op->prev->next = op->next;
				} else {
					map = op->next;
				}
				Release(op);
			}
		}

		shouldDestroy |= CheckEnemyBulletCollision(np->loc, np->owner);
		shouldDestroy |= CheckPlayerBulletCollision(np->loc, np->owner);

		if(shouldDestroy) {
			if(last) {
				last->next = np->next;
			} else {
				bullets = np->next;
			}
			Release(np);
		} else {
			last = np;
		}
	}

	return 1;

}

/****************************************************************************
 ****************************************************************************/
void InsertBullet(Vector pos, Vector dir, void *owner) {
	BulletNode *np;
	float dist;

	dist = sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
	if(dist == 0.0) {
		return;
	}

	np = Allocate(sizeof(BulletNode));
	np->next = bullets;
	bullets = np;

	np->owner = owner;
	np->loc = pos;
	np->loc.y += 0.5;
	np->dir = dir;
	np->dir.x *= bulletDelta / dist;
	np->dir.y *= bulletDelta / dist;
	np->dir.z *= bulletDelta / dist;
	GetPlayerRotation(&np->rotatex, &np->rotatey);

}


