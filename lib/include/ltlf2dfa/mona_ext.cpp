#include "ltlf2dfa/mona_ext.h"
#include "synutil/formula_in_bdd.h"
#include "synutil/syn_states.h"
#include <cstring>

typedef unsigned long long ull;

void printDotFile(DFA *dfa, string dot_filename, int var_num, vector<unsigned int> var_index)
{
    FILE *original_stdout = stdout;
    stdout = fopen(dot_filename.data(), "w");
    // === real code BEGIN
    dfaPrintGraphviz(dfa, var_num, var_index.data());
    // === real code END
    fclose(stdout);
    stdout = original_stdout;
}

void printDfaFile(DFA *dfa, string dfa_filename, int var_num, std::vector<std::string> &var_names, std::vector<char> &var_orders)
{
    std::vector<char *> var_names_ptr;
    for (auto var_name : var_names)
    {
        char *var_name_ptr = new char[var_name.size() + 1];
        std::strcpy(var_name_ptr, var_name.c_str());
        var_names_ptr.push_back(var_name_ptr);
    }
    dfaExport(dfa, dfa_filename.data(), var_num, var_names_ptr.data(), var_orders.data());
}

DFA *graph2DFA(Syn_Graph &graph, DdNode *init_bddP, int var_num, std::vector<int> indicies)
{
    // assert(graph.vertices.size() > 2);  // NOTE: other cases: 1. sth. -> alway true/false 2. init is true/false !!!
    unordered_map<ull, int> bddP_to_stateid;
    unordered_map<int, ull> stateid_to_bddP;
    int stateid_cnt = 0;
    // insert all vertex into bddP_to_stateid
    // INSERT-1. init_bddP
    int init_stateid = 0;
    auto init_vertex_Iter = graph.vertices.find(init_bddP);
    assert(init_vertex_Iter != graph.vertices.end());
    bddP_to_stateid.insert({ull(init_bddP), stateid_cnt++});
    stateid_to_bddP.insert({stateid_cnt - 1, ull(init_bddP)});
    assert(init_stateid == stateid_cnt - 1);
    graph.vertices.erase(init_bddP);
    // INSERT-2. false and true
    int true_stateid = 1, false_stateid = 2;
    bddP_to_stateid.insert({ull(FormulaInBdd::TRUE_bddP_), stateid_cnt++});
    stateid_to_bddP.insert({stateid_cnt - 1, ull(FormulaInBdd::TRUE_bddP_)});
    assert(true_stateid == stateid_cnt - 1);
    bddP_to_stateid.insert({ull(FormulaInBdd::FALSE_bddP_), stateid_cnt++});
    stateid_to_bddP.insert({stateid_cnt - 1, ull(FormulaInBdd::FALSE_bddP_)});
    assert(false_stateid == stateid_cnt - 1);
    graph.vertices.erase(FormulaInBdd::TRUE_bddP_);
    graph.vertices.erase(FormulaInBdd::FALSE_bddP_);
    // INSERT-2. others
    for (auto vertex : graph.vertices)
    {
        assert(bddP_to_stateid.find(ull(vertex)) == bddP_to_stateid.end());
        bddP_to_stateid.insert({ull(vertex), stateid_cnt++});
        stateid_to_bddP.insert({stateid_cnt - 1, ull(vertex)});
    }

    dfaSetup(bddP_to_stateid.size(), var_num, indicies.data());
    vector<bool> state_visited(bddP_to_stateid.size(), 0);
    // EDGE-1. init_bddP
    auto init_edges_Iter = graph.edges.find(init_bddP);
    if (init_edges_Iter != graph.edges.end())
    {
        auto init_succ_edges = init_edges_Iter->second;
        dfaAllocExceptions(init_succ_edges.size());
        for (auto edge : init_succ_edges)
        {
            int dest_stateid = bddP_to_stateid[ull(edge.dest)];
            auto bin_edge_ptr = af2binaryString(edge.label);
            dfaStoreException(dest_stateid, bin_edge_ptr->data());
            delete bin_edge_ptr;
        }
        dfaStoreState(false_stateid);
        graph.edges.erase(init_bddP);
    }
    else
    {
        dfaAllocExceptions(0);
        dfaStoreState(false_stateid);
    }
    state_visited[init_stateid] = true;
    // EDGE-2. true and false
    dfaAllocExceptions(0);
    dfaStoreState(true_stateid);
    state_visited[true_stateid] = true;
    dfaAllocExceptions(0);
    dfaStoreState(false_stateid);
    state_visited[false_stateid] = true;
    // EDGE-3. others
    assert(stateid_cnt == bddP_to_stateid.size());
    for (int i = 0; i < stateid_cnt; i++)
    {
        if (state_visited[i])
            continue;
        auto vertex = (DdNode *)stateid_to_bddP[i];
        auto succ_edges_Iter = graph.edges.find(vertex);
        if (succ_edges_Iter == graph.edges.end())
        {
            dfaAllocExceptions(0);
            dfaStoreState(false_stateid);
            continue;
        }
        /* Else */
        auto succ_edges = succ_edges_Iter->second;
        dfaAllocExceptions(succ_edges.size());
        for (auto edge : succ_edges)
        {
            int dest_stateid = bddP_to_stateid[ull(edge.dest)];
            auto bin_edge_ptr = af2binaryString(edge.label);
            dfaStoreException(dest_stateid, bin_edge_ptr->data());
            delete bin_edge_ptr;
        }
        dfaStoreState(false_stateid);
    }
    // get state_type_arr_s
    assert(bddP_to_stateid.size() > 2);
    string state_type_s = string(bddP_to_stateid.size(), '0');
    for (auto bddP_and_stateid_pair : bddP_to_stateid)
    {
        auto bddP = bddP_and_stateid_pair.first;
        auto stateid = bddP_and_stateid_pair.second;
        if (syn_states::in_isAcc_byEmpty_map(bddP) && syn_states::isAcc_byEmpty(bddP))
            state_type_s[stateid] = '+';
        else if (syn_states::is_ewin_state(bddP))
            state_type_s[stateid] = '-';
        else
            state_type_s[stateid] = '0';
    }
    /**
     * DONE: change state_type of some state from 0 to +
     * NOTE: in following context, a is env var, b is sys var
     * 1. wnext state, e.g.
     *      - X(a)
     *      - X(a) & X(b)
     *      - X(a) & a
     *      - a & X(b)
     * 2. release state, e.g.
     *      - false R a     i.e.      G(a)
     */
    vector<char> state_type_ch_arr(state_type_s.c_str(), state_type_s.c_str() + state_type_s.size() + 1);
    DFA *dfa = dfaBuild(state_type_ch_arr.data());
    return dfa;
}