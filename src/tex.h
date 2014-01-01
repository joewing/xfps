
#ifndef TEX_H
#define TEX_H

void InitializeTextures();
void DestroyTextures();

unsigned int InsertTexture(const char *name);

ImageNode *ReadImage(const char *fileName);

#endif

