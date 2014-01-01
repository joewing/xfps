
#ifndef FONT_H
#define FONT_H

void InitializeFont();
void DestroyFont();

void DrawString(const char *str, int x, int y);
void DrawNumber(int number, int x, int y);

#endif

