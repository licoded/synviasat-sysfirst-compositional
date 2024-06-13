#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <tuple>

#include "ltlf2dfa/edge_cons.h"
#include "ltlf2dfa/synthesis.h"
#include "ltlfsat/carchecker.h"
#include "synutil/af_utils.h"
#include "synutil/preprocess.h"
#include "synutil/syn_states.h"

namespace whole_dfa {

edgeCons::edgeCons(DdNode *src_bdd, aalta_formula *state_af, aalta_formula *acc_edge)
    : state_af_(state_af), blocked_Y_(aalta_formula::TRUE()), status_(Dfs_incomplete), current_Y_idx_(-1)
{
    unordered_map<ull, XCons *> bdd_XCons;
    unordered_map<ull, vector<DdNode *> *> XCons_related_succ;
    queue<tuple<DdNode *, aalta_formula *, bool>> q;
    aalta_formula *acc_or_s = aalta_formula(aalta_formula::Or, acc_edge, state_af).unique();
    aalta_formula *xxxx = aalta_formula(aalta_formula::And, edgeNotToFalse(state_af)->simplify(), acc_or_s).unique();
    q.push({src_bdd, NULL, false});

    while (!q.empty()) /* do BFS!!! */
    {
        DdNode *node = std::get<0>(q.front());
        aalta_formula *af_Y = std::get<1>(q.front());
        bool is_complement = std::get<2>(q.front());
        q.pop();

        if (!FormulaInBdd::is_Y_var(node))
        {
            DdNode *true_node = is_complement ? Cudd_Not(node) : node;

            if (af_Y == NULL)
                af_Y = aalta_formula::TRUE();
            Y_parts_.push_back(af_Y);

            if (bdd_XCons.find(ull(true_node)) == bdd_XCons.end())
            {
                XCons *x_cons = new XCons(true_node, src_bdd, xxxx, af_Y);
                bdd_XCons.insert({ull(true_node), x_cons});
                vector<DdNode *> *related_succ = new vector<DdNode *>();
                x_cons->get_related_succ(*related_succ);
                XCons_related_succ[ull(x_cons)] = related_succ;
            }
            X_cons_.push_back(bdd_XCons[ull(true_node)]);
            for (auto it : *XCons_related_succ[ull(X_cons_.back())])
                succ_bddP_to_idx_.insert({ull(it), X_cons_.size() - 1});
            continue;
        }

        aalta_formula *cur_Y = FormulaInBdd::get_af_var(Cudd_NodeReadIndex(node));
        aalta_formula *not_cur_Y = aalta_formula(aalta_formula::Not, NULL, cur_Y).unique();
        aalta_formula *T_afY = af_Y == NULL ? cur_Y : aalta_formula(aalta_formula::And, af_Y, cur_Y).unique();
        aalta_formula *E_afY = af_Y == NULL ? not_cur_Y : aalta_formula(aalta_formula::And, af_Y, not_cur_Y).unique();

        q.push({Cudd_T(node), T_afY, is_complement ^ Cudd_IsComplement(node)});
        q.push({Cudd_E(node), E_afY, is_complement ^ Cudd_IsComplement(node)});
    }
    for (const auto &it : XCons_related_succ)
        delete it.second;
    resizeContainers();
}

edgeCons::~edgeCons()
{
    vector<ull> tmp;
    for (auto it : X_cons_)
        tmp.push_back(ull(it));
    sort(tmp.begin(), tmp.end());
    auto uit = unique(tmp.begin(), tmp.end());
    for (auto it = tmp.begin(); it < uit; ++it)
        delete (XCons *)(*it);
}

XCons::XCons(DdNode *root, DdNode *state_bddp, aalta_formula *state_af, aalta_formula *af_Y)
    : current_X_idx_(-1), status_(Dfs_incomplete) // blocked_X_(aalta_formula::TRUE())
{
    queue<tuple<DdNode *, aalta_formula *, bool>> q;
    q.push({root, NULL, false});

    while (!q.empty()) /* do BFS!!! */
    {
        DdNode *node = std::get<0>(q.front());
        aalta_formula *af_X = std::get<1>(q.front());
        bool is_complement = std::get<2>(q.front());
        q.pop();

        if (!FormulaInBdd::is_X_var(node))
        {
            if (af_X == NULL)
                af_X = aalta_formula::TRUE();
            X_parts_.push_back(af_X);

            aalta_formula *edge_af = aalta_formula(aalta_formula::And, af_X, af_Y).unique();
            unordered_set<int> edge_var_set;
            edge_af->to_set(edge_var_set);
            fill_in_edgeset(edge_var_set);
            aalta_formula *succ_state_af;
            succ_state_af = FormulaProgression_empty(state_af, edge_var_set);
            DdNode *succ_state_bdd = FormulaInBdd(succ_state_af, xnf_empty(succ_state_af)).GetBddPointer();
            successors_.push_back(succ_state_bdd);

            if (succ_state_bdd == state_bddp || syn_states::is_ewin_state(succ_state_bdd))
            {
                status_ = Ewin;
                break;
            }

            succ_bddP_to_idx_.insert({ull(succ_state_bdd), successors_.size() - 1});
            continue;
        }

        aalta_formula *cur_X = FormulaInBdd::get_af_var(Cudd_NodeReadIndex(node));
        aalta_formula *not_cur_X = aalta_formula(aalta_formula::Not, NULL, cur_X).unique();
        aalta_formula *T_afX = af_X == NULL ? cur_X : aalta_formula(aalta_formula::And, af_X, cur_X).unique();
        aalta_formula *E_afX = af_X == NULL ? not_cur_X : aalta_formula(aalta_formula::And, af_X, not_cur_X).unique();

        q.push({Cudd_T(node), T_afX, is_complement ^ Cudd_IsComplement(node)});
        q.push({Cudd_E(node), E_afX, is_complement ^ Cudd_IsComplement(node)});
    }
    resizeContainers();
}

void XCons::get_related_succ(vector<DdNode *> &related_succ)
{
    for (const auto &p : succ_bddP_to_idx_)
        related_succ.push_back((DdNode *)(p.first));
}

void edgeCons::processSignal(Signal sig, DdNode *succ)
{
    assert(sig != Unsat);
    if (sig == To_ewin)
    {
        auto range = succ_bddP_to_idx_.equal_range(ull(succ));
        for (auto it = range.first; it != range.second; ++it)
        {
            X_cons_[it->second]->processSignal(To_ewin, succ);
            insert_ewin_Y_idx(it->second);
            insert_trav_all_afY_Y_idx(it->second);
        }
        if (ewin_Y_idx_.size() == Y_parts_.size())
            status_ = Ewin;
    }
    else if (sig == To_swin)
    {
        auto range = succ_bddP_to_idx_.equal_range(ull(succ));
        for (auto it = range.first; it != range.second; ++it)
        {
            X_cons_[it->second]->processSignal(To_swin, succ);
            if (X_cons_[it->second]->hasTravAllEdges())
                insert_trav_all_afY_Y_idx(it->second);
            if (X_cons_[it->second]->get_status() == Swin)
            {
                status_ = Swin;
            }
            else if (X_cons_[it->second]->get_status() == Dfs_complete)
            {
                insert_dfs_complete_Y_idx(it->second);
            }
        }
    }
    else if (sig == Pending)
    {
        auto range = succ_bddP_to_idx_.equal_range(ull(succ));
        for (auto it = range.first; it != range.second; ++it)
        {
            X_cons_[it->second]->processSignal(Pending, succ);
            if (X_cons_[it->second]->hasTravAllEdges())
                insert_trav_all_afY_Y_idx(it->second);
            if (X_cons_[it->second]->get_status() == Dfs_complete)
                insert_dfs_complete_Y_idx(it->second);
        }
    }
    if ((ewin_Y_idx_.size() + dfs_complete_Y_idx_.size()) == Y_parts_.size())
        status_ = Dfs_complete;
    if (X_cons_[current_Y_idx_]->hasTravAllEdges())
        current_Y_idx_ = -1;
}

void XCons::processSignal(Signal sig, DdNode *succ)
{
    assert(sig != Unsat);
    if (sig == To_ewin)
    {
        status_ = Ewin;
    }
    else if (sig == To_swin)
    {
        auto range = succ_bddP_to_idx_.equal_range(ull(succ));
        for (auto it = range.first; it != range.second; ++it)
        {
            insert_swin_X_idx(it->second);
            insert_trav_all_afX_X_idx(it->second);
        }
        if (swin_X_idx_.size() == X_parts_.size())
            status_ = Swin;
    }
    else if (sig == Pending)
    {
        auto range = succ_bddP_to_idx_.equal_range(ull(succ));
        for (auto it = range.first; it != range.second; ++it)
        {
            insert_searched_X_idx(it->second);
            insert_trav_all_afX_X_idx(it->second);
        }
    }
    if (swin_X_idx_.size() + searched_X_idx_.size() == X_parts_.size())
        status_ = Dfs_complete;
    current_X_idx_ = -1;
}

bool edgeCons::getEdge(unordered_set<int> &edge, queue<pair<aalta_formula *, aalta_formula *>> &model)
{
    aalta_formula *edge_af = NULL;
    if (current_Y_idx_ == -1)
        for (int i = 0; i < Y_parts_.size(); ++i)
        {
            if (X_cons_[i]->hasTravAllEdges())
                insert_trav_all_afY_Y_idx(i);
            if (trav_all_afY_Y_idx_.find(i) == trav_all_afY_Y_idx_.end())
            {
                current_Y_idx_ = i;
                break;
            }
        }
    assert(current_Y_idx_ != -1);
    aalta_formula *af_Y = Y_parts_[current_Y_idx_];
    aalta_formula *af_X = X_cons_[current_Y_idx_]->getEdge();
    dout << "=getEdge="
         << "\t"
         << "Y: " << setw(24) << af_Y->to_string() << "\t"
         << "X: " << setw(24) << af_X->to_string() << endl;
    edge_af = aalta_formula(aalta_formula::And, af_X, af_Y).unique()->simplify();
    edge_af->to_set(edge);
    // cout<<edge_af->to_string()<<endl;
    fill_in_edgeset(edge);
    return true;
}

aalta_formula *XCons::getEdge()
{
    assert(current_X_idx_ == -1);
    for (int i = 0; i < X_parts_.size(); ++i)
        if (trav_all_afX_X_idx_.find(i) == trav_all_afX_X_idx_.end())
        {
            current_X_idx_ = i;
            break;
        }
    assert(current_X_idx_ != -1);
    return X_parts_[current_X_idx_];
}

// aalta_formula *edgeCons::set_search_direction(const pair<aalta_formula *, aalta_formula *> &XY)
// {
//     assert(SAT_TRACE_FLAG && current_Y_idx_ == -1);
//     current_Y_idx_ = find_match_Y_idx(XY.second);
//     aalta_formula *X = X_cons_[current_Y_idx_]->set_search_direction(XY.first);
//     if (X == NULL)
//         return NULL;
//     else
//         return aalta_formula(aalta_formula::And, Y_parts_[current_Y_idx_], X).unique();
// }

bool edgeCons::checkSwinForBackwardSearch()
{
    bool is_swin = false;
    for (auto it : dfs_complete_Y_idx_)
        if ((X_cons_[it]->checkSwinForBackwardSearch()))
        {
            is_swin = true;
            break;
        }
    return is_swin;
}

void edgeCons::check_hasTravAllEdges()
{
    for (int i = 0; i < Y_parts_.size(); ++i)
    {
        if ((X_cons_[i]->get_status() == Ewin) || X_cons_[i]->hasTravAllEdges())
            insert_trav_all_afY_Y_idx(i);
    }
}

void edgeCons::get_succ_edges(vector<Syn_Edge> &succ_edges)
{
    assert(X_cons_.size() == Y_parts_.size());
    if (get_status() == Ewin)
        return;
    for (int i = 0; i < X_cons_.size(); ++i)
    {
        aalta_formula *af_Y = Y_parts_[i];
        X_cons_[i]->get_succ_edges(af_Y, succ_edges);
    }
}

void XCons::get_succ_edges(aalta_formula *af_X, vector<Syn_Edge> &succ_edges)
{
    assert(X_parts_.size() == successors_.size());
    for (int i = 0; i < X_parts_.size(); ++i)
    {
        DdNode *succ_bdd = successors_[i];
        if (syn_states::in_isAcc_byEmpty_map(succ_bdd) && syn_states::isAcc_byEmpty(succ_bdd))
            continue;
        aalta_formula *af_Y = X_parts_[i];
        aalta_formula *af_edge = aalta_formula(aalta_formula::And, af_X, af_Y).unique();
        succ_edges.push_back({succ_bdd, af_edge});
    }
}

bool XCons::checkSwinForBackwardSearch()
{
    bool is_swin = true;
    for (auto it : searched_X_idx_)
        if (syn_states::is_swin_state(successors_[it]))
        {
            is_swin = false;
            break;
        }
    return is_swin;
}

int edgeCons::find_match_Y_idx(aalta_formula *Y)
{
    // get_edge_cons_for_aaltaf() guarantee that
    // Dfs_complete X will not appear again.
    unordered_set<int> target_y;
    Y->to_set(target_y);
    for (int i = 0; i < Y_parts_.size(); ++i)
        if (ewin_Y_idx_.find(i) == ewin_Y_idx_.end() && dfs_complete_Y_idx_.find(i) == dfs_complete_Y_idx_.end())
        {
            unordered_set<int> tmp;
            Y_parts_[i]->to_set(tmp);
            if (isCompatible(&target_y, &tmp))
                return i;
        }
    assert(false);
}

void edgeCons::insert_ewin_Y_idx(int y)
{
    if (ewin_Y_idx_.find(y) == ewin_Y_idx_.end())
    {
        ewin_Y_idx_.insert(y);
        aalta_formula *not_Y = aalta_formula(aalta_formula::Not, NULL, Y_parts_[y]).nnf();
        blocked_Y_ = (aalta_formula(aalta_formula::And, blocked_Y_, not_Y).simplify())->unique();
    }
}

void edgeCons::insert_dfs_complete_Y_idx(int y)
{
    if (dfs_complete_Y_idx_.find(y) == dfs_complete_Y_idx_.end())
    {
        dfs_complete_Y_idx_.insert(y);
        aalta_formula *not_Y = aalta_formula(aalta_formula::Not, NULL, Y_parts_[y]).nnf();
        blocked_Y_ = (aalta_formula(aalta_formula::And, blocked_Y_, not_Y).simplify())->unique();
    }
}

void edgeCons::insert_trav_all_afY_Y_idx(int y)
{
    if (trav_all_afY_Y_idx_.find(y) == trav_all_afY_Y_idx_.end())
    {
        trav_all_afY_Y_idx_.insert(y);
    }
}

void XCons::insert_swin_X_idx(int x)
{
    if (swin_X_idx_.find(x) == swin_X_idx_.end())
    {
        swin_X_idx_.insert(x);
        // aalta_formula *not_X = aalta_formula(aalta_formula::Not, NULL, X_parts_[x]).nnf();
        // blocked_X_ = (aalta_formula(aalta_formula::And, blocked_X_, not_X).simplify())->unique();
    }
}
void XCons::insert_searched_X_idx(int x)
{
    if (searched_X_idx_.find(x) == searched_X_idx_.end())
    {
        searched_X_idx_.insert(x);
        // aalta_formula *not_X = aalta_formula(aalta_formula::Not, NULL, X_parts_[x]).nnf();
        // blocked_X_ = (aalta_formula(aalta_formula::And, blocked_X_, not_X).simplify())->unique();
    }
}

void XCons::insert_trav_all_afX_X_idx(int x)
{
    if (trav_all_afX_X_idx_.find(x) == trav_all_afX_X_idx_.end())
    {
        trav_all_afX_X_idx_.insert(x);
    }
}

bool isCompatible(unordered_set<int> *edge1, unordered_set<int> *edge2)
{
    unordered_set<int> *smaller_set = edge1, *bigger_set = edge2;
    if ((smaller_set->size()) > (bigger_set->size()))
        smaller_set = edge2, bigger_set = edge1;
    for (auto it : (*smaller_set))
        if (bigger_set->find(-it) != bigger_set->end())
            return false;
    return true;
}

// aalta_formula *edgeCons::get_edge_cons_for_aaltaf()
// {
//     if (current_Y_idx_ == -1)
//         return blocked_Y_;
//     return aalta_formula(aalta_formula::And,
//                          Y_parts_[current_Y_idx_],
//                          X_cons_[current_Y_idx_]->get_blocked_X())
//         .unique();
// }

aalta_formula *XCons::set_search_direction(aalta_formula *X)
{
    assert(SAT_TRACE_FLAG && current_X_idx_ == -1);
    current_X_idx_ = find_match_X_idx(X);
    if (current_X_idx_ == -1)
        return NULL;
    return X_parts_[current_X_idx_];
}

int XCons::find_match_X_idx(aalta_formula *X)
{
    unordered_set<int> target_x;
    X->to_set(target_x);
    for (int i = 0; i < X_parts_.size(); ++i)
        if (swin_X_idx_.find(i) == swin_X_idx_.end() && searched_X_idx_.find(i) == searched_X_idx_.end())
        {
            unordered_set<int> tmp;
            X_parts_[i]->to_set(tmp);
            if (isCompatible(&target_x, &tmp))
                return i;
        }
    return -1; // fail to find
}

} // namespace whole_dfa