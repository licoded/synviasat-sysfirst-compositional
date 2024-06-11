#pragma once

#include "formula/aalta_formula.h"
#include <cstring>
#include <unordered_set>

bool is_realizable(aalta::aalta_formula *src_formula, std::unordered_set<std::string> &env_var, bool verbose);