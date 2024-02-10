#include <stdexcept>
#include <iostream>
#include "../compressor-lib/Interface.h"

int main(int argc, char const *argv[])
{
    try
    {
        Interface::run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    
    return 0;
}
