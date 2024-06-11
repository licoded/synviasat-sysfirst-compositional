#include "ltlf2dfa/syft_ext.h"
#include <cudd/cuddObj.hh>
#include <unordered_map>

bool syft_check_synthesis(SynType which_first, std::string dfa_filename, std::string &partfile)
{
    Cudd *mgr = new Cudd();
    bool res = false;
    std::unordered_map<unsigned, BDD> strategy;
    Syft::syn test(mgr, dfa_filename, partfile);
    bool syn_res = (which_first == SynType::SYS_FIRST) ? test.realizablity_sys(strategy) : test.realizablity_env(strategy);
    return syn_res;
}