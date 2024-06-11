#pragma once
#include <syft/syn.h>

enum class SynType
{
    ENV_FIRST,
    SYS_FIRST,
};

bool syft_check_synthesis(SynType which_first, std::string dfa_filename, std::string &partfile);
