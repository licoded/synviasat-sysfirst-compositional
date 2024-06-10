#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
#ifdef DEBUG
    std::cout << "With DEBUG Defined" << std::endl;
#endif
    std::cout << "Here is main.cpp" << std::endl;
    return 0;
}
