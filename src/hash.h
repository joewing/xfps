
#ifndef HASH_H
#define HASH_H

typedef unsigned int (*HashFunction)(void *key);
typedef unsigned int (*HashCompare)(void *a, void *b);

typedef struct HashType {
	void *data;
} HashType;

HashType *CreateHash(unsigned int tableSize, HashFunction func,
	HashCompare comp);
void DestroyHash(HashType *hash);

void InsertHash(HashType *hash, void *key, void *data);
void *RemoveHash(HashType *hash, void *key);
void *SearchHash(HashType *hash, void *key);

#endif


