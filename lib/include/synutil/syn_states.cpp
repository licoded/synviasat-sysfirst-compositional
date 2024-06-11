#include "synutil/syn_states.h"
#include "synutil/formula_in_bdd.h"

std::unordered_set<ull> syn_states::swin_state_bdd_set;
std::unordered_set<ull> syn_states::ewin_state_bdd_set;
std::unordered_set<ull> syn_states::dfs_complete_state_bdd_set;
std::vector<DdNode *> syn_states::swin_state_bdd_vec;
std::vector<DdNode *> syn_states::ewin_state_bdd_vec;
std::unordered_map<ull, std::set<DdNode *> *> syn_states::predecessors;

Status syn_states::getBddStatus(DdNode *b)
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

Status syn_states::get_status_by_set(DdNode *bddp)
{
    if (swin_state_bdd_set.find(ull(bddp)) != swin_state_bdd_set.end())
        return Swin;
    if (ewin_state_bdd_set.find(ull(bddp)) != ewin_state_bdd_set.end())
        return Ewin;
    if (dfs_complete_state_bdd_set.find(ull(bddp)) != dfs_complete_state_bdd_set.end())
        return Dfs_complete;
    return Dfs_incomplete;
}

Status syn_states::get_status_by_set_imply(DdNode *bddp, int swin_checked_idx, int ewin_checked_idx)
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
