
#ifndef ENEMY_H
#define ENEMY_H

void InitializeEnemies();
void DestroyEnemies();

void InsertEnemy(Vector loc);
void SpawnEnemies(int number);

int CheckEnemyBulletCollision(Vector pos, void *owner);

int UpdateEnemies();
void DrawEnemies();

#endif

