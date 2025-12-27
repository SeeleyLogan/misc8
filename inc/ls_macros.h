/* ls_macros.h - general use macros & wrappers - Logan Seeley 2025 */


#ifndef LS_TYPES_INCLUDED
#define LS_TYPES_INCLUDED

	#include <stdint.h>	

	typedef uint8_t		ls_bool_t;

	#define LS_TRUE		1
	#define LS_FALSE	0

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

	#define LS_NULL		LS_CAST(0, ls_void_p)

#endif


#ifndef LS_MACROS_INCLUDED
#define LS_MACROS_INCLUDED

	#if defined(__has_attribute) && __has_attribute(always_inline) && !defined(LS_NO_INLINE) && __STDC_VERSION__ >= 199901L
		#define LS_INLINE inline __attribute__((always_inline))
	#elif __STDC_VERSION__ >= 199901L
		#define LS_INLINE inline
	#else
		#define LS_INLINE
	#endif

	#if defined(__has_attribute) && __has_attribute(unused)
		#define LS_LIBFN __attribute__((unused))  /* library function: dont warn if not used */
	#else
		#define LS_LIBFN
	#endif

	#if defined(__has_attribute) && __has_attribute(LS_USED)
		#define LS_USED __attribute__((used))
	#else
		#define LS_USED
	#endif

	#if defined(__has_attribute) && __has_attribute(deprecated)
		#define LS_DEPRECATED __attribute__((deprecated))
	#else
		#define LS_DEPRECATED
	#endif

	#if defined(__has_attribute) && __has_attribute(hot)
		#define LS_HOT __attribute__((hot))
	#else
		#define LS_HOT
	#endif

	#if defined(__has_attribute) && __has_attribute(cold)
		#define LS_COLD __attribute__((cold))  /* rarely used, not useful */
	#else
		#define LS_COLD
	#endif

	#include <stdio.h>
    #include <stdlib.h>
	#include <string.h>
	#include <assert.h>

	#define LS_EXIT 		exit
	#define LS_ERROR_EXIT(e, ...)	(printf(__VA_ARGS__), LS_EXIT(e))
	#define LS_MEMSET 		memset
	#define LS_MEMCPY 		memcpy

	#define LS_ASSERT	assert

    #define LS_SUCCESS  0
    #define LS_FAIL     1

	#define LS_CAST(v, t) ((t) (v))

	#define LS_FLOOR_LOG2(n) (63u - __builtin_clzll(LS_CAST(n, ls_u64_t)))
	#define LS_CEIL_LOG2(n) (64u - __builtin_clzll(LS_CAST(n, ls_u64_t) - 1))

    #define LS_ROUND_DOWN_TO(n, m) ((n) - ((n) % (m))) 	  	 	/* rounds n down to nearest multiple of m, integers only */
	#define LS_ROUND_UP_TO(n, m) (((n) + (m) - 1) / (m) * (m))  /* rounds n up to nearest multiple of m, integers only */
	
#endif


#ifndef LS_NO_SHORT_NAMES
#define LS_NO_SHORT_NAMES

    #define bool_t 		ls_bool_t

	#define TRUE		LS_TRUE
	#define FALSE		LS_FALSE

	#define u8_t	    ls_u8_t
	#define i8_t        ls_i8_t
	#define u16_t	    ls_u16_t
	#define i16_t       ls_i16_t
	#define u32_t	    ls_u32_t
	#define i32_t       ls_i32_t
	#define u64_t	    ls_u64_t
	#define i64_t       ls_i64_t

	#define f32_t       ls_f32_t
	#define f64_t       ls_f64_t

	#define void_p      ls_void_p
	#define u64_p       ls_u64_p

	#define result_t    ls_result_t

	#define INLINE 		LS_INLINE
	#define LIBFN		LS_LIBFN
	#define USED		LS_USED
	#define DEPRECATED  LS_DEPRECATED
	#define HOT			LS_HOT
	#define COLD		LS_COLD

	#define EXIT        LS_EXIT
	#define ERROR_EXIT		LS_ERROR_EXIT
	#define MEMSET      LS_MEMSET
	#define MEMCPY      LS_MEMCPY

	#define ASSERT		LS_ASSERT

    #define SUCCESS     LS_SUCCESS
    #define FAIL        LS_FAIL

	#define CAST        LS_CAST

	#define FLOOR_LOG2  LS_FLOOR_LOG2
	#define CEIL_LOG2   LS_CEIL_LOG2

    #define ROUND_DOWN_TO  	LS_ROUND_DOWN_TO
	#define ROUND_UP_TO  	LS_ROUND_UP_TO

#endif


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
