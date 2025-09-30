#include "./misc8.h"


i32_t main(i32_t arg_c, string *arg_v)
{
    misc8_t misc8 = new_misc8(arg_c, arg_v);

    run_program(&misc8);

    return SUCCESS;
}
