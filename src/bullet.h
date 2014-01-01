
#ifndef BULLET_H
#define BULLET_H

void InitializeBullets();
void DestroyBullets();

void DrawBullets();
int UpdateBullets();
void InsertBullet(Vector pos, Vector dir, void *owner);


#endif

