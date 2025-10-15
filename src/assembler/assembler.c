#include "./assembler.h"


void new_assemble(i32_t arg_c, char **arg_v)
{
    assembler_s assembler;
    result_t    chunk_status = 0;


    u64_t  memory_size;
    void_p memory = vmalloc(&memory_size);
    if (memory == NULL)
        ERROR(0x1, "error: could not allocate memory.\n");


    assembler.chunk_arena      = arena_init(memory, memory_size, 4096);

    assembler.binary_file      = NULL;
    assembler.binary           = arena_get_chunk(&assembler.chunk_arena, &chunk_status);

    assembler.assembly_file    = NULL;

    assembler.has_data_section = FALSE;

    if (chunk_status & LS_CHUNK_ARENA_MEM_FULL)
        ERROR(0x1, "error: memory full.\n");


    parse_args(&assembler, arg_c, arg_v);
    parse_tokens(&assembler);

    assemble(&assembler);

    parse_symbols(&assembler);


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
            ERROR(0x1, "error: incomplete argument: '%s'\n", arg_v[i]);

        switch (arg_v[i][1])
        {
        case 'i':
            parse_input_flag(assembler, arg_v[i]);
        break;

        case 'o':
            parse_output_flag(assembler, arg_v[i]);
        break;

        default:
            ERROR(0x1, "error: unkown flag '%c'\n", arg_v[i][1]);
        break;
        }
    }

    if (assembler->assembly_file == NULL)
        ERROR(0x1, "error: no input file provided.");

    if (assembler->binary_file == NULL)
        parse_output_flag(assembler, "-o./program.bin");
}

void parse_input_flag(assembler_s *assembler, char *arg)
{
    u64_t mem_z;
    u64_t file_z;
    u64_t read_z;

    if (assembler->assembly_file != NULL)
        ERROR(0x1, "error: too many assembly files provided.\n");

    if (strlen(arg) <= 2)
        ERROR(0x1, "error: program assembly has no value.\n");

    assembler->assembly_file = fopen(&(arg[2]), "rb");
    if (assembler->assembly_file == NULL)
        ERROR(0x1, "error: could not open assembly file.\n");

    assembler->assembly = vmalloc(&mem_z);
    if (assembler->assembly == NULL)
        ERROR(0x1, "error: could not allocate memory for input file.\n");

    fseek(assembler->assembly_file, 0, SEEK_END);
    file_z = ftell(assembler->assembly_file);
    rewind(assembler->assembly_file);

    read_z = fread(assembler->assembly, 1, file_z, assembler->assembly_file);
    if (read_z != file_z)
        ERROR(0x1, "error: could not load input file to memory.\n");
    assembler->assembly[read_z] = '\0';

    assembler->token_v    = CAST(LS_ROUND_UP_TO(CAST(assembler->assembly, u64_t) + read_z + 1, sizeof(token_s)), token_s *);
    assembler->max_tokens = (CAST(assembler->token_v, u64_t) - CAST(assembler->assembly, u64_t)) / sizeof(token_s); 
    assembler->token_c    = 0;
}

void parse_output_flag(assembler_s *assembler, char *arg)
{
    if (assembler->binary_file != NULL)
        ERROR(0x1, "error: too many output files provided.\n");

    if (strlen(arg) <= 2)
        ERROR(0x1, "error: output argument has no value.\n");

    assembler->binary_file = fopen(&(arg[2]), "w");
    if (assembler->binary_file == NULL)
        ERROR(0x1, "error: could not create output file.\n");
}


void parse_tokens(assembler_s *assembler)
{    
    char *token = strtok(assembler->assembly, ".\n");

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
            token = strtok(NULL, ".\n");
            continue;
        }

        compute_token(assembler, token);

        token = strtok(NULL, ".\n");
    }
    while (token != NULL);

    verify_tokens(assembler);
}

void compute_token(assembler_s *assembler, char *token)
{
    if (strcmp(token, "main") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = MAIN };
    else if (strcmp(token, "end_main") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = END_MAIN };
    else if (strcmp(token, "data") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = DATA };
    else if (strcmp(token, "end_data") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = END_DATA };
    else if (token[0] == '$')
        assembler->token_v[assembler->token_c] = (token_s) { .type = SYMBOL, .metadata = &(token[1]) };
    else if (strcmp(token, "nop") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = NOP };
    else if (strcmp(token, "add") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = ADD };
    else if (strcmp(token, "sub") == 0)
        assembler->token_v[assembler->token_c] = (token_s) { .type = SUB };
    else if (strncmp(token, "load", 4) == 0)
    {
        char *symbol = token;
        while (isspace(*symbol))
            symbol++;

        assembler->token_v[assembler->token_c] = (token_s) { .type = LOAD, .metadata = symbol };
    }
    else if (strncmp(token, "loadj", 5) == 0)
    {
        char *symbol = token;
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
    result_t status;

    bool_t in_main_section = FALSE;
    bool_t in_data_section = FALSE;

    section_s section;

    for (int i = 0; i < assembler->token_c; i++)
        switch (assembler->token_v[assembler->token_c].type)
        {
        case MAIN:
            if (in_main_section || in_data_section)
                ERROR(0x1, "error: main section inside another section.\n");

            in_main_section = TRUE;
            section.body    = arena_get_chunk(&assembler->chunk_arena, &status);
            section.body_l  = 0;
        break;

        case END_MAIN:
            if (!in_main_section)
                ERROR(0x1, "error: ending non-main section.\n");

            in_main_section = FALSE;
        break;

        case DATA:
            if (in_main_section || in_data_section)
                ERROR(0x1, "error: data section inside another section.\n");

            in_main_section = TRUE;
        break;

        case END_DATA:
            if (!in_data_section)
                ERROR(0x1, "error: ending non-data section.\n");

            in_data_section = FALSE;
        break;

        default:
        break;
        }
}


void assemble(assembler_s *assembler)
{
    section_s current_section;

    for (int i = 0; i < assembler->token_c; i++)
        switch (assembler->token_v[i].type)
        {
        
        }
}


void parse_symbols(assembler_s *assembler)
{
    for (int i = 0; i < assembler->token_c; i++)
    {
        printf("%d: ", assembler->token_v[i].type);

        if (assembler->token_v[i].type == SYMBOL)
            printf("%s", assembler->token_v[i].metadata);

        printf("\n");
    }
}
