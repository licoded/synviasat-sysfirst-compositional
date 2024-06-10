#pragma once

#include <unordered_set>

extern unsigned int X_var_nums, Y_var_nums, total_var_nums;
extern std::unordered_set<int> X_vars, Y_vars;

void clear_XY_vars();
void calc_XY_var_nums();