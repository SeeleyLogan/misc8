#include "./assembler.h"


void new_assemble(i32_t arg_c, char **arg_v)
{
    assembler_s assembler;


    u64_t  memory_size;
    void_p memory = vmalloc(&memory_size);
    if (memory == NULL)
        ERROR_EXIT(0x1, "error: could not allocate memory.\n");


    assembler.chunk_arena      = arena_init(memory, memory_size);

    assembler.binary_file      = NULL;
    assembler.assembly_file    = NULL;

    assembler.has_data_section = FALSE;

    parse_args(&assembler, arg_c, arg_v);
    parse_tokens(&assembler);
    parse_symbols(&assembler);

    assemble(&assembler);

    write_binary(&assembler);


    fclose(assembler.binary_file);
    fclose(assembler.assembly_file);

    arena_fini(&assembler.chunk_arena);
    vfree(memory);
    vfree(assembler.assembly);
}


void parse_args(assembler_s *assembler, i32_t arg_c, char **arg_v)
{
    for (u8_t i = 0; i < arg_c; i++)
    {
        u64_t arg_l = strlen(arg_v[i]);

        if ((arg_l < 2) || (arg_v[i][0] != '-'))
            ERROR_EXIT(0x1, "error: incomplete argument: '%s'\n", arg_v[i]);
        
        switch (arg_v[i][1])
        {
        case 'i':
            parse_input_flag(assembler, arg_v[i]);
        break;

        case 'o':
            parse_output_flag(assembler, arg_v[i]);
        break;

        default:
            ERROR_EXIT(0x1, "error: unkown flag '%c'\n", arg_v[i][1]);
        break;
        }
    }

    if (assembler->assembly_file == NULL)
        ERROR_EXIT(0x1, "error: no input file provided.");
}

void parse_input_flag(assembler_s *assembler, char *arg)
{
    u64_t mem_z;
    u64_t file_z;
    u64_t read_z;

    if (assembler->assembly_file != NULL)
        ERROR_EXIT(0x1, "error: too many assembly files provided.\n");

    if (strlen(arg) <= 2)
        ERROR_EXIT(0x1, "error: program assembly has no value.\n");

    assembler->assembly_file = fopen(&(arg[2]), "rb");
    if (assembler->assembly_file == NULL)
        ERROR_EXIT(0x1, "error: could not open assembly file.\n");

    assembler->assembly = vmalloc(&mem_z);
    if (assembler->assembly == NULL)
        ERROR_EXIT(0x1, "error: could not allocate memory for input file.\n");

    fseek(assembler->assembly_file, 0, SEEK_END);
    file_z = ftell(assembler->assembly_file);
    rewind(assembler->assembly_file);

    pcommit_range(assembler->assembly, 0, file_z);
    read_z = fread(assembler->assembly, 1, file_z, assembler->assembly_file);
    
    if (read_z != file_z)
        ERROR_EXIT(0x1, "error: could not load input file to memory.\n");
    assembler->assembly[read_z] = '\0';
    
    assembler->token_v    = CAST(LS_ROUND_UP_TO(CAST(assembler->assembly, u64_t) + read_z + 1, sizeof(token_s)), token_s *);
    assembler->max_tokens = (CAST(assembler->token_v, u64_t) - CAST(assembler->assembly, u64_t)) / sizeof(token_s); 
    assembler->token_c    = 0;
}

void parse_output_flag(assembler_s *assembler, char *arg)
{
    if (assembler->binary_file != NULL)
        ERROR_EXIT(0x1, "error: too many output files provided.\n");

    if (strlen(arg) <= 2)
        ERROR_EXIT(0x1, "error: output argument has no value.\n");

    assembler->binary_file = fopen(&(arg[2]), "w");
    if (assembler->binary_file == NULL)
        ERROR_EXIT(0x1, "error: could not create output file.\n");
}


void parse_tokens(assembler_s *assembler)
{    
    char *token = strtok(assembler->assembly, "\n");

    do
    {
        char *comment_start = strstr(token, ";");
        if (comment_start != NULL)
            *comment_start = '\0';

        u64_t len = strlen(token);
        while (len > 0 && isspace((unsigned char)token[len - 1]))
            token[--len] = '\0';

        while (isspace(*token))
            token++;

        if (strlen(token) == 0)
        {
            token = strtok(NULL, "\n");
            continue;
        }

        compute_token(assembler, token);

        token = strtok(NULL, "\n");
    }
    while (token != NULL);

    verify_tokens(assembler);
}

void compute_token(assembler_s *assembler, char *token)
{
    if (strcmp(token, ".main") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = MAIN };
    else if (strcmp(token, ".end_main") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = END_MAIN };
    else if (strcmp(token, ".data") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = DATA };
    else if (strcmp(token, ".end_data") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = END_DATA };
    else if (token[0] == '*')
        assembler->token_v[assembler->token_c] = (token_s) { .type = SYMBOL, .metadata = &(token[1]) };
    else if (strcmp(token, "nop") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = NOP };
    else if (strcmp(token, "add") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = ADD };
    else if (strcmp(token, "sub") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = SUB };
    else if (strncmp(token, "load", 4) == 0 && strncmp(token, "loadj", 5) != 0)
    {
        char *symbol = token + 4;

        while (isspace(*symbol))
            symbol++;

        assembler->token_v[assembler->token_c] = (token_s) { .type = LOAD, .metadata = symbol };
    }
    else if (strncmp(token, "loadj", 5) == 0)
    {
        char *symbol = token + 5;
        while (isspace(*symbol))
            symbol++;
        
        assembler->token_v[assembler->token_c] = (token_s) { .type = LOADJ, .metadata = symbol };
    }
    else if (strcmp(token, "fetch") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = FETCH };
    else if (strcmp(token, "fetchj") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = FETCHJ };
    else if (strcmp(token, "write") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = WRITE };
    else if (strcmp(token, "copyab") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = COPYAB };
    else if (strcmp(token, "copyba") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = COPYBA };
    else if (strcmp(token, "jmp") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = JMP };
    else if (strcmp(token, "jc") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = JC };
    else if (strcmp(token, "jz") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = JZ };
    else if (strcmp(token, "dwritei") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = DWRITEI };
    else if (strcmp(token, "dwrited") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = DWRITED };
    else if (strcmp(token, "halt") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = HALT };
    else
        assembler->token_v[assembler->token_c] = (token_s) { .type = VALUE, .metadata = token };

    assembler->token_c++;
}

void verify_tokens(assembler_s *assembler)
{
    bool_t in_main_section  = FALSE;
    bool_t in_data_section  = FALSE;
    bool_t has_main_section = FALSE;
    bool_t has_data_section = FALSE;

    for (u64_t i = 0; i < assembler->token_c; i++)
    {
        switch (assembler->token_v[i].type)
        {
        case MAIN:
            if (in_main_section || in_data_section)
                ERROR_EXIT(0x1, "error: main section inside another section.\n");
            if (has_main_section)
                ERROR_EXIT(0x1, "error: more than one main section.\n");

            in_main_section  = TRUE;
            has_main_section = TRUE;
            assembler->main.body   = arena_get_chunk(&assembler->chunk_arena);
            assembler->main.body_l = 0;
        break;

        case END_MAIN:
            if (!in_main_section)
                ERROR_EXIT(0x1, "error: ending non-main section.\n");

            in_main_section = FALSE;
        break;

        case DATA:
            if (in_main_section || in_data_section)
                ERROR_EXIT(0x1, "error: data section inside another section.\n");
            if (has_data_section)
                ERROR_EXIT(0x1, "error: more than one data section.\n");

            in_data_section  = TRUE;
            has_data_section = TRUE;
            assembler->data.body        = arena_get_chunk(&assembler->chunk_arena);
            assembler->data.body_l      = 0;
            assembler->has_data_section = TRUE;
        break;

        case END_DATA:
            if (!in_data_section)
                ERROR_EXIT(0x1, "error: ending non-data section.\n");

            in_data_section = FALSE;
        break;

        default:
            if (!in_main_section && !in_data_section)
                ERROR_EXIT(0x1, "error: symbol outside of section.\n");
        }
    }

    if (!has_main_section)
        ERROR_EXIT(0x1, "error: no main section exists.\n");

    if (in_main_section == TRUE || in_data_section == TRUE)
        ERROR_EXIT(0x1, "error: non-ending section");

    if ((assembler->main.body == NULL) || (assembler->data.body == NULL))
        ERROR_EXIT(0x1, "error: unexpected issue allocating memory.\n");
}


void parse_symbols(assembler_s *assembler)
{
    /* token verification validates
     * this will never be written to
     * before a current section is decided */
    section_s *current_section;
    symbol_s  *current_symbol;
    u32_t      current_section_offset;

    u16_t     byte_c = 0;

    for (u32_t i = 0; i < assembler->token_c; i++)
        switch (assembler->token_v[i].type)
        {
            case MAIN:
                current_section = &assembler->main;

                current_section->symbol_v = arena_get_chunk(&assembler->chunk_arena);
                if (current_section->symbol_v == NULL)
                    ERROR_EXIT(0x1, "error: unexpected issue allocating memory.\n");

                current_section->symbol_c = 0;
                current_section_offset    = byte_c;
            break;

            case DATA:
                current_section = &assembler->data;

                current_section->symbol_v = arena_get_chunk(&assembler->chunk_arena);
                if (current_section->symbol_v == NULL)
                    ERROR_EXIT(0x1, "error: unexpect issue allocating memory.\n");

                current_section->symbol_c = 0;
                current_section_offset    = byte_c;
            break;

            case SYMBOL:
                current_symbol         = &current_section->symbol_v[current_section->symbol_c];
                current_symbol->name   = assembler->token_v[i].metadata;
                current_symbol->name_z = strlen(current_symbol->name);
                current_symbol->value  = byte_c - current_section_offset;
                
                current_section->symbol_c++;
            break;

            case VALUE:
                if (assembler->token_v[i].metadata[0] == '&')  /* value is symbol */
                    byte_c += 2;  /* 2 is size of symbol */
                else
                    byte_c++;
            break;

            case LOAD:
                byte_c += 2;
            break;

            case LOADJ:
                byte_c += 3;
            break;

            /* calculating section size here is a cheat, I should defer it to another function */
            case END_MAIN:
                current_section->body_l = byte_c - current_section_offset;
            break;

            case END_DATA:
                current_section->body_l = byte_c - current_section_offset;
            break;

            default:
                byte_c++;
        }
}

u16_t get_symbol_value(assembler_s *assembler, section_s *section, char *symbol_name, result_t *status)
{
    section_s *other_section;
    u16_t      value_offset;

    if (section == &assembler->main)
        value_offset = 0;
    else
        value_offset = 0x1000 - assembler->data.body_l;

    for (u16_t i = 0; i < section->symbol_c; i++)
    {
        if (strcmp(section->symbol_v[i].name, symbol_name) == 0)
            return value_offset + section->symbol_v[i].value;
    }

    if (section == &assembler->main && assembler->has_data_section)
    {
        other_section = &assembler->data;
        value_offset  = 0x1000 - assembler->data.body_l;
    }
    else
    {
        other_section = &assembler->main;
        value_offset  = 0;
    }

    /* check other section if symbol was not found in first section */
    for (u16_t i = 0; i < other_section->symbol_c; i++)
    {
        if (strcmp(other_section->symbol_v[i].name, symbol_name) == 0)
            return value_offset + other_section->symbol_v[i].value;
    }

    (*status) = SYMBOL_NOT_FOUND;
    return FAIL;
}

void assemble(assembler_s *assembler)
{
    result_t status = 0;

    /* token verification validates
     * this will never be written to
     * before a current section is decided */
    section_s *current_section;
    
    u16_t symbol_value;
    u16_t byte_value;

    u16_t byte_c;

    /* todo: separate below into function to help readability and code-quality */
    for (u32_t i = 0; i < assembler->token_c; i++)
    {
        switch (assembler->token_v[i].type)
        {
            case MAIN:
                current_section = &assembler->main;
                byte_c          = 0;
            break;

            case DATA:
                current_section = &assembler->data;
                byte_c          = 0;
            break;

            case VALUE:
                if (assembler->token_v[i].metadata[0] == '&')  /* value is symbol */
                {
                    symbol_value = get_symbol_value(assembler, current_section, &(assembler->token_v[i].metadata[1]), &status);
                    if (status == SYMBOL_NOT_FOUND)
                        ERROR_EXIT(0x1, "error: symbol '%s' does not exist.\n", &(assembler->token_v[i].metadata[1]));

                    current_section->body[byte_c]     = CAST(symbol_value & 0xFF, u8_t);
                    current_section->body[byte_c + 1] = symbol_value >> 8;

                    byte_c += 2;  /* 2 is size of symbol */
                }
                else
                {
                    byte_value = parse_value(assembler->token_v[i].metadata);
                    
                    current_section->body[byte_c] = byte_value;

                    byte_c++;
                }
            break;

            case LOAD:
                byte_value = parse_value(assembler->token_v[i].metadata);

                current_section->body[byte_c]     = LOAD;
                current_section->body[byte_c + 1] = byte_value;

                byte_c += 2;
            break;

            case LOADJ:
                symbol_value = get_symbol_value(assembler, current_section, &(assembler->token_v[i].metadata[1]), &status);
                if (status == SYMBOL_NOT_FOUND)
                    ERROR_EXIT(0x1, "error: symbol '%s' does not exist.\n", &(assembler->token_v[i].metadata[1]));

                current_section->body[byte_c]     = LOADJ;
                current_section->body[byte_c + 1] = CAST(symbol_value & 0xFF, u8_t);
                current_section->body[byte_c + 2] = symbol_value >> 8;

                byte_c += 3;
            break;

            case SYMBOL:
            case END_MAIN:
            case END_DATA:
            break;

            default:
                current_section->body[byte_c] = assembler->token_v[i].type;
                byte_c++;
        }
    }
}

void write_binary(assembler_s *assembler)
{
    fwrite(assembler->main.body, 1, assembler->main.body_l, assembler->binary_file);

    if (!assembler->has_data_section)
        return;

    if (fseek(assembler->binary_file, 0x1000 - assembler->data.body_l, SEEK_SET) != 0)
        ERROR_EXIT(0x1, "error: could not write file successfully.\n");
    
    fwrite(assembler->data.body, 1, assembler->data.body_l, assembler->binary_file);
}


u16_t parse_value(char *string_value)
{
    if (string_value[0] == 'x')
        return CAST(strtol(&(string_value[1]), NULL, 16), u16_t);
    else if (string_value[0] == 'b')
        return CAST(strtol(&(string_value[1]), NULL, 2), u16_t);
    else if (string_value[0] == 'o')
        return CAST(strtol(&(string_value[1]), NULL, 8), u16_t);

    return CAST(strtol(string_value, NULL, 10), u16_t);
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
