#pragma once

#include "ltlf2dfa/graph.h"
#include <cudd/cuddObj.hh>
#include <formula/aalta_formula.h>

using Syn_Edge = pair<DdNode *, aalta::aalta_formula *>;
using Syn_Graph = MyGraph<DdNode *, aalta::aalta_formula *>;

void printGraph(Syn_Graph &graph);