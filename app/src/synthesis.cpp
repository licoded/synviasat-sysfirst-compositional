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

    return forwardSearch(init);
}