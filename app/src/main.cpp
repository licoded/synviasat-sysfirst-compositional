#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <unordered_set>

#include "formula/aalta_formula.h"
// #include "ltlfsat/carchecker.h"
#include "ltlfsyn/synthesis.h"
#include "synutil/preprocess.h"
// #include "synutil/syn_states.h"
#include "synthesis.h"
#include "util/getenv.h"

using namespace aalta;
using namespace std;

void usage() {}

int main(int argc, char **argv)
{
    if (argc != 4)
        usage();

    // read formula
    ifstream fin;
    fin.open(argv[1], ios::in);
    if (!fin.is_open())
    {
        cout << "cannot open file " << argv[1] << endl;
        return 0;
    }
    string input_f, tmp;
    unordered_set<string> env_var;
    getline(fin, input_f);
    fin.close();

    // read variables partition
    fin.open(argv[2], ios::in);
    if (!fin.is_open())
    {
        cout << "cannot open file " << argv[2] << endl;
        return 0;
    }
    fin >> tmp;
    while (fin >> tmp)
    {
        if (tmp[0] != '.')
            env_var.insert(tmp);
        else
            break;
    }
    fin.close();

    // rewrite formula
    aalta_formula *af;
    // set tail id to be 1
    af = aalta_formula::TAIL();
    aalta_formula::TRUE();
    aalta_formula::FALSE();
    af = aalta_formula(input_f.c_str(), true).nnf(); //->unique();
    // af = af->remove_wnext();
    af = af->simplify();
    // af = af->split_next();
    af = mySimplify(af);

    bool verbose = readflag_from_env("VERBOSE");
    SAT_TRACE_FLAG = readflag_from_env("SAT_TRACE");

    bool result = is_realizable(af, env_var, verbose);
    if (result)
        cout << "Realizable" << endl;
    else
        cout << "Unrealizable" << endl;
    // int state_cnt = syn_states::get_state_cnt();
    // state_cnt -= 2;
    // cout << "A total of " << (state_cnt == 0 ? 1 : state_cnt) << " traversed state(s)." << endl;
    // cout<<swin_state_bdd_vec.size()<<endl;
    // cout<<ewin_state_bdd_vec.size()<<endl;
    aalta_formula::destroy();
    FormulaInBdd::QuitBdd4LTLf();

    return 0;
}
