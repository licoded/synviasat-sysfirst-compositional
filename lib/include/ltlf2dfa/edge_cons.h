#pragma once

#include <memory>
#include <queue>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "formula/aalta_formula.h"
#include "ltlf2dfa/graph_ext.h"
#include "synutil/formula_in_bdd.h"
#include "synutil/syn_type.h"

using namespace std;
using namespace aalta;

typedef unsigned long long ull;

namespace whole_dfa {

class XCons
{
  public:
    XCons(DdNode *XCons_bddp, DdNode *state_bddp, aalta_formula *state_af, aalta_formula *af_Y);
    void get_related_succ(vector<DdNode *> &);
    Status get_status() { return status_; }
    void processSignal(Signal, DdNode *succ);
    aalta_formula *getEdge();
    aalta_formula *set_search_direction(aalta_formula *X);
    int find_match_X_idx(aalta_formula *X);
    // aalta_formula *get_blocked_X() { return blocked_X_; }
    bool checkSwinForBackwardSearch();
    bool hasTravAllEdges() { return trav_all_afX_X_idx_.size() == X_parts_.size(); }
    void get_succ_edges(aalta_formula *af_X, vector<Syn_Edge> &succ_edges);

  private:
    vector<aalta_formula *> X_parts_;
    vector<DdNode *> successors_;

    unordered_multimap<ull, int> succ_bddP_to_idx_;

    set<int> swin_X_idx_;
    set<int> searched_X_idx_;
    set<int> trav_all_afX_X_idx_;

    // aalta_formula *blocked_X_;

    int current_X_idx_;
    Status status_;

    void insert_swin_X_idx(int x);
    void insert_searched_X_idx(int x);
    void insert_trav_all_afX_X_idx(int x);

    void resizeContainers()
    {
        X_parts_.shrink_to_fit();
        successors_.shrink_to_fit();
    }
};

class edgeCons
{
  public:
    edgeCons(DdNode *src_xnf_bddp, aalta_formula *state_af, aalta_formula *acc_edge);
    ~edgeCons();
    Status get_status() { return status_; }
    void processSignal(Signal, DdNode *succ);
    bool getEdge(unordered_set<int> &edge, queue<pair<aalta_formula *, aalta_formula *>> &model);
    bool checkSwinForBackwardSearch();
    bool hasTravAllEdges() { return trav_all_afY_Y_idx_.size() == Y_parts_.size(); }
    void check_hasTravAllEdges();
    void get_succ_edges(vector<Syn_Edge> &succ_edges);

  private:
    aalta_formula *state_af_;
    aalta_formula *blocked_Y_;

    vector<aalta_formula *> Y_parts_;
    vector<XCons *> X_cons_;

    unordered_multimap<ull, int> succ_bddP_to_idx_;

    set<int> ewin_Y_idx_;
    set<int> dfs_complete_Y_idx_;
    set<int> trav_all_afY_Y_idx_;

    Status status_;
    int current_Y_idx_;

    // aalta_formula *get_edge_cons_for_aaltaf();
    // aalta_formula * set_search_direction(const pair<aalta_formula *, aalta_formula *>&);
    int find_match_Y_idx(aalta_formula *Y);
    void insert_ewin_Y_idx(int y);
    void insert_dfs_complete_Y_idx(int y);
    void insert_trav_all_afY_Y_idx(int y);
    bool checkConflict(pair<aalta_formula *, aalta_formula *> &edge)
    {
        return FormulaInBdd::check_conflict(aalta_formula(aalta_formula::And, edge.first, edge.second).unique(), blocked_Y_);
    }

    void resizeContainers()
    {
        Y_parts_.shrink_to_fit();
        X_cons_.shrink_to_fit();
    }
};

bool isCompatible(unordered_set<int> *edge1, unordered_set<int> *edge2);

} // namespace whole_dfa