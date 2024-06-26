#include "synutil/af_utils.h"
#include "synutil/partvar.h"
#include <cassert>
#include <map>

std::map<aalta_formula *, aalta_formula *> f_to_xnf;

aalta_formula *xnf(aalta_formula *phi)
{
    if (phi == NULL)
        return NULL;
    if (f_to_xnf.find(phi) != f_to_xnf.end())
        return f_to_xnf[phi];
    int op = phi->oper();
    if (op >= 11)
        return phi;
    switch (op)
    {
    case aalta_formula::True:
    case aalta_formula::False:
    case aalta_formula::Not:
    case aalta_formula::Next:
    case aalta_formula::WNext:
    {
        return phi;
    }
    case aalta_formula::And:
    {
        aalta_formula *xnf_phi;
        aalta_formula *lf = xnf(phi->l_af());
        if ((lf->oper()) == aalta_formula::False)
            xnf_phi = aalta_formula::FALSE();
        else
        {
            aalta_formula *rf = xnf(phi->r_af());
            if ((rf->oper()) == aalta_formula::False)
                xnf_phi = aalta_formula::FALSE();
            else if ((lf->oper()) == aalta_formula::True)
                xnf_phi = rf;
            else if ((rf->oper()) == aalta_formula::True)
                xnf_phi = lf;
            else
                xnf_phi = aalta_formula(aalta_formula::And, lf, rf).unique();
        }
        f_to_xnf.insert({phi, xnf_phi});
        return xnf_phi;
    }
    case aalta_formula::Or:
    {
        aalta_formula *xnf_phi;
        aalta_formula *lf = xnf(phi->l_af());
        if ((lf->oper()) == aalta_formula::True)
            xnf_phi = aalta_formula::TRUE();
        else
        {
            aalta_formula *rf = xnf(phi->r_af());
            if ((rf->oper()) == aalta_formula::True)
                xnf_phi = aalta_formula::TRUE();
            else if ((lf->oper()) == aalta_formula::False)
                xnf_phi = rf;
            else if ((rf->oper()) == aalta_formula::False)
                xnf_phi = lf;
            else
                xnf_phi = aalta_formula(aalta_formula::Or, lf, rf).unique();
        }
        f_to_xnf.insert({phi, xnf_phi});
        return xnf_phi;
    }
    case aalta_formula::Until:
    { // l U r=xnf(r) | (xnf(l) & X(l U r))
        aalta_formula *next_phi = aalta_formula(aalta_formula::Next, NULL, phi).unique();
        aalta_formula *xnf_l_and_next_phi = (phi->l_af()->oper() == aalta_formula::True)
                                                ? next_phi // futrue
                                                : aalta_formula(aalta_formula::And, xnf(phi->l_af()), next_phi).unique();
        aalta_formula *xnf_phi = aalta_formula(aalta_formula::Or, xnf(phi->r_af()), xnf_l_and_next_phi).unique();
        f_to_xnf.insert({phi, xnf_phi});
        return xnf_phi;
    }
    case aalta_formula::Release:
    { // l R r=xnf(r) & (xnf(l) | WX(l R r))
        aalta_formula *wnext_phi = aalta_formula(aalta_formula::WNext, NULL, phi).unique();
        aalta_formula *xnf_l_or_wnext_phi = (phi->l_af()->oper() == aalta_formula::False)
                                                ? wnext_phi // global
                                                : aalta_formula(aalta_formula::Or, xnf(phi->l_af()), wnext_phi).unique();
        aalta_formula *xnf_phi = aalta_formula(aalta_formula::And, xnf(phi->r_af()), xnf_l_or_wnext_phi).unique();
        f_to_xnf.insert({phi, xnf_phi});
        return xnf_phi;
    }
    }
}

aalta_formula *xnf_empty(aalta_formula *phi)
{
    if (phi == NULL)
        return NULL;
    if (f_to_xnf.find(phi) != f_to_xnf.end())
        return f_to_xnf[phi];
    int op = phi->oper();
    if (op >= 11)
        return phi;
    switch (op)
    {
    case aalta_formula::True:
    case aalta_formula::False:
    case aalta_formula::Not:
    case aalta_formula::Next:
    case aalta_formula::WNext:
    {
        return phi;
    }
    case aalta_formula::And:
    {
        aalta_formula *lf = xnf_empty(phi->l_af());
        aalta_formula *rf = xnf_empty(phi->r_af());
        if ((lf->oper()) == aalta_formula::False || (rf->oper()) == aalta_formula::False)
            return aalta_formula::FALSE();
        if ((lf->oper()) == aalta_formula::True)
            return rf;
        if ((rf->oper()) == aalta_formula::True)
            return lf;
        return aalta_formula(aalta_formula::And, lf, rf).unique();
    }
    case aalta_formula::Or:
    {
        aalta_formula *lf = xnf_empty(phi->l_af());
        aalta_formula *rf = xnf_empty(phi->r_af());
        if ((lf->oper()) == aalta_formula::True || (rf->oper()) == aalta_formula::True)
            return aalta_formula::TRUE();
        if ((lf->oper()) == aalta_formula::False)
            return rf;
        if ((rf->oper()) == aalta_formula::False)
            return lf;
        return aalta_formula(aalta_formula::Or, lf, rf).unique();
    }
    case aalta_formula::Until:
    { // l U r=xnf_empty(r) | (xnf_empty(l) & X(l U r))
        // keep old, F true = true U true
        // if (phi->l_af()->oper() == aalta_formula::True && phi->r_af()->oper() == aalta_formula::True)
        if (phi->l_af() == aalta_formula::TRUE() && phi->r_af() == aalta_formula::TRUE())
            return phi;
        aalta_formula *lf = xnf_empty(phi->l_af());
        aalta_formula *rf = xnf_empty(phi->r_af());
        aalta_formula *firstpart = NULL;
        if (rf->oper() == aalta_formula::False)
            firstpart = aalta_formula::FALSE();
        else
        {
            aalta_formula *F_true_af = aalta_formula(aalta_formula::Until, aalta_formula::TRUE(), aalta_formula::TRUE()).unique();
            firstpart = aalta_formula(aalta_formula::And, rf, F_true_af).unique();
        }
        aalta_formula *secondpart = NULL;
        if (lf->oper() == aalta_formula::False)
            secondpart = aalta_formula::FALSE();
        else
        {
            aalta_formula *next_phi = aalta_formula(aalta_formula::Next, NULL, phi).unique();
            secondpart = aalta_formula(aalta_formula::And, lf, next_phi).unique();
        }
        return aalta_formula(aalta_formula::Or, firstpart, secondpart).unique();
    }
    case aalta_formula::Release:
    { // l R r=xnf_empty(r) & (xnf_empty(l) | WX(l R r))
        // keep old, G false = false R false
        // if (phi->l_af()->oper() == aalta_formula::False && phi->r_af()->oper() == aalta_formula::False)
        if (phi->l_af() == aalta_formula::FALSE() && phi->r_af() == aalta_formula::FALSE())
            return phi;
        aalta_formula *lf = xnf_empty(phi->l_af());
        aalta_formula *rf = xnf_empty(phi->r_af());
        aalta_formula *firstpart = NULL;
        if (rf->oper() == aalta_formula::True)
            firstpart = aalta_formula::TRUE();
        else
        {
            aalta_formula *G_false_af = aalta_formula(aalta_formula::Release, aalta_formula::FALSE(), aalta_formula::FALSE()).unique();
            firstpart = aalta_formula(aalta_formula::Or, rf, G_false_af).unique();
        }
        aalta_formula *secondpart = NULL;
        if (lf->oper() == aalta_formula::True)
            firstpart = aalta_formula::TRUE();
        else
        {
            aalta_formula *wnext_phi = aalta_formula(aalta_formula::WNext, NULL, phi).unique();
            secondpart = aalta_formula(aalta_formula::Or, lf, wnext_phi).unique();
        }
        return aalta_formula(aalta_formula::And, firstpart, secondpart).unique();
    }
    }
}

aalta_formula *FormulaProgression(aalta_formula *predecessor, unordered_set<int> &edge)
{
    if (predecessor == NULL)
        return NULL;
    int op = predecessor->oper();
    if (op == aalta_formula::Not || op >= 11)
    { // literal
        int lit = (op >= 11) ? op : (-((predecessor->r_af())->oper()));
        return ((edge.find(lit) != edge.end()) ? aalta_formula::TRUE() : aalta_formula::FALSE());
    }
    switch (op)
    {
    case aalta_formula::True:
    case aalta_formula::False:
    {
        return predecessor;
    }
    case aalta_formula::And:
    {
        aalta_formula *lf = FormulaProgression(predecessor->l_af(), edge);
        if ((lf->oper()) == aalta_formula::False)
            return aalta_formula::FALSE();
        aalta_formula *rf = FormulaProgression(predecessor->r_af(), edge);
        if ((rf->oper()) == aalta_formula::False)
            return aalta_formula::FALSE();
        else if ((lf->oper()) == aalta_formula::True)
            return rf;
        else if ((rf->oper()) == aalta_formula::True)
            return lf;
        else
            return aalta_formula(aalta_formula::And, lf, rf).unique();
    }
    case aalta_formula::Or:
    {
        aalta_formula *l_fp = FormulaProgression(predecessor->l_af(), edge);
        if ((l_fp->oper()) == aalta_formula::True)
            return aalta_formula::TRUE();
        aalta_formula *r_fp = FormulaProgression(predecessor->r_af(), edge);
        if ((r_fp->oper()) == aalta_formula::True)
            return aalta_formula::TRUE();
        else if ((l_fp->oper()) == aalta_formula::False)
            return r_fp;
        else if ((r_fp->oper()) == aalta_formula::False)
            return l_fp;
        else
            return aalta_formula(aalta_formula::Or, l_fp, r_fp).unique();
    }
    case aalta_formula::Next:
    case aalta_formula::WNext:
    {
        return (predecessor->r_af())->unique();
    }
    // if predecessor is in XNF,
    // the following two cases cannot appear
    case aalta_formula::Until:
    { // l U r = r | (l & X(l U r))
        aalta_formula *first_part = FormulaProgression(predecessor->r_af(), edge);
        if ((first_part->oper()) == aalta_formula::True)
            return aalta_formula::TRUE();
        aalta_formula *l_fp = FormulaProgression(predecessor->l_af(), edge);
        aalta_formula *second_part = NULL;
        if ((l_fp->oper()) == aalta_formula::True)
        {
            if (first_part == predecessor->r_af())
                return predecessor;
            second_part = predecessor;
        }
        else if ((l_fp->oper()) == aalta_formula::False)
            return first_part;
        else
            second_part = aalta_formula(aalta_formula::And, l_fp, predecessor).unique();
        if ((first_part->oper()) == aalta_formula::False)
            return second_part;
        else
            return aalta_formula(aalta_formula::Or, first_part, second_part).unique();
    }
    case aalta_formula::Release:
    { // l R r = r & (l | N(l R r))
        aalta_formula *first_part = FormulaProgression(predecessor->r_af(), edge);
        if ((first_part->oper()) == aalta_formula::False)
            return aalta_formula::FALSE();
        aalta_formula *l_fp = FormulaProgression(predecessor->l_af(), edge);
        aalta_formula *second_part = NULL;
        if ((l_fp->oper()) == aalta_formula::True)
            return first_part;
        else if ((l_fp->oper()) == aalta_formula::False)
            second_part = predecessor;
        else
            second_part = aalta_formula(aalta_formula::Or, l_fp, predecessor).unique();
        if ((first_part->oper()) == aalta_formula::True)
            return second_part;
        else
            return aalta_formula(aalta_formula::And, first_part, second_part).unique();
    }
    }
}

aalta_formula *FormulaProgression_empty(aalta_formula *predecessor, unordered_set<int> &edge)
{
    if (predecessor == NULL)
        return NULL;
    int op = predecessor->oper();
    if (op == aalta_formula::Not || op >= 11)
    { // literal
        int lit = (op >= 11) ? op : (-((predecessor->r_af())->oper()));
        return ((edge.find(lit) != edge.end()) ? aalta_formula::TRUE() : aalta_formula::FALSE());
    }
    switch (op)
    {
    case aalta_formula::True:
    case aalta_formula::False:
    {
        return predecessor;
    }
    case aalta_formula::And:
    {
        aalta_formula *lf = FormulaProgression_empty(predecessor->l_af(), edge);
        if ((lf->oper()) == aalta_formula::False)
            return aalta_formula::FALSE();
        aalta_formula *rf = FormulaProgression_empty(predecessor->r_af(), edge);
        if ((rf->oper()) == aalta_formula::False)
            return aalta_formula::FALSE();
        else if ((lf->oper()) == aalta_formula::True)
            return rf;
        else if ((rf->oper()) == aalta_formula::True)
            return lf;
        else
            return aalta_formula(aalta_formula::And, lf, rf).unique();
    }
    case aalta_formula::Or:
    {
        aalta_formula *l_fp = FormulaProgression_empty(predecessor->l_af(), edge);
        if ((l_fp->oper()) == aalta_formula::True)
            return aalta_formula::TRUE();
        aalta_formula *r_fp = FormulaProgression_empty(predecessor->r_af(), edge);
        if ((r_fp->oper()) == aalta_formula::True)
            return aalta_formula::TRUE();
        else if ((l_fp->oper()) == aalta_formula::False)
            return r_fp;
        else if ((r_fp->oper()) == aalta_formula::False)
            return l_fp;
        else
            return aalta_formula(aalta_formula::Or, l_fp, r_fp).unique();
    }
    case aalta_formula::Next:
    {
        aalta_formula *l_af = (predecessor->r_af())->unique();
        aalta_formula *r_af = aalta_formula(aalta_formula::Until, aalta_formula::TRUE(), aalta_formula::TRUE()).unique();
        return aalta_formula(aalta_formula::And, l_af, r_af).unique();
    }
    case aalta_formula::WNext:
    {
        aalta_formula *l_af = (predecessor->r_af())->unique();
        aalta_formula *r_af = aalta_formula(aalta_formula::Release, aalta_formula::FALSE(), aalta_formula::FALSE()).unique();
        return aalta_formula(aalta_formula::Or, l_af, r_af).unique();
    }
    // if predecessor is in XNF,
    // the following two cases cannot appear
    case aalta_formula::Until:
    { // l U r = r | (l & X(l U r))
        aalta_formula *first_part = FormulaProgression_empty(predecessor->r_af(), edge);
        if ((first_part->oper()) == aalta_formula::True)
            return aalta_formula::TRUE();
        aalta_formula *l_fp = FormulaProgression_empty(predecessor->l_af(), edge);
        aalta_formula *second_part = NULL;
        aalta_formula *predecessor_afterFP = FormulaProgression_empty(aalta_formula(aalta_formula::Next, NULL, predecessor).unique(), edge);
        if ((l_fp->oper()) == aalta_formula::True)
        {
            second_part = predecessor_afterFP;
        }
        else if ((l_fp->oper()) == aalta_formula::False)
            return first_part;
        else
            second_part = aalta_formula(aalta_formula::And, l_fp, predecessor_afterFP).unique();
        if ((first_part->oper()) == aalta_formula::False)
            return second_part;
        else
            return aalta_formula(aalta_formula::Or, first_part, second_part).unique();
    }
    case aalta_formula::Release:
    { // l R r = r & (l | N(l R r))
        aalta_formula *first_part = FormulaProgression_empty(predecessor->r_af(), edge);
        if ((first_part->oper()) == aalta_formula::False)
            return aalta_formula::FALSE();
        aalta_formula *l_fp = FormulaProgression_empty(predecessor->l_af(), edge);
        aalta_formula *second_part = NULL;
        if ((l_fp->oper()) == aalta_formula::True)
            return first_part;
        aalta_formula *predecessor_afterFP
            = FormulaProgression_empty(aalta_formula(aalta_formula::WNext, NULL, predecessor).unique(), edge);
        if ((l_fp->oper()) == aalta_formula::False)
            second_part = predecessor_afterFP;
        else
            second_part = aalta_formula(aalta_formula::Or, l_fp, predecessor_afterFP).unique();
        if ((first_part->oper()) == aalta_formula::True)
            return second_part;
        else
            return aalta_formula(aalta_formula::And, first_part, second_part).unique();
    }
    }
}

bool IsEmptyAcc(aalta_formula *state_af)
{
    if (state_af == NULL)
        return false;
    int op = state_af->oper();
    if (op >= 11) // literal
        return false;
    switch (op)
    {
    case aalta_formula::Not:
        return false;
    case aalta_formula::True:
    case aalta_formula::WNext:
    case aalta_formula::Release:
        return true;
    case aalta_formula::False:
    case aalta_formula::Next:
    case aalta_formula::Until:
        return false;
    case aalta_formula::And:
        return IsEmptyAcc(state_af->l_af()) && IsEmptyAcc(state_af->r_af());
    case aalta_formula::Or:
        return IsEmptyAcc(state_af->l_af()) || IsEmptyAcc(state_af->r_af());
    }
}

bool IsAcc(aalta_formula *predecessor, unordered_set<int> &tmp_edge)
{
    if (predecessor == NULL)
        return false;
    int op = predecessor->oper();
    if (op == aalta_formula::Not || op >= 11)
    { // literal
        int lit = (op >= 11) ? op : (-((predecessor->r_af())->oper()));
        return tmp_edge.find(lit) != tmp_edge.end();
    }
    switch (op)
    {
    case aalta_formula::True:
    case aalta_formula::WNext:
        return true;
    case aalta_formula::False:
    case aalta_formula::Next:
        return false;
    case aalta_formula::And:
        return IsAcc(predecessor->l_af(), tmp_edge) && IsAcc(predecessor->r_af(), tmp_edge);
    case aalta_formula::Or:
        return IsAcc(predecessor->l_af(), tmp_edge) || IsAcc(predecessor->r_af(), tmp_edge);
    case aalta_formula::Until:
    case aalta_formula::Release:
        return IsAcc(predecessor->r_af(), tmp_edge);
    }
}

void fill_in_edgeset(std::unordered_set<int> &partial_edgeset)
{
    if (partial_edgeset.size() == X_var_nums + Y_var_nums)
        return;
    for (auto it : X_vars)
    {
        if (partial_edgeset.find(it) == partial_edgeset.end() && partial_edgeset.find(-(it)) == partial_edgeset.end())
        {
            partial_edgeset.insert(it);
        }
    }
    for (auto it : Y_vars)
    {
        if (partial_edgeset.find(it) == partial_edgeset.end() && partial_edgeset.find(-(it)) == partial_edgeset.end())
        {
            partial_edgeset.insert(it);
        }
    }
}

void to_conjunts(aalta_formula *af, vector<aalta_formula *> &conjuncts)
{
    if (af == NULL)
        return;
    // if (af->oper() == aalta_formula::Not)
    //   conjuncts.push_back(af->r_af());
    // else
    if (af->oper() == aalta_formula::And) // || af->oper() == aalta_formula::Or)
    {
        to_conjunts(af->l_af(), conjuncts);
        to_conjunts(af->r_af(), conjuncts);
    }
    else
        conjuncts.push_back(af);
}

void to_conjunts(aalta_formula *af, set<aalta_formula *> &conjuncts)
{
    if (af == NULL)
        return;
    if (af->oper() == aalta_formula::And)
    {
        to_conjunts(af->l_af(), conjuncts);
        to_conjunts(af->r_af(), conjuncts);
    }
    else
        conjuncts.insert(af);
}

void to_disjunts(aalta_formula *af, set<aalta_formula *> &disjunts)
{
    if (af == NULL)
        return;
    if (af->oper() == aalta_formula::Or)
    {
        to_disjunts(af->l_af(), disjunts);
        to_disjunts(af->r_af(), disjunts);
    }
    else
        disjunts.insert(af);
}

// void fill_in_Y_edgeset(std::unordered_set<int> &partial_edgeset)
// {
//     for (auto it : Y_vars)
//     {
//         if (partial_edgeset.find(it) == partial_edgeset.end() && partial_edgeset.find(-(it)) == partial_edgeset.end())
//         {
//             partial_edgeset.insert(it);
//         }
//     }
// }

// aalta_formula *eliminateY(aalta_formula *af, const unordered_set<int> &Y)
// {
//     if (af == NULL)
//         return NULL;
//     auto op = af->oper();
//     if (op >= 11)
//     {
//         if (X_vars.find(op) != X_vars.end()) // x-variable
//             return af;
//         if (Y.find(-op) != Y.end())
//             return aalta_formula::FALSE();
//         else // two cases: op in Y, or op not in Y
//             return aalta_formula::TRUE();
//     }
//     else if (op == aalta_formula::Not)
//     {
//         auto rop = (af->r_af())->oper();
//         if (X_vars.find(rop) != X_vars.end()) // x-variable
//             return af;
//         if (Y.find(-rop) != Y.end())
//             return aalta_formula::TRUE();
//         else // two cases: rop in Y, or rop not in Y
//             return aalta_formula::FALSE();
//     }
//     switch (op)
//     {
//     case aalta_formula::True:
//     case aalta_formula::False:
//     case aalta_formula::Next:
//     case aalta_formula::WNext:
//     case aalta_formula::Until:
//     case aalta_formula::Release:
//         return af;
//     case aalta_formula::And:
//     {
//         aalta_formula *p_l = eliminateY(af->l_af(), Y);
//         if (p_l == aalta_formula::FALSE())
//             return aalta_formula::FALSE();
//         aalta_formula *p_r = eliminateY(af->r_af(), Y);
//         if (p_r == aalta_formula::FALSE())
//             return aalta_formula::FALSE();
//         if (p_l == aalta_formula::TRUE())
//             return p_r;
//         if (p_r == aalta_formula::TRUE())
//             return p_l;
//         return aalta_formula(aalta_formula::And, p_l, p_r).unique();
//     }
//     case aalta_formula::Or:
//     {
//         aalta_formula *p_l = eliminateY(af->l_af(), Y);
//         if (p_l == aalta_formula::TRUE())
//             return aalta_formula::TRUE();
//         aalta_formula *p_r = eliminateY(af->r_af(), Y);
//         if (p_r == aalta_formula::TRUE())
//             return aalta_formula::TRUE();
//         if (p_l == aalta_formula::FALSE())
//             return p_r;
//         if (p_r == aalta_formula::FALSE())
//             return p_l;
//         return aalta_formula(aalta_formula::Or, p_l, p_r).unique();
//     }
//     default:
//         assert(false);
//     }
// }

vector<char> *af2binaryString(aalta_formula *af)
{
    // -11 -1(TAIL)
    unordered_set<int> edgeset;
    af->to_set(edgeset);
    string bin_edge(total_var_nums, 'X');
    for (auto it : edgeset)
    {
        int var_id = abs(it);
        assert(bin_edge[var_id - 12] == 'X');
        bin_edge[var_id - 12] = it > 0 ? '1' : '0';
    }
    vector<char> *bin_edge_ch_vec = new vector<char>(bin_edge.c_str(), bin_edge.c_str() + bin_edge.size() + 1);
    return bin_edge_ch_vec;
}
