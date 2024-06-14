#include <algorithm>
#include <iostream>
#include <queue>

#include "ltlf2dfa/synthesis.h"
#include "synutil/formula_in_bdd.h"
#include "synutil/preprocess.h"
#include "synutil/syn_states.h"

using namespace std;
using namespace aalta;

namespace whole_dfa {

bool SAT_TRACE_FLAG = false;

unordered_map<ull, int> dfn;
unordered_map<ull, int> low;
int dfs_time;

string getStatusStr(Status status)
{
    switch (status)
    {
    case Swin:
        return "Swin";
    case Ewin:
        return "Ewin";
    case Dfs_complete:
        return "Dfs_complete";
    case Dfs_incomplete:
        return "Dfs_incomplete";
    default:
        return "Unknown";
    }
}

bool search_whole_DFA(Syn_Frame *init_frame, Syn_Graph &graph)
{
    dfs_time = 0;
    dfn.clear(), low.clear();
    int dfs_cur = 0;

    // set dfn and low value for cur_frame (init_frame)
    initial_tarjan_frame(init_frame);

    vector<Syn_Frame *> tarjan_sta; // for tarjan algorithm
    vector<Syn_Frame *> dfs_sta;    // for DFS
    dfs_sta.push_back(init_frame);
    tarjan_sta.push_back(init_frame);

    unordered_map<ull, int> prefix_bdd2curIdx_map;
    unordered_map<ull, int> bdd2tarjanRootTimeId_map;
    prefix_bdd2curIdx_map.insert({ull(init_frame->GetBddPointer()), dfs_cur});
    bdd2tarjanRootTimeId_map.insert({ull(init_frame->GetBddPointer()), -1});
    queue<pair<aalta_formula *, aalta_formula *>> model;
    while (dfs_cur >= 0)
    {
        Status cur_state_status = dfs_sta[dfs_cur]->checkStatus();
        DdNode *cur_bddP = dfs_sta[dfs_cur]->GetBddPointer();
        bool cur_state_should_stopSearch_flag = dfs_sta[dfs_cur]->should_stopSearch();
        if (cur_state_should_stopSearch_flag)
        {
            aalta_formula *cur_afP = dfs_sta[dfs_cur]->GetFormulaPointer();
            if (dfn.at((ull)cur_bddP) == low.at((ull)cur_bddP))
            {
                vector<Syn_Frame *> scc;
                getScc(dfs_cur, scc, tarjan_sta, bdd2tarjanRootTimeId_map);
                backwardSearch(scc);
                addSccToGraph(scc, graph);
                dout << "=getScc=\t" << scc.size() << endl;
                for (auto it : scc)
                {
                    dout << "\t" << it->GetBddPointer() << "\t" << getStatusStr(it->get_status()) << "\t"
                         << it->GetFormulaPointer()->to_string() << endl;
                }
                for (auto it : scc)
                    delete it;
            }
            else
            {
                dout << "=pop="
                     << "\t"
                     << "cur_bddP is not a root of SCC" << endl;
                dout << "\t" << dfs_sta[dfs_cur]->GetBddPointer() << "\t" << getStatusStr(dfs_sta[dfs_cur]->get_status()) << "\t"
                     << dfs_sta[dfs_cur]->GetFormulaPointer()->to_string() << endl;
            }
            prefix_bdd2curIdx_map.erase((ull)cur_bddP);
            dfs_sta.pop_back();
            --dfs_cur;
            dout << "\t"
                 << "pre state:"
                 << "\t";
            if (dfs_cur < 0)
                dout << "DFS complete" << endl;
            else
                dout << "\t" << dfs_sta[dfs_cur]->GetBddPointer() << "\t" << getStatusStr(dfs_sta[dfs_cur]->get_status()) << "\t"
                     << dfs_sta[dfs_cur]->GetFormulaPointer()->to_string() << endl;
            if (dfs_cur < 0)
            {
                syn_states::releasePredecessors();
                return cur_state_status == Swin;
            }
            else
            {
                Syn_Frame *predecessor_fr = dfs_sta[dfs_cur];
                Signal signal = To_swin;
                if (cur_state_status == Ewin)
                    signal = To_ewin;
                else if (cur_state_status == Dfs_complete)
                    signal = Pending;
                predecessor_fr->processSignal(signal, cur_bddP);

                while (!model.empty())
                    model.pop();

                update_by_low(predecessor_fr, cur_bddP);
                continue;
            }
        }

        unordered_set<int> edge_var_set;
        bool exist_edge_to_explorer = dfs_sta[dfs_cur]->getEdge(edge_var_set, model);

        if (!exist_edge_to_explorer)
            continue;

        {
            aalta_formula *next_af = FormulaProgression_empty(dfs_sta[dfs_cur]->GetFormulaPointer(), edge_var_set); //->simplify();
            dout << "\t" << dfs_sta[dfs_cur]->GetFormulaPointer()->to_string() << endl;
            dout << "\t\t";
            for (auto it : edge_var_set)
                dout << (it > 0 ? "" : "!") << aalta_formula::get_name(abs(it)) << ", ";
            dout << endl;
            dout << "\t" << next_af->to_string() << endl;
            Syn_Frame *next_frame = new Syn_Frame(next_af);
            /**
             * Goal: Avoid insert ERROR of predecessors set
             * TODO-opt: In fact, next_bddp cannot be FALSE
             */
            if ((next_frame->GetBddPointer() != FormulaInBdd::TRUE_bddP_) && (next_frame->GetBddPointer() != FormulaInBdd::FALSE_bddP_))
                syn_states::addToGraph(dfs_sta[dfs_cur]->GetBddPointer(), next_frame->GetBddPointer());
            if (dfn.find(ull(next_frame->GetBddPointer())) == dfn.end())
            {
                initial_tarjan_frame(next_frame);

                dfs_sta.push_back(next_frame);
                tarjan_sta.push_back(next_frame);
                dfs_cur++;
                prefix_bdd2curIdx_map.insert({(ull)next_frame->GetBddPointer(), dfs_cur});
                bdd2tarjanRootTimeId_map.insert({(ull)next_frame->GetBddPointer(), -1});
            }
            else
            {
                // update low
                if (bdd2tarjanRootTimeId_map.at(ull(next_frame->GetBddPointer())) == -1)
                    update_by_dfn(dfs_sta[dfs_cur], next_frame);

                // do synthesis feedback
                if (prefix_bdd2curIdx_map.find((ull)next_frame->GetBddPointer()) != prefix_bdd2curIdx_map.end())
                {
                    /**
                     * cur_Y has X -> prefix, cannot make cur_state undetermined
                     * only all Y has X -> prefix, can make cur_state undetermined
                     */
                    // self loop is processed in the constructiobn of edgeCons
                    assert(next_frame->GetBddPointer() != dfs_sta[dfs_cur]->GetBddPointer());
                    dfs_sta[dfs_cur]->processSignal(Pending, next_frame->GetBddPointer());
                    while (!model.empty())
                        model.pop();
                }
                else // else: has cur-- (moved backward)
                {
                    Status next_state_status;
                    next_state_status = syn_states::getBddStatus(next_frame->GetBddPointer());
                    assert(next_state_status != Dfs_incomplete);
                    Signal sig = To_swin;
                    if (next_state_status == Ewin)
                        sig = To_ewin;
                    else if (next_state_status == Dfs_complete)
                        sig = Pending;
                    dfs_sta[dfs_cur]->processSignal(sig, next_frame->GetBddPointer());
                    while (!model.empty())
                        model.pop();
                    /*
                     * TODO: whether modify ull(next_frame->GetBddPointer()) back to dfs_sta[dfs_cur]->current_next_stateid_?
                     *              need assign dfs_sta[dfs_cur]->current_next_stateid_ in getEdge!!!
                     */
                }
                delete next_frame;
            }
        }
    }
}

void backwardSearch(vector<Syn_Frame *> &scc)
{
    unordered_map<ull, Syn_Frame *> bddP_to_synFrP;
    set<DdNode *> cur_swin, new_swin, undecided;

    for (auto it : scc)
    {
        if (it->get_status() == Swin)
            cur_swin.insert(it->GetBddPointer());
        else if (it->get_status() == Dfs_complete)
        {
            undecided.insert(it->GetBddPointer());
            bddP_to_synFrP[ull(it->GetBddPointer())] = it;
        }
    }
    if (undecided.empty())
        return;

    set<DdNode *> candidate_new_swin;
    set<DdNode *> tmp_set;
    do
    {
        new_swin.clear();
        candidate_new_swin.clear();
        tmp_set.clear();
        for (auto it : cur_swin)
        {
            set<DdNode *> *pred = syn_states::getPredecessors(it);
            set_union(candidate_new_swin.begin(), candidate_new_swin.end(), pred->begin(), pred->end(), inserter(tmp_set, tmp_set.begin()));
            candidate_new_swin.swap(tmp_set);
            tmp_set.clear();
        }
        set_intersection(candidate_new_swin.begin(), candidate_new_swin.end(), undecided.begin(), undecided.end(),
                         inserter(tmp_set, tmp_set.begin()));
        candidate_new_swin.swap(tmp_set);
        tmp_set.clear();
        for (auto s : candidate_new_swin)
        {
            if (bddP_to_synFrP[ull(s)]->checkSwinForBackwardSearch())
            {
                syn_states::insert_swin_state(s, false);
                syn_states::remove_dfs_complete_state(s);
                new_swin.insert(s);
                undecided.erase(s);
            }
        }
        cur_swin = new_swin;
    } while (!new_swin.empty());
    for (auto s : undecided)
    {
        syn_states::insert_ewin_state(s, false);
        syn_states::remove_dfs_complete_state(s);
    }
    return;
}

void addSccToGraph(vector<Syn_Frame *> &scc, Syn_Graph &graph)
{
    for (auto syn_frame_ptr : scc)
    {
        auto bddp = syn_frame_ptr->GetBddPointer();
        graph.add_vertex(bddp);
        if (syn_frame_ptr->get_status() == Ewin || syn_states::is_ewin_state(bddp))
            continue;
        vector<Syn_Edge> succ_edges;
        syn_frame_ptr->get_succ_edges(succ_edges);
        for (auto syn_edge : succ_edges)
        {
            // cout << "||\t" << ull(bddp) << " -> " << ull(syn_edge.first) << "\tby\t" << syn_edge.second->to_string() << endl;
            /* NOTE: self-loop in sub_af cannot be deleted, as they may not self-loop in whole_af */
            // if (ull(bddp) == ull(syn_edge.first))
            //     continue;
            graph.add_edge(bddp, syn_edge.first, syn_edge.second);
        }
    }
}

void initial_tarjan_frame(Syn_Frame *cur_frame)
{
    dfn.insert({ull(cur_frame->GetBddPointer()), dfs_time});
    low.insert({ull(cur_frame->GetBddPointer()), dfs_time});
    ++dfs_time;
}

void update_by_low(Syn_Frame *cur_frame, DdNode *next_bddP)
{
    low[(ull)cur_frame->GetBddPointer()] = min(low.at((ull)cur_frame->GetBddPointer()), low.at((ull)next_bddP));
}

void update_by_dfn(Syn_Frame *cur_frame, Syn_Frame *next_frame)
{
    low[(ull)cur_frame->GetBddPointer()] = min(low.at((ull)cur_frame->GetBddPointer()), dfn.at((ull)next_frame->GetBddPointer()));
}

void getScc(int dfs_cur, std::vector<Syn_Frame *> &scc, vector<Syn_Frame *> &tarjan_sta, unordered_map<ull, int> &bdd2tarjanRootTimeId_map)
{
    int lowTimeId = dfn.at((ull)tarjan_sta[dfs_cur]->GetBddPointer());

    do
    {
        scc.push_back(tarjan_sta.back());
        bdd2tarjanRootTimeId_map.at(ull(tarjan_sta.back()->GetBddPointer())) = lowTimeId;
        tarjan_sta.pop_back();
    } while (dfn.at(ull(scc.back()->GetBddPointer())) != lowTimeId);
    assert(dfn.at(ull(scc.back()->GetBddPointer())) == lowTimeId);
}

Syn_Frame::Syn_Frame(aalta_formula *af) : status_(Dfs_incomplete), edgeCons_(NULL), swin_checked_idx_(0), ewin_checked_idx_(0)
{
    state_in_bdd_ = new FormulaInBdd(af, xnf_empty(af));
    DdNode *bddp = state_in_bdd_->GetBddPointer();
    if (!syn_states::in_isAcc_byEmpty_map(bddp))
        syn_states::set_isAcc_byEmpty(bddp, IsEmptyAcc(af->nnf()));
    edgeCons_ = new edgeCons(bddp, af, aalta_formula::FALSE());
    edgeCons_->check_hasTravAllEdges();
    status_ = edgeCons_->get_status();
    if (syn_states::isAcc_byEmpty(bddp))
        status_ = Swin;
}

Syn_Frame::~Syn_Frame()
{
    delete edgeCons_;
    delete state_in_bdd_;
}

Status Syn_Frame::checkStatus()
{
    status_ = edgeCons_->get_status();
    DdNode *bddp = GetBddPointer();
    if (syn_states::isAcc_byEmpty(bddp))
        status_ = Swin;

    if (status_ == Dfs_incomplete)
        status_ = syn_states::get_status_by_set(bddp);
    if (status_ != Dfs_incomplete)
    {
        syn_states::insert_state_with_status(bddp, status_);
        return status_;
    }
    else
        status_ = syn_states::get_status_by_set_imply(bddp, swin_checked_idx_, ewin_checked_idx_);

    return status_;
}

// partition atoms and save index values respectively
void PartitionAtoms(aalta_formula *af, unordered_set<string> &env_val)
{
    if (af == NULL)
        return;
    int op = af->oper();
    if (op >= 11)
        if (env_val.find(af->to_string()) != env_val.end())
            X_vars.insert(op);

        else
            Y_vars.insert(op);

    else
    {
        PartitionAtoms(af->l_af(), env_val);
        PartitionAtoms(af->r_af(), env_val);
    }
}

void Syn_Frame::processSignal(Signal sig, DdNode *succ)
{
    /* NOTE: add isAcc_byEmpty_bddP_map to force those states processSignal with Swin to their predecessors */
    if (syn_states::isAcc_byEmpty(state_in_bdd_->GetBddPointer()))
        sig = To_swin;
    edgeCons_->processSignal(sig, succ);
}

bool Syn_Frame::getEdge(unordered_set<int> &edge, queue<pair<aalta_formula *, aalta_formula *>> &model)
{
    return edgeCons_->getEdge(edge, model);
}

bool Syn_Frame::should_stopSearch()
{
    return (edgeCons_->get_status() == Ewin) || (edgeCons_->hasTravAllEdges());
}

void Syn_Frame::get_succ_edges(vector<Syn_Edge> &succ_edges)
{
    return edgeCons_->get_succ_edges(succ_edges);
}

bool Syn_Frame::checkSwinForBackwardSearch()
{
    return edgeCons_->checkSwinForBackwardSearch();
}

} // namespace whole_dfa