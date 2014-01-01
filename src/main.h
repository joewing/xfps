
#ifndef MAIN_H
#define MAIN_H

extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;
extern const float MOVE_DELTA;
extern const float JUMP_DELTA;
extern const int MAX_JUMP_COUNT;
extern const float GRAVITY;
extern const float TERMINAL_VELOCITY;

extern int maxSize;

extern Display *display;
extern Window mainWindow;

extern char *nextLevel;

void StartLevel(const char *mapName);

#endif

