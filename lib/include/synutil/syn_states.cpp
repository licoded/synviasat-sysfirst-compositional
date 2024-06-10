#include "synutil/syn_states.h"


namespace syn_states
{
    std::unordered_set<ull> swin_state_bdd_set;
    std::unordered_set<ull> ewin_state_bdd_set;
    std::unordered_set<ull> dfs_complete_state_bdd_set;
    std::vector<DdNode *> swin_state_bdd_vec;
    std::vector<DdNode *> ewin_state_bdd_vec;
    std::unordered_map<ull, set<DdNode *> *> predecessors;

    Status getBddStatus(DdNode *b)
    {
        if (swin_state_bdd_set.find(ull(b)) != swin_state_bdd_set.end())
            return Swin;
        else if (ewin_state_bdd_set.find(ull(b)) != ewin_state_bdd_set.end())
            return Ewin;
        else if (dfs_complete_state_bdd_set.find(ull(b)) != dfs_complete_state_bdd_set.end())
            return Dfs_complete;
        else
            return Dfs_incomplete;
    }

    // implemantation of inline funtions
    inline void insert_swin_state(DdNode *bddP, bool from_imply)
    {
        if (swin_state_bdd_set.find(ull(bddP)) == swin_state_bdd_set.end())
        {
            swin_state_bdd_set.insert(ull(bddP));
            if (false) //(!from_imply)
                swin_state_bdd_vec.push_back(bddP);
        }
    }

    inline void insert_ewin_state(DdNode *bddP, bool from_imply)
    {
        if (ewin_state_bdd_set.find(ull(bddP)) == ewin_state_bdd_set.end())
        {
            ewin_state_bdd_set.insert(ull(bddP));
            if (false) //(!from_imply)
                ewin_state_bdd_vec.push_back(bddP);
        }
    }

    inline void insert_dfs_complete_state(DdNode *bddP)
    {
        dfs_complete_state_bdd_set.insert(ull(bddP));
    }

    inline void remove_dfs_complete_state(DdNode *bddP)
    {
        dfs_complete_state_bdd_set.erase(ull(bddP));
    }

    inline bool is_swin_state(DdNode *bddP)
    {
        return swin_state_bdd_set.find(ull(bddP)) != swin_state_bdd_set.end();
    }

    inline bool is_ewin_state(DdNode *bddP)
    {
        return ewin_state_bdd_set.find(ull(bddP)) != ewin_state_bdd_set.end();
    }

    inline bool is_dfs_complete_state(DdNode *bddP)
    {
        return dfs_complete_state_bdd_set.find(ull(bddP)) != dfs_complete_state_bdd_set.end();
    }

    inline int get_state_cnt()
    {
        return swin_state_bdd_set.size() + ewin_state_bdd_set.size() + dfs_complete_state_bdd_set.size();
    }

    inline Status get_status_by_set(DdNode *bddp)
    {
        if (swin_state_bdd_set.find(ull(bddp)) != swin_state_bdd_set.end())
            return Swin;
        if (ewin_state_bdd_set.find(ull(bddp)) != ewin_state_bdd_set.end())
            return Ewin;
        if (dfs_complete_state_bdd_set.find(ull(bddp)) != dfs_complete_state_bdd_set.end())
            return Dfs_complete;
        return Dfs_incomplete;
    }

    inline Status get_status_by_set_imply(DdNode *bddp, int swin_checked_idx, int ewin_checked_idx)
    {
        int vec_size = swin_state_bdd_vec.size();
        for (; swin_checked_idx < vec_size; ++swin_checked_idx)
            if (FormulaInBdd::Implies(swin_state_bdd_vec[swin_checked_idx], bddp))
            {
                syn_states::insert_swin_state(bddp, true);
                return Swin;
            }
        vec_size = ewin_state_bdd_vec.size();
        for (; ewin_checked_idx < vec_size; ++ewin_checked_idx)
            if (FormulaInBdd::Implies(bddp, ewin_state_bdd_vec[ewin_checked_idx]))
            {
                syn_states::insert_ewin_state(bddp, true);
                return Ewin;
            }
        return Dfs_incomplete;
    }

    inline void insert_state_with_status(DdNode *bddp, Status status)
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

    inline void addToGraph(DdNode *src, DdNode *dst)
    {
        if (predecessors.find(ull(dst)) == predecessors.end())
            predecessors[ull(dst)] = new set<DdNode *>();
        (predecessors[ull(dst)])->insert(src);
    }

    inline set<DdNode *> *getPredecessors(DdNode *s)
    {
        assert(predecessors.find(ull(s)) != predecessors.end());
        return predecessors[ull(s)];
    }

    inline void freePredecessorsSet(DdNode *s)
    {
        assert(predecessors.find(ull(s)) != predecessors.end());
        delete predecessors[ull(s)];
        predecessors.erase(ull(s));
    }

    void releasePredecessors()
    {
        for (auto it : predecessors)
            delete it.second;
    }
}