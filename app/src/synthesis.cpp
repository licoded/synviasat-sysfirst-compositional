#include "synthesis.h"
#include "debug.h"
#include "ltlf2dfa/mona_ext.h"
#include "ltlf2dfa/syft_ext.h"
#include "ltlf2dfa/synthesis.h"
#include "ltlfsyn/synthesis.h"
#include "synutil/formula_in_bdd.h"
#include "synutil/partvar.h"
#include "synutil/syn_states.h"
#include <numeric>
#include <vector>

void get_var_names() {}

bool is_realizable(aalta::aalta_formula *src_formula, std::unordered_set<std::string> &env_var, std::string partfile, bool verbose = false)
{
    clear_XY_vars();
    ltlfsyn::PartitionAtoms(src_formula, env_var);
    // number of variables
    calc_XY_var_nums();

    FormulaInBdd::InitBdd4LTLf(src_formula);
    syn_states::insert_swin_state(FormulaInBdd::TRUE_bddP_);
    syn_states::insert_ewin_state(FormulaInBdd::FALSE_bddP_);
    ltlfsyn::Syn_Frame *init = new ltlfsyn::Syn_Frame(src_formula);

    hash_set<aalta_formula *> and_sub_afs = src_formula->and_to_set();
    for (auto it : and_sub_afs)
    {
        ltlfsyn::Syn_Frame *init = new ltlfsyn::Syn_Frame(it);
        if (!ltlfsyn::forwardSearch(init))
            return false;
    }

    if (and_sub_afs.size() == 1)
        return true;

    whole_dfa::SAT_TRACE_FLAG = false;
    syn_states::set_isAcc_byEmpty(FormulaInBdd::TRUE_bddP_, true);
    syn_states::set_isAcc_byEmpty(FormulaInBdd::FALSE_bddP_, false);

    // prepare for export DFA
    int var_num = total_var_nums;
    std::vector<std::string> var_names(total_var_nums);
    for (auto item : X_vars)
        var_names[item - 12] = aalta_formula(item, NULL, NULL).to_string();
    for (auto item : Y_vars)
        var_names[item - 12] = aalta_formula(item, NULL, NULL).to_string();
    std::vector<char> orders(var_num, 2);
    std::vector<unsigned int> var_index(var_num);
    std::vector<int> indicies(var_num);
    for (int i = 0; i < var_num; i++)
    {
        var_index[i] = i;
        indicies[i] = i;
    }

#ifdef DEBUG
    system("mkdir -p examples/temp-drafts");
#endif
    // get whole DFA
    bool Minimize_FLAG = true;
    DFA *dfa = dfaTrue(), *dfa_cur_min;
    for (auto it : and_sub_afs)
    {
        Syn_Graph graph;

        whole_dfa::Syn_Frame *init = new whole_dfa::Syn_Frame(it);
        DdNode *init_bddP = init->GetBddPointer();
        dout << "sub_af:\t" << it->to_string() << endl;
        whole_dfa::search_whole_DFA(init, graph);
#ifdef DEBUG
        printGraph(graph); // for DEBUG
#endif

        DFA *dfa_cur = graph2DFA(graph, init_bddP, var_num, indicies);
        if (Minimize_FLAG)
        {
            dfa_cur_min = dfaMinimize(dfa_cur);
            free(dfa_cur);
        }
        else
            dfa_cur_min = dfa_cur;
#ifdef DEBUG
        string af_s = it->to_string();
        af_s.erase(remove(af_s.begin(), af_s.end(), ' '), af_s.end()); // delete all spaces from af_s
        printDfaFile(dfa_cur_min, "examples/temp-drafts/" + af_s + ".dfa", var_num, var_names, orders);
        printDotFile(dfa_cur_min, "examples/temp-drafts/" + af_s + ".dot", var_num, var_index);
#endif
        dfa = dfaProduct(dfa, dfa_cur_min, dfaAND);
        free(dfa_cur_min);
        if (Minimize_FLAG)
            dfa = dfaMinimize(dfa);
    }

    char *temp_filename = tmpnam(nullptr);
    printDfaFile(dfa, string(temp_filename), var_num, var_names, orders);

    bool syn_res = syft_check_synthesis(SynType::SYS_FIRST, string(temp_filename), partfile);

#ifdef DEBUG
    string wholedfa2dot_filename = "examples/temp-drafts/whole_dfa2.dot";
    printDotFile(dfa, "examples/temp-drafts/whole.dot", var_num, var_index);
    printDfaFile(dfa, "examples/temp-drafts/whole.dfa", var_num, var_names, orders);
    system("dfa2dot examples/temp-drafts/whole.dot examples/temp-drafts/whole_dfa2.dot");
#endif
    free(dfa);

    return syn_res;
}