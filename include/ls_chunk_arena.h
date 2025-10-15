/*
 * ls_chunk_arena.h - v1.0.3 - chunk arena allocator - Logan Seeley 2025
 *
 * Documentation
 *
 *	Behaviour & Safety
 *
 *		This arena allocator expects memory to be provided to it.
 *		The lifetime of any [ls_chunk_arena_s] must be less than
 *		that of the memory provided to it.
 *
 *		The arena does not free deleted chunks; it tries to reuse
 *		them. Meaning this allocator does not work well with
 *		programs that fluctuate greatly with memory usage.
 *
 *	Usage
 *
 *		Memory allocators compatible with this arena allocator
 *		likely require memory to be committed before used.
 *		Define [_ls_chunk_arena_alloca_commit_range] to the
 *		commit function provided by your allocator **before**
 *		including this file.
 *
 *	Functions
 *
 * 		ls_chunk_arena_s ls_chunk_arena_init(ls_void_p memory, ls_u64_t memory_size, ls_u64_t chunk_size) - arena_init
 *			[memory] must be aligned to [chunk_size]
 *			and by divisible by such. [chunk_size]
 *			must be a power of 2.
 *
 *		void ls_chunk_arena_fini(ls_chunk_arena_s *chunk_arena) - arena_fini
 *
 *		ls_void_p ls_chunk_arena_get_chunk(ls_chunk_arena_s *chunk_arena, ls_u32_t *status) - arena_get_chunk
 *			[status] is out
 *			[*status] |= LS_CHUNK_ARENA_SUCCESS
 *			[*status] |= LS_CHUNK_ARENA_MEM_FULL -> could not fetch chunk: none left. [return] will also be LS_NULL
 *
 *		void ls_chunk_arena_delete_chunk(ls_chunk_arena_s *chunk_arena, ls_void_p chunk_p) - arena_delete_chunk
 *			[chunk_p] must have been returned by [ls_chunk_arena_get_chunk]
 */


#ifndef LS_CHUNK_ARENA_H
#define LS_CHUNK_ARENA_H


#ifndef LS_TYPES_INCLUDED
#define LS_TYPES_INCLUDED

	#include <stdint.h>	

	typedef uint8_t		ls_bool_t;

	typedef uint8_t		ls_u8_t;
	typedef int8_t		ls_i8_t;
	typedef uint16_t	ls_u16_t;
	typedef int16_t		ls_i16_t;
	typedef uint32_t	ls_u32_t;
	typedef int32_t		ls_i32_t;
	typedef uint64_t	ls_u64_t;
	typedef int64_t		ls_i64_t;

	typedef float       ls_f32_t;
	typedef double      ls_f64_t;

	typedef void 	 *  ls_void_p;
	typedef ls_u64_t *	ls_u64_p;

	typedef ls_u32_t	ls_result_t;

	#define LS_NULL		0

#endif

#ifndef LS_MACROS_INCLUDED
#define LS_MACROS_INCLUDED

	#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && (defined(__GNUC__) || defined(__clang__))
		#define LS_INLINE inline __attribute__((always_inline, unused))
	#else
		#define LS_INLINE
	#endif

	#if defined(__GNUC__) || defined(__clang__)
		#define LS_USED __attribute__((unused))
	#else
		#define LS_USED
	#endif

	#define LS_CAST(v, t) ((t) (v))

	#define LS_FLOOR_LOG2(n) (63 - __builtin_clzll(n))
	#define LS_CEIL_LOG2(n) (64 - __builtin_clzll((n) - 1))

    #define LS_ROUND_DOWN_TO(n, m) ((n) - ((n) % (m))) 	  		/* rounds n down to nearest multiple of m, integers only */
	#define LS_ROUND_UP_TO(n, m) (((n) + (m) - 1) / (m) * (m))  /* rounds n up to nearest multiple of m, integers only */

	#include <stdlib.h>
	#include <string.h>

	#define LS_EXIT 	exit
	#define LS_MEMSET 	memset
	#define LS_MEMCPY 	memcpy
	
#endif


#ifndef LS_CHUNK_ARENA_NO_SHORT_NAMES
    
	#define arena_s    	 	   	ls_chunk_arena_s
    #define arena_init		 	ls_chunk_arena_init
	#define arena_fini		 	ls_chunk_arena_fini
	#define arena_get_chunk    	ls_chunk_arena_get_chunk
	#define arena_delete_chunk	ls_chunk_arena_delete_chunk

#endif


#define LS_CHUNK_ARENA_SUCCESS  0
#define LS_CHUNK_ARENA_MEM_FULL	1


/* 
 * IMPORTANT: arena allocator expects programmer to provide memory
 * meaning it requires a function to handle said memory
 * your allocator may provide it for you, simply wrap it in a macro
 * if the chunk allocator's memory is physical and continuous: these can be empty defines
 */
#ifndef _ls_chunk_arena_alloca_commit_range
    #error "chunk arena is missing allocator binding"
#endif


#define LS_CHUNK_ARENA_INDEX_TO_ADDR(chunk_arena, index) (LS_CAST((index) * chunk_arena->_chunk_size + LS_CAST(chunk_arena->_memory, ls_u64_t), ls_void_p))
#define LS_CHUNK_ARENA_ADDR_TO_INDEX(chunk_arena, ptr) ((LS_CAST(ptr, ls_u64_t) - LS_CAST(chunk_arena->memory, ls_u64_t)) / chunk_arena->_chunk_size)


typedef struct
{
    ls_void_p	_memory;

    ls_u64_t	_max_chunk_c;
	ls_u64_t	_chunk_size;
	ls_u64_t	_chunk_c;

	ls_u64_t	_next_committed_chunk;
	ls_u64_t	_last_deleted_chunk;
}
ls_chunk_arena_s;


static ls_chunk_arena_s 	ls_chunk_arena_init							(ls_void_p			 memory, 		ls_u64_t 		memory_size, 	ls_u64_t 	chunk_size) LS_USED;
static void				 	ls_chunk_arena_fini							(ls_chunk_arena_s 	*chunk_arena) 															LS_USED;

static ls_void_p 			ls_chunk_arena_get_chunk					(ls_chunk_arena_s   *chunk_arena, 	ls_result_t    *status)									LS_USED;
static ls_void_p 			_ls_chunk_arena_revive_last_deleted_chunk	(ls_chunk_arena_s 	*chunk_arena) 															LS_USED;
static void					ls_chunk_arena_delete_chunk					(ls_chunk_arena_s 	*chunk_arena, 	ls_void_p 		chunk_p)								LS_USED;


static LS_INLINE ls_chunk_arena_s ls_chunk_arena_init(ls_void_p memory, ls_u64_t memory_size, ls_u64_t chunk_size)
{
    ls_chunk_arena_s chunk_arena; 

    chunk_arena._memory        			= memory;

    chunk_arena._max_chunk_c   			= memory_size >> __builtin_ctzll(chunk_size);
	chunk_arena._chunk_size				= chunk_size;
	chunk_arena._chunk_c				= 0;

	chunk_arena._next_committed_chunk 	= 1;
	chunk_arena._last_deleted_chunk		= 0;

    return chunk_arena;
}

static LS_INLINE void ls_chunk_arena_fini(ls_chunk_arena_s *chunk_arena)
{
    chunk_arena->_memory        		= LS_NULL;

	chunk_arena->_max_chunk_c   		= LS_NULL;
	chunk_arena->_chunk_size			= LS_NULL;
	chunk_arena->_chunk_c				= LS_NULL;
	
	chunk_arena->_next_committed_chunk 	= LS_NULL;
	chunk_arena->_last_deleted_chunk	= LS_NULL;
}


static LS_INLINE ls_void_p ls_chunk_arena_get_chunk(ls_chunk_arena_s *chunk_arena, ls_result_t *status)
{
	if (chunk_arena->_max_chunk_c == chunk_arena->_chunk_c)
	{
		*status |= LS_CHUNK_ARENA_MEM_FULL;
		return LS_NULL;
	}
	else
		*status |= LS_CHUNK_ARENA_SUCCESS;

	chunk_arena->_chunk_c++;
	
	if (!chunk_arena->_last_deleted_chunk)
	{
		ls_void_p chunk_p = LS_CHUNK_ARENA_INDEX_TO_ADDR(chunk_arena, chunk_arena->_next_committed_chunk - 1);
		_ls_chunk_arena_alloca_commit_range(chunk_arena->_memory, chunk_p, chunk_arena->_chunk_size);

		chunk_arena->_next_committed_chunk++;

		return chunk_p;
	}
	else
		return _ls_chunk_arena_revive_last_deleted_chunk(chunk_arena);
}

static LS_INLINE ls_void_p _ls_chunk_arena_revive_last_deleted_chunk(ls_chunk_arena_s *chunk_arena)
{
	ls_u64_p deleted_chunk = LS_CAST(LS_CHUNK_ARENA_INDEX_TO_ADDR(chunk_arena, chunk_arena->_last_deleted_chunk - 1), ls_u64_p);

	chunk_arena->_last_deleted_chunk = deleted_chunk[0];

	return deleted_chunk;
}


static LS_INLINE void ls_chunk_arena_delete_chunk(ls_chunk_arena_s *chunk_arena, ls_void_p chunk_p)
{
	ls_u64_t chunk_i;
	
	chunk_i = (LS_CAST(chunk_p, ls_u64_t) - LS_CAST(chunk_arena->_memory, ls_u64_t)) >> __builtin_ctzll(chunk_arena->_chunk_size);

	LS_CAST(chunk_p, ls_u64_p)[0] = chunk_arena->_last_deleted_chunk;

	chunk_arena->_last_deleted_chunk = chunk_i + 1;
	chunk_arena->_chunk_c--;
}


#endif  /* #ifndef LS_CHUNK_ARENA_H */


/*
 * Copyright (C) 2025  Logan Seeley
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
