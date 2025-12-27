#include "./emulator.h"


i32_t main(i32_t arg_c, char **arg_v)
{
    misc8_t misc8 = new_misc8(arg_c - 1, &(arg_v[1]));

    run_program(&misc8);

    return SUCCESS;
}
