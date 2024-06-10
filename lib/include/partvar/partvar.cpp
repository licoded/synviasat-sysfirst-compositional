#include "partvar.h"

unsigned int X_var_nums, Y_var_nums, total_var_nums;
std::unordered_set<int> X_vars, Y_vars;

void clear_XY_vars()
{
    X_vars.clear();
    Y_vars.clear();
}

void calc_XY_var_nums()
{
    X_var_nums = X_vars.size();
    Y_var_nums = Y_vars.size();
    total_var_nums = X_var_nums + Y_var_nums;
}