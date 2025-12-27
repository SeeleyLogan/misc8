/* assembler.h - misc8 assembler header definitions - Logan Seeley 2025 */


#ifndef LS_ASSEMBLER_H
#define LS_ASSEMBLER_H


#include <string.h>
#include <ctype.h>

#include <ls_macros.h>

#include <ls_valloc.h>
#define vmalloc ls_valloc_vmalloc
#define vfree ls_valloc_vfree
#define pcommit_range   ls_valloc_pcommit_range
#define ls_chunk_arena_alloca_commit_range_ pcommit_range 
#include <ls_chunk_arena.h>
#define arena_s         ls_chunk_arena_s
#define arena_init      ls_chunk_arena_init
#define arena_fini      ls_chunk_arena_fini
#define arena_get_chunk ls_chunk_arena_get_chunk


#define SYMBOL_NOT_FOUND    1


/* instruction enum values correspond to instruction op-code */
typedef enum
{
    NOP      = 0x0,
    ADD      = 0x1,
    SUB      = 0x2,
    LOAD     = 0x3,
    LOADJ    = 0x4,
    FETCH    = 0x5,
    FETCHJ   = 0x6,
    WRITE    = 0x7,
    COPYAB   = 0x8,
    COPYBA   = 0x9,
    JMP      = 0xA,
    JC       = 0xB,
    JZ       = 0xC,
    DWRITEI  = 0xD,
    DWRITED  = 0xE,
    HALT     = 0xF,
    MAIN     = 0x10,
    END_MAIN = 0x11,
    DATA     = 0x12,
    END_DATA = 0x13,
    SYMBOL   = 0x14,
    VALUE    = 0x15,
}
token_e;

typedef struct
{
    token_e  type;
    char    *metadata;
}
token_s;

/* try to keep size at 16 bytes, aligns well with chunk size */
typedef struct
{
    char  *name;
    u32_t  name_z;

    u32_t  value;
}
symbol_s;

typedef struct
{
    u8_t     *body;
    u64_t     body_l;

    symbol_s *symbol_v;
    u32_t     symbol_c;
}
section_s;

typedef struct
{
    arena_s    chunk_arena;

    FILE      *binary_file;

    FILE      *assembly_file;
    char      *assembly;

    token_s   *token_v;
    u64_t      max_tokens;
    u64_t      token_c;

    section_s  main;
    section_s  data;
    bool_t     has_data_section;
}
assembler_s;


void  new_assemble     (i32_t        arg_c,        char     **arg_v);

void  parse_args       (assembler_s *assembler,    i32_t      arg_c,        char     **arg_v);
void  parse_input_flag (assembler_s *assembler,    char      *arg);
void  parse_output_flag(assembler_s *assembler,    char      *arg);

void  parse_tokens     (assembler_s *assembler);
void  compute_token    (assembler_s *assembler,    char      *token);
void  verify_tokens    (assembler_s *assembler);

void  parse_symbols    (assembler_s *assembler);
u16_t get_symbol_value (assembler_s *assembler,    section_s *section,      char   *symbol_name,     result_t *status);

void  assemble         (assembler_s *assembler);
void  write_binary     (assembler_s *assembler);

u16_t parse_value      (char *string_value);


#include "./assembler.c"


#endif  /* #ifndef LS_ASSEMBLER_H */


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
