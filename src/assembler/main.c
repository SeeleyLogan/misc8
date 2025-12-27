#include "./assembler.h"

i32_t main(i32_t arg_c, char **arg_v)
{
    setvbuf(stdout, NULL, _IONBF, 0);

    new_assemble(arg_c - 1, &(arg_v[1]));

    return SUCCESS;
}
