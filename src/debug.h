/****************************************************************************
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

#ifndef DEBUG_H
#define DEBUG_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Debug(const char *str, ...);

#ifdef DEBUG

	#define Assert( x ) \
		if(!( x )) {     \
			Debug("ASSERT FAILED: %s[%u]", __FILE__, __LINE__ ); \
			abort(); \
		}

	#define StartDebug() \
		DEBUG_StartDebug( __FILE__, __LINE__ )
	#define StopDebug() \
		DEBUG_StopDebug( __FILE__, __LINE__ )

	#define Allocate( x ) \
		DEBUG_Allocate( x, __FILE__, __LINE__ )
	#define Reallocate( x, y ) \
		DEBUG_Reallocate( x, y, __FILE__, __LINE__ )
	#define Release( x ) \
		DEBUG_Release( x, __FILE__, __LINE__ )

	void DEBUG_StartDebug(const char*, unsigned int);
	void DEBUG_StopDebug(const char*, unsigned int);
	void *DEBUG_Allocate(size_t, const char*, unsigned int);
	void *DEBUG_Reallocate(void*, size_t, const char*, unsigned int);
	void DEBUG_Release(void*, const char*, unsigned int);

#else

	#define Assert( x )           do { } while(0)

	#define StartDebug()          do { } while(0)
	#define StopDebug()           do { } while(0)

	#define Allocate( x )         malloc( (x) )
	#define Reallocate( x, y )    realloc( (x), (y) )
	#define Release( x )          free( (x) )

#endif

#endif

