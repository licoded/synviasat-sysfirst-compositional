#pragma once

#include "synutil/syn_type.h"
#include <cassert>
#include <cudd/cuddObj.hh>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

typedef unsigned long long ull;

class syn_states
{
  private:
    static std::unordered_set<ull> swin_state_bdd_set;
    static std::unordered_set<ull> ewin_state_bdd_set;
    static std::vector<DdNode *> swin_state_bdd_vec;
    static std::vector<DdNode *> ewin_state_bdd_vec;
    static std::unordered_set<ull> dfs_complete_state_bdd_set;
    static std::unordered_map<ull, std::set<DdNode *> *> predecessors;
    static std::unordered_map<ull, bool> isAcc_byEmpty_bddP_map;

  public:
    static Status getBddStatus(DdNode *b);

    static void insert_swin_state(DdNode *bddP, bool from_imply = false);
    static void insert_ewin_state(DdNode *bddP, bool from_imply = false);
    static void insert_dfs_complete_state(DdNode *bddP);
    static void remove_dfs_complete_state(DdNode *bddP);

    static bool is_swin_state(ull bddP);
    static bool is_swin_state(DdNode *bddP);
    static bool is_ewin_state(ull bddP);
    static bool is_ewin_state(DdNode *bddP);
    static bool is_dfs_complete_state(DdNode *bddP);

    static int get_state_cnt();

    static Status get_status_by_set(DdNode *bddp);
    static Status get_status_by_set_imply(DdNode *bddp, int swin_checked_idx, int ewin_checked_idx);
    static void insert_state_with_status(DdNode *bddp, Status status);

    static void addToGraph(DdNode *src, DdNode *dst);
    static std::set<DdNode *> *getPredecessors(DdNode *);
    static void freePredecessorsSet(DdNode *);
    static void releasePredecessors();

    static void set_isAcc_byEmpty(ull bddP, bool isAcc_byEmpty);
    static void set_isAcc_byEmpty(DdNode *bddP, bool isAcc_byEmpty);
    static bool in_isAcc_byEmpty_map(ull bddP);
    static bool in_isAcc_byEmpty_map(DdNode *bddP);
    static bool isAcc_byEmpty(ull bddP);
    static bool isAcc_byEmpty(DdNode *bddP);
};

// implemantation of inline funtions
inline void syn_states::insert_swin_state(DdNode *bddP, bool from_imply)
{
    if (swin_state_bdd_set.find(ull(bddP)) == swin_state_bdd_set.end())
    {
        swin_state_bdd_set.insert(ull(bddP));
        if (false) //(!from_imply)
            swin_state_bdd_vec.push_back(bddP);
    }
}

inline void syn_states::insert_ewin_state(DdNode *bddP, bool from_imply)
{
    if (ewin_state_bdd_set.find(ull(bddP)) == ewin_state_bdd_set.end())
    {
        ewin_state_bdd_set.insert(ull(bddP));
        if (false) //(!from_imply)
            ewin_state_bdd_vec.push_back(bddP);
    }
}

inline void syn_states::insert_dfs_complete_state(DdNode *bddP)
{
    dfs_complete_state_bdd_set.insert(ull(bddP));
}

inline void syn_states::remove_dfs_complete_state(DdNode *bddP)
{
    dfs_complete_state_bdd_set.erase(ull(bddP));
}

inline bool syn_states::is_swin_state(ull bddP)
{
    return swin_state_bdd_set.find(bddP) != swin_state_bdd_set.end();
}

inline bool syn_states::is_swin_state(DdNode *bddP)
{
    return is_swin_state(ull(bddP));
}

inline bool syn_states::is_ewin_state(ull bddP)
{
    return ewin_state_bdd_set.find(bddP) != ewin_state_bdd_set.end();
}

inline bool syn_states::is_ewin_state(DdNode *bddP)
{
    return is_ewin_state(ull(bddP));
}

inline bool syn_states::is_dfs_complete_state(DdNode *bddP)
{
    return dfs_complete_state_bdd_set.find(ull(bddP)) != dfs_complete_state_bdd_set.end();
}

inline int syn_states::get_state_cnt()
{
    return swin_state_bdd_set.size() + ewin_state_bdd_set.size() + dfs_complete_state_bdd_set.size();
}

inline void syn_states::insert_state_with_status(DdNode *bddp, Status status)
{
    switch (status)
    {
    case Swin:
        syn_states::insert_swin_state(bddp, false);
        break;
    case Ewin:
        syn_states::insert_ewin_state(bddp, false);
        break;
    case Dfs_complete:
        syn_states::insert_dfs_complete_state(bddp);
    default:
        break;
    }
}

inline void syn_states::addToGraph(DdNode *src, DdNode *dst)
{
    if (predecessors.find(ull(dst)) == predecessors.end())
        predecessors[ull(dst)] = new std::set<DdNode *>();
    (predecessors[ull(dst)])->insert(src);
}

inline std::set<DdNode *> *syn_states::getPredecessors(DdNode *s)
{
    assert(predecessors.find(ull(s)) != predecessors.end());
    return predecessors[ull(s)];
}

inline void syn_states::freePredecessorsSet(DdNode *s)
{
    assert(predecessors.find(ull(s)) != predecessors.end());
    delete predecessors[ull(s)];
    predecessors.erase(ull(s));
}

inline void syn_states::releasePredecessors()
{
    for (auto it : predecessors)
        delete it.second;
}

inline void syn_states::set_isAcc_byEmpty(ull bddP, bool isAcc_byEmpty)
{
    isAcc_byEmpty_bddP_map[bddP] = isAcc_byEmpty;
}

inline void syn_states::set_isAcc_byEmpty(DdNode *bddP, bool isAcc_byEmpty)
{
    set_isAcc_byEmpty(ull(bddP), isAcc_byEmpty);
}

inline bool syn_states::in_isAcc_byEmpty_map(ull bddP)
{
    return isAcc_byEmpty_bddP_map.find(bddP) != isAcc_byEmpty_bddP_map.end();
}

inline bool syn_states::in_isAcc_byEmpty_map(DdNode *bddP)
{
    return in_isAcc_byEmpty_map(ull(bddP));
}

inline bool syn_states::isAcc_byEmpty(ull bddP)
{
    return isAcc_byEmpty_bddP_map[bddP];
}

inline bool syn_states::isAcc_byEmpty(DdNode *bddP)
{
    return isAcc_byEmpty(ull(bddP));
}
