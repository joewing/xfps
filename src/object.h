
#ifndef OBJECT_H
#define OBJECT_H

void InitializeObjects();
void ResetObjects();
void DestroyObjects();

void DrawObject(const ObjectNode *obj);
int UpdateObjects();

void ComputeBoundingBox(ObjectNode *np);

ObjectNode *CheckCollision(Vector pt, float size);

#endif

