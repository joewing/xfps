
#ifndef PLAYER_H
#define PLAYER_H

void InitializePlayer();
void DestroyPlayer();

void SetPlayerLocation(const Vector *loc);

int GetPlayerHealth();
Vector GetPlayerPosition();
Vector GetPlayerDirection();
void GetPlayerRotation(int *rx, int *ry);

void JumpPlayer(char press);
void RotatePlayer(int drx, int dry);
void MovePlayerLeft(char set);
void MovePlayerRight(char set);
void MovePlayerForward(char set);
void MovePlayerBackward(char set);
void SetPlayerButtonPress(unsigned int button);

void SetPlayerPerspective();

int CheckPlayerBulletCollision(Vector pos, void *owner);
int UpdatePlayer();

#endif

