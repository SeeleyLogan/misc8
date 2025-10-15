/*
 * ls_valloc.h - v1.1.1 - virtual memory allocator - Logan Seeley 2025
 *
 * Documentation
 *
 *  IMPORTANT: When compiling with strict C standards (on linux), _GNU_SOURCE must be defined at compile time.
 *
 *  Functions
 * 
 *      ls_void_p ls_valloc_vmalloc(ls_u64_t *size) - vmalloc
 *          Reserves as many virtual addresses as _ls_memtotal() specifies.
 *          Returns LS_NULL on failure. Max of [2^47/_ls_memtotal()] allocations at once
 *          [size] out reference for virtual allocation size.
 * 
 *      void ls_valloc_vfree(ls_void_p ptr) - vfree
 *          Decommits all committed pages and unreserves all virtual addresses
 *          of [ptr].
 * 
 *      void ls_valloc_pfree(ls_void_p ptr) - pfree
 *          Decommmits all committed pages of [ptr].
 * 
 *      void ls_valloc_pfree_range(ls_void_p ptr, ls_u64_t offset, ls_u64_t range) - pfree_range
 *          Decommits pages past an offset and to a range.
 * 
 *      void ls_valloc_pcommit_range(ls_void_p ptr, ls_u64_t offset, ls_u64_t range) - pcommit_range_win32
 *          Before writing to memory, you must specify where
 *          you plan on writing to[1].
 *
 *      [^] [ptr] must have been returned by [ls_vmalloc()] for each function.
 *      
 *      [1] Note that this function only does anything on windows, use anyways (compatability).
 */


#ifndef LS_VALLOC_H
#define LS_VALLOC_H


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


#ifndef LS_VALLOC_NO_SHORT_NAMES

    #define vmalloc         ls_valloc_vmalloc
    #define vfree           ls_valloc_vfree
    #define pfree           ls_valloc_pfree
    #define pfree_range     ls_valloc_pfree_range
    #define pcommit_range   ls_valloc_pcommit_range

#endif


#include <stdio.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/mman.h>
    #include <unistd.h>
#endif


static ls_void_p   ls_valloc_vmalloc    (ls_u64_p  size)                                    LS_USED;
static void        ls_valloc_vfree      (ls_void_p ptr)                                     LS_USED;
static void        ls_valloc_pfree      (ls_void_p ptr)                                     LS_USED;
static void        ls_valloc_pfree_range(ls_void_p ptr, ls_u64_t offset, ls_u64_t range)    LS_USED;

#ifdef _WIN32
    static void _ls_valloc_pcommit_range_win32(ls_void_p ptr, ls_u64_t offset, ls_u64_t range) LS_USED;

    #define ls_valloc_pcommit_range(ptr, offset, range) _ls_valloc_pcommit_range_win32(ptr, offset, range)
#else
    #define ls_valloc_pcommit_range(ptr, offset, range)  /* still good practice to call this */
#endif

static ls_u64_t _ls_valloc_page_size(void);
static ls_u64_t _ls_valloc_memtotal(void);


static LS_INLINE ls_void_p ls_valloc_vmalloc(ls_u64_p size)
{
    ls_void_p buf;

    *size = _ls_valloc_memtotal();

    #ifdef _WIN32
        return VirtualAlloc(LS_NULL, *size, MEM_RESERVE, PAGE_READWRITE);
    #else
        buf = mmap(LS_NULL, *size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        return (buf == MAP_FAILED) ? LS_NULL : buf;
    #endif
}


static LS_INLINE void ls_valloc_vfree(ls_void_p ptr)
{
    #ifdef _WIN32
        VirtualFree(ptr, _ls_valloc_memtotal(), MEM_RELEASE);
    #else
        munmap(ptr, _ls_valloc_memtotal());
    #endif
}


static LS_INLINE void ls_valloc_pfree(ls_void_p ptr)
{
    #ifdef _WIN32
        VirtualFree(ptr, _ls_valloc_memtotal(), MEM_DECOMMIT);
    #else
        madvise(ptr, _ls_valloc_memtotal(), MADV_DONTNEED);
    #endif
}


static LS_INLINE void ls_valloc_pfree_range(ls_void_p ptr, ls_u64_t offset, ls_u64_t range)
{
    ls_u64_t page_size = _ls_valloc_page_size();

    offset = LS_ROUND_DOWN_TO(offset, page_size) + page_size;
    range  = LS_ROUND_DOWN_TO(range, page_size);
    
    if (!range || offset >= _ls_valloc_memtotal())
        return;  /* nothing to do */
    
    if (offset + range > _ls_valloc_memtotal())
        range = _ls_valloc_memtotal() - offset;
    
    #ifdef _WIN32
        VirtualFree(LS_CAST(LS_CAST(ptr, ls_u64_t) + offset, ls_void_p), range, MEM_DECOMMIT);
    #else
        madvise(LS_CAST(LS_CAST(ptr, ls_u64_t) + offset, ls_void_p), range, MADV_DONTNEED);
    #endif
}


#ifdef _WIN32

static LS_INLINE void _ls_valloc_pcommit_range_win32(ls_void_p ptr, ls_u64_t offset, ls_u64_t range)
{
    ls_u64_t page_size = _ls_valloc_page_size();
    
    offset = LS_ROUND_DOWN_TO(offset, page_size);
    range  = LS_ROUND_DOWN_TO(range, page_size);
    
    if (!range || offset >= _ls_valloc_memtotal())
        return;  /* nothing to do */
    
    if (offset + range > _ls_valloc_memtotal())
        range = _ls_valloc_memtotal() - offset;
    
    VirtualAlloc(LS_CAST(LS_CAST(ptr, ls_u64_t) + offset, ls_void_p), range, MEM_COMMIT, PAGE_READWRITE);
}
#endif


static LS_INLINE ls_u64_t _ls_valloc_page_size(void)
{
    #ifdef _WIN32
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
    
        return sysinfo.dwPageSize;
    #else
        return sysconf(_SC_PAGESIZE);
    #endif
}


static LS_INLINE ls_u64_t _ls_valloc_memtotal(void)
{
    #ifdef _WIN32
        MEMORYSTATUS memstat;
        GlobalMemoryStatus(&memstat);
    
        return memstat.dwTotalPhys;
    #else
        FILE*  meminfo_f = fopen("/proc/meminfo", "rb");
        ls_u64_t memtotal;
    
        fscanf(meminfo_f, "MemTotal:%zu", &memtotal);
    
        fclose(meminfo_f);
    
        return memtotal * 1024;
    #endif
}


#endif  /* #ifdef LS_VALLOC_H */


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
