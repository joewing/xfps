/****************************************************************************
 * A generic hash table.
 * Copyright (C) 2003 Joe Wingbermuehle
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ****************************************************************************/

#include <stdlib.h>

#include "debug.h"
#include "hash.h"

typedef struct HashNode {
	void *key;
	void *data;
	struct HashNode *next;
} HashNode;

typedef struct HashData {
	unsigned int tableSize;
	HashFunction func;
	HashCompare comp;
	HashNode **data;
} HashData;

/****************************************************************************
 ****************************************************************************/
HashType *CreateHash(unsigned int tableSize, HashFunction func,
	HashCompare comp) {

	HashType *hp;
	HashData *dp;
	unsigned int x;

	Assert(tableSize > 0);

	hp = Allocate(sizeof(HashType));
	dp = Allocate(sizeof(HashData));
	hp->data = dp;

	dp->tableSize = tableSize;
	dp->func = func;
	dp->comp = comp;
	dp->data = Allocate(sizeof(HashNode*) * tableSize);

	for(x = 0; x < tableSize; x++) {
		dp->data[x] = NULL;
	}

	return hp;
}

/****************************************************************************
 ****************************************************************************/
void DestroyHash(HashType *hash) {
	HashData *dp;
	HashNode *np;
	HashNode *tp;
	unsigned int x;

	Assert(hash);

	dp = hash->data;

	for(x = 0; x < dp->tableSize; x++) {
		np = dp->data[x];
		while(np) {
			tp = np->next;
			Release(np);
			np = tp;
		}
	}
	Release(dp->data);
	Release(dp);
	Release(hash);

}

/****************************************************************************
 ****************************************************************************/
void InsertHash(HashType *hash, void *key, void *data) {
	HashData *dp = (HashData*)(hash->data);
	HashNode *np;
	unsigned int index;

	index = (dp->func)(key);

	Assert(index < dp->tableSize);

	np = Allocate(sizeof(HashNode));
	np->key = key;
	np->data = data;
	np->next = dp->data[index];
	dp->data[index] = np;

}

/****************************************************************************
 ****************************************************************************/
void *RemoveHash(HashType *hash, void *key) {
	HashData *dp = (HashData*)(hash->data);
	HashNode *np, *lp;
	void *temp;
	unsigned int index;

	index = (dp->func)(key);

	Assert(index < dp->tableSize);

	np = dp->data[index];
	lp = NULL;
	while(np) {
		if((dp->comp)(np->key, key)) {
			temp = np->data;

			if(lp) {
				lp->next = np->next;
			} else {
				dp->data[index] = np->next;
			}

			Release(np);

			return temp;
		}
		lp = np;
		np = np->next;
	}

	return NULL;
}

/****************************************************************************
 ****************************************************************************/
void *SearchHash(HashType *hash, void *key) {
	HashData *dp = (HashData*)(hash->data);
	HashNode *np;
	unsigned int index;

	index = (dp->func)(key);

	Assert(index < dp->tableSize);

	np = dp->data[index];
	while(np) {
		if((dp->comp)(np->key, key)) {
			return np->data;
		}
		np = np->next;
	}

	return NULL;
}



