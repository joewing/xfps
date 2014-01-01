
#ifndef LEX_H
#define LEX_H

TokenNode *Tokenize(const char *line);
void ReleaseTokens(TokenNode *np);

#endif

