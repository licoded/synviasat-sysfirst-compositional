#pragma once

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <cassert>
#include "synutil/syn_type.h"
#include "synutil/formula_in_bdd.h"

typedef unsigned long long ull;

class syn_states
{
private:
    static std::unordered_set<ull> swin_state_bdd_set;
    static std::unordered_set<ull> ewin_state_bdd_set;
    static std::vector<DdNode *> swin_state_bdd_vec;
    static std::vector<DdNode *> ewin_state_bdd_vec;
    static std::unordered_set<ull> dfs_complete_state_bdd_set;
    static std::unordered_map<ull, set<DdNode *> *> predecessors;

public:
    static Status getBddStatus(DdNode *b);

    static void insert_swin_state(DdNode *bddP, bool from_imply = false);
    static void insert_ewin_state(DdNode *bddP, bool from_imply = false);
    static void insert_dfs_complete_state(DdNode *bddP);
    static void remove_dfs_complete_state(DdNode *bddP);

    static bool is_swin_state(DdNode *bddP);
    static bool is_ewin_state(DdNode *bddP);
    static bool is_dfs_complete_state(DdNode *bddP);

    static int get_state_cnt();

    static Status get_status_by_set(DdNode *bddp);
    static Status get_status_by_set_imply(DdNode *bddp, int swin_checked_idx, int ewin_checked_idx);
    static void insert_state_with_status(DdNode *bddp, Status status);

    static void addToGraph(DdNode *src, DdNode *dst);
    static set<DdNode *> *getPredecessors(DdNode *);
    static void freePredecessorsSet(DdNode *);
    static void releasePredecessors();
};