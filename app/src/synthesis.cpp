#include "synthesis.h"
#include "ltlfsyn/synthesis.h"
#include "synutil/formula_in_bdd.h"
#include "synutil/partvar.h"
#include "synutil/syn_states.h"

bool is_realizable(aalta::aalta_formula *src_formula, std::unordered_set<std::string> &env_var, bool verbose = false)
{
    clear_XY_vars();
    PartitionAtoms(src_formula, env_var);
    // number of variables
    calc_XY_var_nums();

    FormulaInBdd::InitBdd4LTLf(src_formula);
    syn_states::insert_swin_state(FormulaInBdd::TRUE_bddP_);
    syn_states::insert_ewin_state(FormulaInBdd::FALSE_bddP_);
    Syn_Frame *init = new Syn_Frame(src_formula);

    hash_set<aalta_formula *> and_sub_afs = src_formula->and_to_set();
    for (auto it : and_sub_afs)
    {
        Syn_Frame *init = new Syn_Frame(it);
        if (!forwardSearch(init))
            return false;
    }

    if (and_sub_afs.size() == 1)
        return true;

    return true;
}