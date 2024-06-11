#include <cstring>
#include <iostream>

bool readflag_from_env(const char *name, bool default_value = false)
{
    const char *flag_Str = getenv("SAT_TRACE");
    bool flag = default_value;
    if (flag_Str != NULL && strlen(flag_Str) > 0)
        flag = std::stoi(flag_Str);
    return flag;
}