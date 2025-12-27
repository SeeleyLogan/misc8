/* emulator.c - misc8 emulator implementation - Logan Seeley 2025 */


#include "./emulator.h"


misc8_t new_misc8(i32_t arg_c, char **arg_v)
{
    misc8_t misc8;
    misc8.cycles_to_complete = 0;
    misc8.program_counter    = 0;
    misc8.is_program_loaded  = FALSE;

    parse_args(&misc8, arg_c, arg_v);

    if (!misc8.is_program_loaded)
        ERROR_EXIT(0x6, "error: no program provided.");

    load_program(&misc8);

    return misc8;
}


void parse_args(misc8_t *misc8, i32_t arg_c, char **arg_v)
{
    for (u8_t i = 0; i < arg_c; i++)
    {
        u64_t arg_l = strlen(arg_v[i]);

        if ((arg_l < 2) || (arg_v[i][0] != '-'))
            ERROR_EXIT(0x1, "error: incomplete argument: '%s'", arg_v[i]);

        switch (arg_v[i][1])
        {
        case 'i':
            parse_program_flag(misc8, arg_v[i]);
        break;

        default:
            ERROR_EXIT(0x2, "error: unkown flag '%c'", arg_v[i][1]);
        break;
        }
    }
}

void parse_program_flag(misc8_t *misc8, char *arg)
{
    u64_t program_file_name_l;

    if (misc8->is_program_loaded)
        ERROR_EXIT(0x3, "error: too many programs provided.\n");

    if (strlen(arg) <= 2)
        ERROR_EXIT(0x4, "error: program argument has no value.\n");

    program_file_name_l = strlen(arg) - 2;
    if (program_file_name_l >= sizeof(misc8->program_file_name))
        ERROR_EXIT(0x5, "error: program file name too long.\n");

    MEMCPY(misc8->program_file_name, arg + 2, program_file_name_l);
    misc8->program_file_name[program_file_name_l] = 0;

    misc8->is_program_loaded = TRUE;
}


void load_program(misc8_t *misc8)
{
    u64_t program_file_l;
    u64_t bytes_read;

    FILE *program_file = fopen(misc8->program_file_name, "rb");
    if (program_file == NULL)
        ERROR_EXIT(0x7, "error: could not open file.");

    fseek(program_file, 0, SEEK_END);
    program_file_l = ftell(program_file);
    if (program_file_l > sizeof(misc8->program))
        ERROR_EXIT(0x8, "error: program too large");

    fseek(program_file, 0, SEEK_SET);

    bytes_read = fread(misc8->program, sizeof(char), program_file_l, program_file);
    if (bytes_read != program_file_l)
        ERROR_EXIT(0x9, "error: couldn't load entire file.");

    fclose(program_file);
}

void run_program(misc8_t *misc8)
{
    printf("Registers show value after instruction execution.\n\n");
    printf("pc       instruction   a-register   b-register   j-register     carry   zero\n");
    printf("------------------------------------------------------------------------------\n");

    while (TRUE)  /* this is meant to be scary  */
    {
        u8_t instruction = misc8->program[misc8->program_counter & ADDR_MASK];

        char instruction_name[12];
        get_instruction_name(misc8, instruction_name);

        /* program counter: instruction */
        printf("0x%03X:   %*s  ", misc8->program_counter & ADDR_MASK, -12, instruction_name);

        misc8->program_counter++;

        if (!execute_instruction(misc8, instruction) || misc8->cycles_to_complete > 2000)  /* returns false on error or end of program */
            break;

        /* a-register b-register j-register flags */
        printf("0x%02X | %*u   ", misc8->a_register, -3, misc8->a_register);
        printf("0x%02X | %*u   ", misc8->b_register, -3, misc8->b_register);
        printf("0x%03X | %*u   ", misc8->j_register, -4, misc8->j_register & ADDR_MASK);
        printf("%s   %s\n", (misc8->carry_flag) ? "SET  " : "UNSET", (misc8->zero_flag) ? "SET  " : "UNSET");
    }

    printf("\n\nProgram completed after %llu clock cycles.", misc8->cycles_to_complete);
}

bool_t execute_instruction(misc8_t *misc8, u8_t instruction)
{
    switch (instruction)
    {
    case NOP:
        misc8->cycles_to_complete += 5;
    break;

    case ADD:
        if ((CAST(misc8->a_register, u64_t) + CAST(misc8->b_register, u64_t)) > 255)
            misc8->carry_flag = TRUE;
        else
            misc8->carry_flag = FALSE;

        misc8->a_register += misc8->b_register;

        if (misc8->a_register == 0)
            misc8->zero_flag = TRUE;
        else
            misc8->zero_flag = FALSE;

        misc8->cycles_to_complete += 6;
    break;

    case SUB:
        if (CAST(CAST(misc8->a_register, u8_t) + CAST(~misc8->a_register + 1, u8_t), u16_t) > 255)
            misc8->carry_flag = TRUE;
        else
            misc8->carry_flag = FALSE;

        misc8->a_register -= misc8->b_register;

        if (misc8->a_register == 0)
            misc8->zero_flag = TRUE;
        else
            misc8->zero_flag = FALSE;

        misc8->cycles_to_complete += 6;
    break;

    case LOAD:
        misc8->a_register = misc8->program[misc8->program_counter & ADDR_MASK];
        misc8->program_counter++;

        misc8->cycles_to_complete += 8;
    break;

    case LOADJ:
        misc8->j_register = (CAST(misc8->program[(misc8->program_counter + 1) & ADDR_MASK], u16_t) << 8) + CAST(misc8->program[misc8->program_counter & ADDR_MASK], u8_t);
        misc8->program_counter += 2;

        misc8->cycles_to_complete += 11;
    break;

    case FETCH:
        misc8->a_register = misc8->program[misc8->j_register & ADDR_MASK];
    
        misc8->cycles_to_complete += 8;
    break;

    case FETCHJ:
        misc8->j_register = (CAST(misc8->program[(misc8->j_register + 1) & ADDR_MASK], u16_t) << 8) + CAST(misc8->program[misc8->j_register & ADDR_MASK], u8_t);
        misc8->a_register = misc8->program[misc8->j_register];
        
        misc8->cycles_to_complete += 18;
    break;

    case WRITE:
        misc8->program[misc8->j_register] = misc8->a_register;

        misc8->cycles_to_complete += 8;
    break;

    case COPYAB:
        misc8->b_register = misc8->a_register;

        misc8->cycles_to_complete += 6;
    break;

    case COPYBA:
        misc8->a_register = misc8->b_register;

        misc8->cycles_to_complete += 6;
    break;

    case JMP:
        misc8->program_counter = misc8->j_register;

        misc8->cycles_to_complete += 7;
    break;

    case JC:
        if (misc8->carry_flag)
        {
            misc8->program_counter = misc8->j_register;

            misc8->cycles_to_complete += 7;
        }
        else
            misc8->cycles_to_complete += 5;
    break;

    case JZ:
        if (misc8->zero_flag)
        {
            misc8->program_counter = misc8->j_register;

            misc8->cycles_to_complete += 7;
        }
        else
            misc8->cycles_to_complete += 5;
    break;

    case DWRITEI:
        misc8->cycles_to_complete += 6;
    break;

    case DWRITED:
        misc8->cycles_to_complete += 6;
    break;

    case HALT:
        misc8->cycles_to_complete += 32;
    return FALSE;

    default:
        printf("error: unknown instruction");
    return FALSE;
    }

    return TRUE;
}

void get_instruction_name(misc8_t *misc8, char *name)
{
    u8_t instruction = misc8->program[misc8->program_counter & ADDR_MASK];

    switch (instruction)
    {
    case NOP:
        MEMCPY(name, "nop", 4);
    return;

    case ADD:
        MEMCPY(name, "add", 4);
    return;

    case SUB:
        MEMCPY(name, "sub", 4);
    return;

    case LOAD:
        sprintf(name, "load 0x%02X", CAST(misc8->program[(misc8->program_counter + 1) & ADDR_MASK], u8_t));
    return;

    case LOADJ:
        sprintf(name, "loadj 0x%01X%02X", CAST(misc8->program[(misc8->program_counter + 2) & ADDR_MASK], u8_t), CAST(misc8->program[(misc8->program_counter + 1) & ADDR_MASK], u8_t));
    return;

    case FETCH:
        MEMCPY(name, "fetch", 6);
    return;

    case FETCHJ:
        MEMCPY(name, "fetchj", 7);
    return;

    case WRITE:
        MEMCPY(name, "write", 6);
    return;

    case COPYAB:
        MEMCPY(name, "copyab", 7);
    return;

    case COPYBA:
        MEMCPY(name, "copyba", 7);
    return;

    case JMP:
        MEMCPY(name, "jmp", 4);
    return;

    case JC:
        MEMCPY(name, "jc", 3);
    return;

    case JZ:
        MEMCPY(name, "jz", 3);
    return;

    case DWRITEI:
        MEMCPY(name, "dwritei", 8);
    return;

    case DWRITED:
        MEMCPY(name, "dwrited", 8);
    return;

    case HALT:
        MEMCPY(name, "halt", 5);
    return;

    default:
        MEMCPY(name, "???", 4);
    return;
    }
}


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
