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
 *		Define [ls_chunk_arena_alloca_commit_range_] to the
 *		commit function provided by your allocator **before**
 *		including this file.
 *
 *	Functions
 *
 * 		ls_chunk_arena_s ls_chunk_arena_init(ls_void_p memory, ls_u64_t memory_size)
 *			[memory] must be aligned to LS_CHUNK_ARENA_CHUNK_Z
 *			and by divisible by such.
 *
 *		void ls_chunk_arena_fini(ls_chunk_arena_s *chunk_arena) - arena_fini
 *
 *		ls_void_p ls_chunk_arena_get_chunk(ls_chunk_arena_s *chunk_arena)
 *			Returns LS_NULL when memory full
 *
 *		void ls_chunk_arena_delete_chunk(ls_chunk_arena_s *chunk_arena, ls_void_p chunk_p)
 *			[chunk_p] must have been returned by [ls_chunk_arena_get_chunk]
 */


#ifndef LS_CHUNK_ARENA_H
#define LS_CHUNK_ARENA_H


#include "./ls_macros.h"


#define LS_CHUNK_ARENA_CHUNK_Z	4096


/* 
 * IMPORTANT: arena allocator expects programmer to provide memory
 * meaning it requires a function to handle said memory
 * your allocator may provide it for you, simply wrap it in a macro
 * if the chunk allocator's memory is physical and continuous: these can be empty defines
 */
#ifndef ls_chunk_arena_alloca_commit_range_
    #error "chunk arena is missing allocator binding"
#endif


#define LS_CHUNK_ARENA_INDEX_TO_ADDR(chunk_arena, index) (LS_CAST((index) * LS_CHUNK_ARENA_CHUNK_Z + LS_CAST(chunk_arena->memory_, ls_u64_t), ls_void_p))
#define LS_CHUNK_ARENA_ADDR_TO_INDEX(chunk_arena, ptr) ((LS_CAST(ptr, ls_u64_t) - LS_CAST(chunk_arena->memory_, ls_u64_t)) / LS_CHUNK_ARENA_CHUNK_Z)


typedef struct
{
    ls_void_p	memory_;

    ls_u64_t	max_chunk_c_;
	ls_u64_t	chunk_c_;

	ls_u64_t	next_committed_chunk_;
	ls_u64_t	last_deleted_chunk_;
}
ls_chunk_arena_s;


static ls_chunk_arena_s ls_chunk_arena_init						 (ls_void_p			memory, 		ls_u64_t 		memory_size) 	LS_LIBFN;
static void				ls_chunk_arena_fini						 (ls_chunk_arena_s *chunk_arena) 									LS_LIBFN;

static ls_void_p 		ls_chunk_arena_get_chunk				 (ls_chunk_arena_s *chunk_arena)									LS_LIBFN;
static ls_void_p 		ls_chunk_arena_revive_last_deleted_chunk_(ls_chunk_arena_s *chunk_arena) 									LS_LIBFN;
static void				ls_chunk_arena_delete_chunk				 (ls_chunk_arena_s *chunk_arena, 	ls_void_p 		chunk_p)		LS_LIBFN;


static LS_INLINE ls_chunk_arena_s ls_chunk_arena_init(ls_void_p memory, ls_u64_t memory_size)
{
    ls_chunk_arena_s chunk_arena; 

    chunk_arena.memory_        			= memory;

    chunk_arena.max_chunk_c_   			= memory_size / LS_CHUNK_ARENA_CHUNK_Z;
	chunk_arena.chunk_c_				= 0;

	chunk_arena.next_committed_chunk_ 	= 1;
	chunk_arena.last_deleted_chunk_		= 0;

    return chunk_arena;
}

static LS_INLINE void ls_chunk_arena_fini(ls_chunk_arena_s *chunk_arena)
{
    chunk_arena->memory_        		= LS_NULL;

	chunk_arena->max_chunk_c_   		= 0;
	chunk_arena->chunk_c_				= 0;
	
	chunk_arena->next_committed_chunk_ 	= 0;
	chunk_arena->last_deleted_chunk_	= 0;
}


static LS_INLINE ls_void_p ls_chunk_arena_get_chunk(ls_chunk_arena_s *chunk_arena)
{
	if (chunk_arena->max_chunk_c_ == chunk_arena->chunk_c_)
	{
		return LS_NULL;
	}

	chunk_arena->chunk_c_++;
	
	if (!chunk_arena->last_deleted_chunk_)
	{
		ls_void_p chunk_p = LS_CHUNK_ARENA_INDEX_TO_ADDR(chunk_arena, chunk_arena->next_committed_chunk_ - 1);
		ls_chunk_arena_alloca_commit_range_(chunk_arena->memory_, (chunk_arena->next_committed_chunk_ - 1) * LS_CHUNK_ARENA_CHUNK_Z, LS_CHUNK_ARENA_CHUNK_Z);

		chunk_arena->next_committed_chunk_++;

		LS_MEMSET(chunk_p, 0, LS_CHUNK_ARENA_CHUNK_Z);

		return chunk_p;
	}
	else
	{
		ls_void_p chunk_p = ls_chunk_arena_revive_last_deleted_chunk_(chunk_arena);

		LS_MEMSET(chunk_p, 0, LS_CHUNK_ARENA_CHUNK_Z);

		return chunk_p;
	}
}

static LS_INLINE ls_void_p ls_chunk_arena_revive_last_deleted_chunk_(ls_chunk_arena_s *chunk_arena)
{
	ls_u64_p deleted_chunk = LS_CAST(LS_CHUNK_ARENA_INDEX_TO_ADDR(chunk_arena, chunk_arena->last_deleted_chunk_ - 1), ls_u64_p);

	chunk_arena->last_deleted_chunk_ = deleted_chunk[0];

	return deleted_chunk;
}


static LS_INLINE void ls_chunk_arena_delete_chunk(ls_chunk_arena_s *chunk_arena, ls_void_p chunk_p)
{
	ls_u64_t chunk_i;
	
	chunk_i = LS_CHUNK_ARENA_ADDR_TO_INDEX(chunk_arena, chunk_p);

	LS_CAST(chunk_p, ls_u64_p)[0] = chunk_arena->last_deleted_chunk_;

	chunk_arena->last_deleted_chunk_ = chunk_i + 1;
	chunk_arena->chunk_c_--;
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
