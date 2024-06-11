#pragma once

extern "C" {
#include <mona/dfa.h>
#undef TRUE
#undef FALSE
}
#include "ltlf2dfa/graph_ext.h"
#include <cudd/cudd.h>

void printDotFile(DFA *dfa, string &dot_filename, int var_num, unsigned int *var_index);
void printDfaFile(DFA *dfa, string &dfa_filename, int var_num, char *var_names[], char var_orders[]);
DFA *graph2DFA(Syn_Graph &graph, DdNode *init_bddP, int var_num, int *indicies);