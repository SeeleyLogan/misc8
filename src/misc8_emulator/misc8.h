/* ls_misc8.h - misc8 emulator header definitions - Logan Seeley 2025 */


#ifndef LS_MISC8_H
#define LS_MISC8_H


#include <stdio.h>
#include <string.h>

#include "./ls_macros.h"


typedef struct misc8_s
{
    u64_t  cycles_to_complete;
    
    char   program[4096];
    char   program_file_name[256];

    u16_t  program_counter;
    u16_t  m_register;
    u16_t  j_register;
    u8_t   a_register;
    u8_t   b_register;

    bool_t carry_flag;
    bool_t zero_flag;

    bool_t program_loaded;
}
misc8_t;


#define NOP     0x0 
#define ADD     0x1
#define SUB     0x2
#define LOAD    0x3
#define LOADJ   0x4
#define FETCH   0x5
#define FETCHJ  0x6
#define WRITE   0x7
#define COPYAB  0x8
#define COPYBA  0x9
#define JMP     0xA
#define JC      0xB
#define JZ      0xC
#define DWRITEI 0xD
#define DWRITED 0xE
#define HALT    0xF


#define ADDR_MASK 0xFFF


misc8_t new_misc8(i32_t arg_c, string *arg_v);

void    parse_args(misc8_t *misc8, i32_t arg_c, string *arg_v);
void    parse_program_flag(misc8_t *misc8, string arg);

void    load_program(misc8_t *misc8);
void    run_program(misc8_t *misc8);
bool_t  execute_instruction(misc8_t *misc8, u8_t instruction);
void    get_instruction_name(misc8_t *misc8, string name);


#include "./misc8.c"


#endif  /* #ifndef LS_MISC8_H */


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
