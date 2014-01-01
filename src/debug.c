/***************************************************************************
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
 ***************************************************************************/

#include "debug.h"

/***************************************************************************
 * Emit a message.
 ***************************************************************************/
void Debug(const char *str, ...) {
#ifdef DEBUG
	va_list ap;
	va_start(ap, str);

	fprintf(stderr, "DEBUG: ");
	vfprintf(stderr, str, ap);
	fprintf(stderr, "\n");

	va_end(ap);
#endif
}

#ifdef DEBUG

typedef struct MemoryType {
	char *file;
	unsigned int line;
	size_t size;
	void *pointer;
	struct MemoryType *next;
} MemoryType;

static MemoryType *allocations = NULL;

/***************************************************************************
 * Start the debugger.
 ***************************************************************************/
void DEBUG_StartDebug(const char *file, unsigned int line) {
	Debug("%s[%u]: debug mode started", file, line);
}

/***************************************************************************
 * Stop the debugger.
 ***************************************************************************/
void DEBUG_StopDebug(const char *file, unsigned int line) {
	MemoryType *mp;
	unsigned int count = 0;

	Debug("%s[%u]: debug mode stopped", file, line);

	if(allocations) {
		Debug("MEMORY: memory leaks follow");
		for(mp = allocations; mp; mp = mp->next) {
			Debug("%u bytes in %s at line %u",
				mp->size, mp->file, mp->line);
			++count;
		}
		if(count == 1) {
			Debug("MEMORY: 1 memory leak");
		} else {
			Debug("MEMORY: %u memory leaks", count);
		}
	} else {
		Debug("MEMORY: no memory leaks");
	}
}

/***************************************************************************
 * Allocate memory and log.
 ***************************************************************************/
void *DEBUG_Allocate(size_t size, const char *file, unsigned int line) {
	MemoryType *mp;

	if(size <= 0) {
		Debug("MEMORY: %s[%u]: Attempt to allocate %d bytes of memory",
			file, line, size);
	}

	mp = (MemoryType*)malloc(sizeof(MemoryType));
	Assert(mp);

	mp->file = (char*)malloc(strlen(file) + 1);
	Assert(mp->file);
	strcpy(mp->file, file);

	mp->line = line;
	mp->size = size;

	mp->pointer = malloc(size);
	Assert(mp->pointer);

	mp->next = allocations;
	allocations = mp;

	return mp->pointer;
}

/***************************************************************************
 * Reallocate memory and log.
 ***************************************************************************/
void *DEBUG_Reallocate(void *ptr, size_t size, const char *file,
	unsigned int line) {

	MemoryType *mp;

	if(size <= 0) {
		Debug("MEMORY: %s[%u]: Attempt to reallocate %d bytes of memory",
			file, line, size);
	}
	if(!ptr) {
		Debug("MEMORY: %s[%u]: Attempt to reallocate NULL pointer. "
			"Calling Allocate...", file, line);
		return DEBUG_Allocate(size, file, line);
	} else {

		for(mp = allocations; mp; mp = mp->next) {
			if(mp->pointer == ptr) {
				mp->file = (char*)realloc(mp->file, strlen(file) + 1);
				Assert(mp->file);
				strcpy(mp->file, file);
				mp->line = line;
				mp->size = size;
				mp->pointer = realloc(ptr, size);
				Assert(mp->pointer);
				return mp->pointer;
			}
		}

		Debug("MEMORY: %s[%u]: Attempt to reallocate unallocated pointer",
			file, line);
		mp = (MemoryType*)malloc(sizeof(MemoryType));
		Assert(mp);
		mp->file = (char*)malloc(strlen(file) + 1);
		strcpy(mp->file, file);
		mp->line = line;
		mp->size = size;
		mp->pointer = malloc(size);
		Assert(mp->pointer);
		mp->next = allocations;
		allocations = mp;

		return mp->pointer;
	}

}

/***************************************************************************
 * Release memory and log.
 ***************************************************************************/
void DEBUG_Release(void *ptr, const char *file, unsigned int line) {
	MemoryType *mp, *last;

	if(!ptr) {
		Debug("MEMORY: %s[%u]: Attempt to delete NULL pointer",
			file, line);
	} else {
		last = NULL;
		for(mp = allocations; mp; mp = mp->next) {
			if(mp->pointer == ptr) {
				if(last) {
					last->next = mp->next;
				} else {
					allocations = mp->next;
				}
				free(mp->file);
				free(mp);
				free(ptr);
				return;
			}
			last = mp;
		}
		Debug("MEMORY: %s[%u]: Attempt to delete unallocated pointer",
			file, line);
		free(ptr);
	}
}


#endif

