/****************************************************************************
 ****************************************************************************/

#include "xfps.h"

static PlayerNode player;

static int DoMove(float dx, float dz);
static int DoJump(float dy);
static int CheckPlayerObjectCollision(Vector v, float size);

/****************************************************************************
 ****************************************************************************/
void InitializePlayer() {
	player.loc.x = 0.0;
	player.loc.y = 0.0;
	player.loc.z = 0.0;
	player.rotatex = 0;
	player.rotatey = 0;
	player.health = 100;
	player.jumpCount = 0;
	player.motion = MOTION_ONGROUND;
}

/****************************************************************************
 ****************************************************************************/
void SetPlayerLocation(const Vector *loc) {
	player.loc = *loc;
}

/****************************************************************************
 ****************************************************************************/
void SetPlayerPerspective() {
	float realx, realz;

	realx = player.loc.x * cosine[player.rotatey]
		+ player.loc.z * sine[player.rotatey];
	realz = player.loc.z * cosine[player.rotatey]
		- player.loc.x * sine[player.rotatey];

	gluPerspective(60.0, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
		MOVE_DELTA / 2, maxSize * 6);

	glRotatef(player.rotatex, 1.0, 0.0, 0.0);
	glTranslatef(-realx, -player.loc.y - 0.5, -realz);
	glRotatef(player.rotatey, 0.0, 1.0, 0.0);

}

/****************************************************************************
 ****************************************************************************/
int GetPlayerHealth() {
	return player.health;
}

/****************************************************************************
 ****************************************************************************/
Vector GetPlayerPosition() {
	return player.loc;
}

/****************************************************************************
 ****************************************************************************/
Vector GetPlayerDirection() {
	Vector dir;
	int rx;
	float srx;

	rx = (player.rotatex + 90) % 360;
	srx = sine[rx];

	dir.x = sine[player.rotatey] * srx;
	dir.y = cosine[rx];
	dir.z = -cosine[player.rotatey] * srx;

	return dir;
}

/****************************************************************************
 ****************************************************************************/
void GetPlayerRotation(int *rx, int *ry) {
	*rx = -player.rotatex;
	*ry = -player.rotatey;
}

/****************************************************************************
 ****************************************************************************/
void RotatePlayer(int drx, int dry) {

	player.rotatey += dry;
	while(player.rotatey < 0) {
		player.rotatey += 360;
	}
	player.rotatey %= 360;

	player.rotatex += drx;
	if(player.rotatex > 45) {
		player.rotatex = 45;
	}
	if(player.rotatex < -45) {
		player.rotatex = -45;
	}

}

/****************************************************************************
 ****************************************************************************/
void MovePlayerLeft(char set) {
	if(set) {
		player.motion &= ~MOTION_RIGHT;
		player.motion |= MOTION_LEFT;
	} else {
		player.motion &= ~MOTION_LEFT;
	}
}

/****************************************************************************
 ****************************************************************************/
void MovePlayerRight(char set) {
	if(set) {
		player.motion &= ~MOTION_LEFT;
		player.motion |= MOTION_RIGHT;
	} else {
		player.motion &= ~MOTION_RIGHT;
	}
}

/****************************************************************************
 ****************************************************************************/
void MovePlayerForward(char set) {
	if(set) {
		player.motion &= ~MOTION_BACKWARD;
		player.motion |= MOTION_FORWARD;
	} else {
		player.motion &= ~MOTION_FORWARD;
	}
}

/****************************************************************************
 ****************************************************************************/
void MovePlayerBackward(char set) {
	if(set) {
		player.motion &= ~MOTION_FORWARD;
		player.motion |= MOTION_BACKWARD;
	} else {
		player.motion &= ~MOTION_BACKWARD;
	}
}

/****************************************************************************
 ****************************************************************************/
void DestroyPlayer() {
}

/****************************************************************************
 ****************************************************************************/
void JumpPlayer(char press) {
   if(!press) {
      return;
   }
	if(!(player.motion & MOTION_JUMP)) {
		if(player.motion & MOTION_ONGROUND) {
			player.motion |= MOTION_JUMP;
			player.motion &= ~MOTION_ONGROUND;
			player.jumpCount = MAX_JUMP_COUNT;
		}
	}
}

/****************************************************************************
 ****************************************************************************/
void SetPlayerButtonPress(unsigned int button) {
	InsertBullet(player.loc, GetPlayerDirection(), &player);
}

/****************************************************************************
 ****************************************************************************/
int CheckPlayerBulletCollision(Vector pos, void *owner) {

	if(owner == &player) {
		return 0;
	}

	if(abs(player.loc.x - pos.x) < 0.5) {
		if(abs(player.loc.y - pos.y) < 0.5) {
			if(abs(player.loc.z - pos.z) < 0.5) {
				if(player.health > 5) {
					player.health -= 5;
				} else {
					player.health = 0;
				}
				return 1;
			}
		}
	}

	return 0;
}

/****************************************************************************
 ****************************************************************************/
int UpdatePlayer() {
	float sdelta = MOVE_DELTA * sine[player.rotatey];
	float cdelta = MOVE_DELTA * cosine[player.rotatey];
	int updated = 0;
	int isUp;

	if(player.motion & MOTION_JUMP) {
		--player.jumpCount;
		if(player.jumpCount <= 0 || !DoJump(JUMP_DELTA)) {
			player.jumpCount = 0;
			player.motion &= ~MOTION_JUMP;
		} else {
			updated = 1;
		}
	}

	if(player.motion & (MOTION_FORWARD | MOTION_BACKWARD
		| MOTION_LEFT | MOTION_RIGHT)) {
		isUp = DoJump(JUMP_DELTA);
	} else {
		isUp = 0;
	}

	if(player.motion & MOTION_FORWARD) {
		updated |= DoMove(sdelta, -cdelta);
	}

	if(player.motion & MOTION_BACKWARD) {
		updated |= DoMove(-sdelta, cdelta);
	}

	if(player.motion & MOTION_RIGHT) {
		updated |= DoMove(cdelta, sdelta);
	}

	if(player.motion & MOTION_LEFT) {
		updated |= DoMove(-cdelta, -sdelta);
	}

	if(isUp) {
		DoJump(-JUMP_DELTA);
	}

	if(!(player.motion & MOTION_JUMP)) {
		if(DoJump(-JUMP_DELTA)) {
			player.motion &= ~MOTION_ONGROUND;
			updated = 1;
		} else {
			player.motion |= MOTION_ONGROUND;
		}
	}

	return updated;
}

/****************************************************************************
 ****************************************************************************/
int DoJump(float dy) {
	Vector v;

	v.x = player.loc.x;
	v.y = player.loc.y + dy;
	v.z = player.loc.z;

	if(!CheckPlayerObjectCollision(v, JUMP_DELTA)) {
		player.loc.y += dy;
		return 1;
	}

	return 0;

}

/****************************************************************************
 ****************************************************************************/
int DoMove(float dx, float dz) {
	Vector v;

	v.x = player.loc.x + dx;
	v.y = player.loc.y;
	v.z = player.loc.z + dz;
	if(!CheckPlayerObjectCollision(v, MOVE_DELTA)) {
		player.loc.x += dx;
		player.loc.z += dz;
		return 1;
	}

	if(dx > dz) {
		v.x = player.loc.x + dx;
		v.y = player.loc.y;
		v.z = player.loc.z;
		if(!CheckPlayerObjectCollision(v, MOVE_DELTA)) {
			player.loc.x += dx;
			return 1;
		}
		v.x = player.loc.x;
		v.y = player.loc.y;
		v.z = player.loc.z + dz;
		if(!CheckPlayerObjectCollision(v, MOVE_DELTA)) {
			player.loc.z += dz;
			return 1;
		}
	} else {
		v.x = player.loc.x;
		v.y = player.loc.y;
		v.z = player.loc.z + dz;
		if(!CheckPlayerObjectCollision(v, MOVE_DELTA)) {
			player.loc.z += dz;
			return 1;
		}
		v.x = player.loc.x + dx;
		v.y = player.loc.y;
		v.z = player.loc.z;
		if(!CheckPlayerObjectCollision(v, MOVE_DELTA)) {
			player.loc.x += dx;
			return 1;
		}
	}

	return 0;
}

/****************************************************************************
 ****************************************************************************/
int CheckPlayerObjectCollision(Vector v, float size) {
	ObjectNode *op;

	op = CheckCollision(v, size);
	if(op) {
		if(op->nextMap) {
			nextLevel = Allocate(strlen(op->nextMap) + 1);
			strcpy(nextLevel, op->nextMap);
		}
		return 1;
	} else {
		return 0;
	}
}


