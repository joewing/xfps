
#ifndef MAP_H
#define MAP_H

extern ObjectNode *map;
extern int width, height, depth;

void LoadMap(const char *filename);
void DestroyMap();

#endif

