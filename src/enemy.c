/****************************************************************************
 ****************************************************************************/

#include "xfps.h"

static const int MIN_BULLET_DELAY = 20;
static const int MAX_BULLET_DELAY = 100;
static const int MIN_DIR_DELAY = 5;
static const int MAX_DIR_DELAY = 50;
static const float ENEMY_DELTA = 0.1;
static const float ENEMY_SIZE = 1.0;

typedef struct EnemyNode {
	Vector loc;
	Vector dir;
	int dcount;
	int bcount;
	struct EnemyNode *prev;
	struct EnemyNode *next;
} EnemyNode;

static EnemyNode *enemies = NULL;
static GLUquadricObj *enemyObject;
static unsigned int enemyTexture;

static int UpdateEnemy(EnemyNode *np);
static void DrawEnemy(EnemyNode *np);
static int DoMove(EnemyNode *np);

/****************************************************************************
 ****************************************************************************/
void InitializeEnemies() {
	enemies = NULL;

	enemyObject = gluNewQuadric();
	gluQuadricDrawStyle(enemyObject, GLU_FILL);
	gluQuadricNormals(enemyObject, GLU_SMOOTH);
	gluQuadricTexture(enemyObject, GL_TRUE);

/*	enemyTexture = InsertTexture("textures/enemy.png");*/

}

/****************************************************************************
 ****************************************************************************/
void DestroyEnemies() {
	EnemyNode *np;

	while(enemies) {
		np = enemies->next;
		Release(enemies);
		enemies = np;
	}

	gluDeleteQuadric(enemyObject);
}

/****************************************************************************
 ****************************************************************************/
void InsertEnemy(Vector loc) {
	EnemyNode *np;

	np = Allocate(sizeof(EnemyNode));
	np->prev = NULL;
	np->next = enemies;
	if(enemies) {
		enemies->prev = np;
	}
	enemies = np;

	np->loc = loc;
	np->dcount = 0;
	np->bcount = (rand() % (MAX_BULLET_DELAY - MIN_BULLET_DELAY))
		+ MIN_BULLET_DELAY;

	UpdateEnemy(np);

}

/****************************************************************************
 ****************************************************************************/
void SpawnEnemies(int number) {
	Vector v;
	int x;

	enemyTexture = InsertTexture("textures/enemy.png");

	for(x = 0; x < number; x++) {

		do {
			v.x = (rand() % width) - width / 2.0;
			v.y = rand() % depth;
			v.z = (rand() % height) - height / 2.0;
		} while(CheckCollision(v, ENEMY_SIZE / 2.0));

		InsertEnemy(v);

	}

}

/****************************************************************************
 ****************************************************************************/
int UpdateEnemies() {
	EnemyNode *np;
	int updated = 0;

	for(np = enemies; np; np = np->next) {
		updated |= UpdateEnemy(np);
	}

	return updated;
}

/****************************************************************************
 ****************************************************************************/
int CheckEnemyBulletCollision(Vector pos, void *owner) {
	EnemyNode *np;
	Vector v;

	for(np = enemies; np; np = np->next) {

		if(owner == np) {
			continue;
		}

		v.x = pos.x - np->loc.x + ENEMY_SIZE / 2.0;
		v.y = pos.y - np->loc.y + ENEMY_SIZE / 2.0;
		v.z = pos.z - np->loc.z + ENEMY_SIZE / 2.0;

		if(v.x >= 0.0 && v.x <= ENEMY_SIZE) {
			if(v.y >= 0.0 && v.y <= ENEMY_SIZE) {
				if(v.z >= 0.0 && v.z <= ENEMY_SIZE) {
					if(np->prev) {
						np->prev->next = np->next;
					} else {
						enemies = np->next;
					}
					if(np->next) {
						np->next->prev = np->prev;
					}
					Release(np);
					return 1;
				}
			}
		}
	}

	return 0;
}

/****************************************************************************
 ****************************************************************************/
int UpdateEnemy(EnemyNode *np) {
	Vector v;
	float dist;
	int updated = 0;

	if(np->dcount <= 0) {

		/* Find the player location wrt this enemy */
		v = GetPlayerPosition();
		v.x -= np->loc.x;
		v.y -= np->loc.y;
		v.z -= np->loc.z;

		dist = sqrt(v.x * v.x + v.z * v.z);

		np->dir.x = (ENEMY_DELTA / dist) * v.x;
		np->dir.y = 0.0;
		np->dir.z = (ENEMY_DELTA / dist) * v.z;

		np->dcount = (rand() % (MAX_DIR_DELAY - MIN_DIR_DELAY))
			+ MIN_DIR_DELAY;

	} else {

		v = np->loc;
		v.y += 0.1;
		if(!CheckCollision(v, ENEMY_SIZE / 2.0)) {
			np->loc.y += 0.1;
		}

		updated |= DoMove(np);

		v = np->loc;
		v.y -= 0.1;
		if(!CheckCollision(v, ENEMY_SIZE / 2.0)) {
			np->loc.y -= 0.1;
		}

		v = np->loc;
		v.y -= 0.2;
		if(!CheckCollision(v, ENEMY_SIZE / 2.0)) {
			np->loc.y -= 0.2;
			updated = 1;
		}

		--np->dcount;

	}

	if(np->bcount <= 0) {
		v = np->loc;
		v.y -= ENEMY_SIZE / 2.0;
		InsertBullet(v, np->dir, np);
		np->bcount = (rand() % (MAX_BULLET_DELAY - MIN_BULLET_DELAY))
			+ MIN_BULLET_DELAY;
	} else {
		--np->bcount;
	}

	return updated;
}

/****************************************************************************
 ****************************************************************************/
int DoMove(EnemyNode *np) {
	Vector v;

	v.x = np->loc.x + np->dir.x;
	v.y = np->loc.y;
	v.z = np->loc.z + np->dir.z;
	if(!CheckCollision(v, ENEMY_SIZE / 2.0)) {
		np->loc.x += np->dir.x;
		np->loc.z += np->dir.z;
		return 1;
	}

	if(np->dir.x > np->dir.z) {
		v.x = np->loc.x + np->dir.x;
		v.y = np->loc.y;
		v.z = np->loc.z;
		if(!CheckCollision(v, ENEMY_SIZE / 2.0)) {
			np->loc.x += np->dir.x;
			return 1;
		}
		v.x = np->loc.x;
		v.y = np->loc.y;
		v.z = np->loc.z + np->dir.z;
		if(!CheckCollision(v, ENEMY_SIZE / 2.0)) {
			np->loc.z += np->dir.z;
			return 1;
		}
	} else {
		v.x = np->loc.x;
		v.y = np->loc.y;
		v.z = np->loc.z + np->dir.z;
		if(!CheckCollision(v, ENEMY_SIZE / 2.0)) {
			np->loc.z += np->dir.z;
			return 1;
		}
		v.x = np->loc.x + np->dir.x;
		v.y = np->loc.y;
		v.z = np->loc.z;
		if(!CheckCollision(v, ENEMY_SIZE / 2.0)) {
			np->loc.x += np->dir.x;
			return 1;
		}
	}

	return 0;
}

/****************************************************************************
 ****************************************************************************/
void DrawEnemies() {
	EnemyNode *np;

	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, enemyTexture);

	for(np = enemies; np; np = np->next) {
		DrawEnemy(np);
	}
}

/****************************************************************************
 ****************************************************************************/
void DrawEnemy(EnemyNode *np) {
	int ry;

	glPushMatrix();
	glTranslatef(np->loc.x, np->loc.y, np->loc.z);

	if(np->dir.x != 0.0) {
		ry = -180.0 * atan2(np->dir.z, np->dir.x) / PI - 90;
	} else {
		ry = 0;
	}
	glRotatef(ry, 0.0, 1.0, 0.0);
	glRotatef(90, 1.0, 0.0, 0.0);
	gluSphere(enemyObject, 0.5, 20, 20);
	glPopMatrix();
}



