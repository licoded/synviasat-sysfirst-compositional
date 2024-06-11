#pragma once

#include <cassert>
#include <set>
#include <unordered_set>

#include "formula/aalta_formula.h"
#include "ltlf2dfa/edge_cons.h"
#include "synutil/formula_in_bdd.h"
#include "synutil/syn_type.h"
#include <cudd/cuddObj.hh>

using namespace std;
using namespace aalta;

namespace whole_dfa {

extern bool SAT_TRACE_FLAG;

typedef unsigned long long ull;

class edgeCons;

class Syn_Frame
{
  private:
    FormulaInBdd *state_in_bdd_;
    edgeCons *edgeCons_;
    Status status_;

    int swin_checked_idx_;
    int ewin_checked_idx_;

  public:
    Syn_Frame(aalta_formula *af);
    ~Syn_Frame();

    DdNode *GetBddPointer() { return state_in_bdd_->GetBddPointer(); }
    aalta_formula *GetFormulaPointer() { return state_in_bdd_->GetFormulaPointer(); }

    Status checkStatus();
    void processSignal(Signal sig, DdNode *succ);

    bool getEdge(unordered_set<int> &edge, queue<pair<aalta_formula *, aalta_formula *>> &model);
    Status get_status() { return status_; }

    bool checkSwinForBackwardSearch();
};

bool forwardSearch(Syn_Frame *);
void backwardSearch(std::vector<Syn_Frame *> &scc);

// for tarjan
void initial_tarjan_frame(Syn_Frame *cur_frame);
void update_by_low(Syn_Frame *cur_frame, DdNode *cur_bddP);
void update_by_dfn(Syn_Frame *cur_frame, Syn_Frame *next_frame);
void getScc(int cur, std::vector<Syn_Frame *> &scc, vector<Syn_Frame *> &sta, unordered_map<ull, int> &sta_bdd2curIdx_map);

void PartitionAtoms(aalta_formula *af, unordered_set<string> &env_val);

} // namespace whole_dfa