#pragma once

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <cassert>
#include "synutil/syn_type.h"
#include "synutil/formula_in_bdd.h"

typedef unsigned long long ull;

namespace syn_states
{
    extern std::unordered_set<ull> swin_state_bdd_set;
    extern std::unordered_set<ull> ewin_state_bdd_set;
    extern std::vector<DdNode *> swin_state_bdd_vec;
    extern std::vector<DdNode *> ewin_state_bdd_vec;
    extern std::unordered_set<ull> dfs_complete_state_bdd_set;
    extern std::unordered_map<ull, set<DdNode *> *> predecessors;

    Status getBddStatus(DdNode *b);

    void insert_swin_state(DdNode *bddP, bool from_imply = false);
    void insert_ewin_state(DdNode *bddP, bool from_imply = false);
    void insert_dfs_complete_state(DdNode *bddP);
    void remove_dfs_complete_state(DdNode *bddP);

    bool is_swin_state(DdNode *bddP);
    bool is_ewin_state(DdNode *bddP);
    bool is_dfs_complete_state(DdNode *bddP);

    int get_state_cnt();

    Status get_status_by_set(DdNode *bddp);
    Status get_status_by_set_imply(DdNode *bddp, int swin_checked_idx, int ewin_checked_idx);
    void insert_state_with_status(DdNode *bddp, Status status);

    void addToGraph(DdNode *src, DdNode *dst);
    set<DdNode *> *getPredecessors(DdNode *);
    void freePredecessorsSet(DdNode *);
    void releasePredecessors();
}