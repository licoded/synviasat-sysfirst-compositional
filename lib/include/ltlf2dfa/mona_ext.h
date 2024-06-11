#pragma once

extern "C" {
#include <mona/dfa.h>
#undef TRUE
#undef FALSE
}
#include "ltlf2dfa/graph_ext.h"
#include <cudd/cuddObj.hh>

void printDotFile(DFA *dfa, string dot_filename, int var_num, vector<unsigned int> var_index);
void printDfaFile(DFA *dfa, string dfa_filename, int var_num, std::vector<std::string> &var_names, std::vector<char> &var_orders);
DFA *graph2DFA(Syn_Graph &graph, DdNode *init_bddP, int var_num, std::vector<int> indicies);