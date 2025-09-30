/* ls_macros.h - generally useful macros - Logan Seeley 2025 */


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

    typedef char     *  ls_string;

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

    #include <stdio.h>

    #define LS_ERROR(err, ...)    do { fprintf(stderr, __VA_ARGS__); LS_EXIT(err); } while (0)

    #define LS_SUCCESS  0
    #define LS_FAIL     1
    
#endif


#ifndef LS_NO_SHORT_NAMES

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

    #define string      ls_string

	#define result_t    ls_result_t

	#define INLINE      LS_INLINE
	#define USED        LS_USED

	#define CAST        LS_CAST

	#define FLOOR_LOG2  LS_FLOOR_LOG2
	#define CEIL_LOG2   LS_CEIL_LOG2

    #define ROUND_DOWN_TO   LS_ROUND_DOWN_TO
	#define ROUND_UP_TO     LS_ROUND_UP_TO

	#define EXIT        LS_EXIT
	#define MEMSET      LS_MEMSET
	#define MEMCPY      LS_MEMCPY

    #define ERROR       LS_ERROR

    #define SUCCESS     LS_SUCCESS
    #define FAIL        LS_FAIL

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
