/* assembler.h - misc8 assembler header definitions - Logan Seeley 2025 */


#ifndef LS_ASSEMBLER_H
#define LS_ASSEMBLER_H


#include <string.h>
#include <ctype.h>

#include <ls_macros.h>

#include <ls_valloc.h>
#define _ls_chunk_arena_alloca_commit_range ls_valloc_pcommit_range 
#include <ls_chunk_arena.h>


typedef enum
{
    MAIN,
    END_MAIN,
    DATA,
    END_DATA,
    SYMBOL,
    VALUE,
    NOP,
    ADD,
    SUB,
    LOAD,
    LOADJ,
    FETCH,
    FETCHJ,
    WRITE,
    COPYAB,
    COPYBA,
    JMP,
    JC,
    JZ,
    DWRITEI,
    DWRITED,
    HALT
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
    char  *symbol_name;
    u32_t  name_size;

    u32_t  section_offset;
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
    u8_t      *binary;

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


void new_assemble     (i32_t        arg_c,        char  **arg_v);

void parse_args       (assembler_s *assembler,    i32_t   arg_c,  char **arg_v);
void parse_input_flag (assembler_s *assembler,    char   *arg);
void parse_output_flag(assembler_s *assembler,    char   *arg);

void parse_tokens     (assembler_s *assembler);
void compute_token    (assembler_s *assembler,    char   *token);
void verify_tokens    (assembler_s *assembler);

void assemble         (assembler_s *assembler);

void parse_symbols    (assembler_s *assembler);

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
