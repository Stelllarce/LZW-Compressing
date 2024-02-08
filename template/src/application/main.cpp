#include <stdexcept>
#include <iostream>
#include "../compressor-lib/Interface.h"

int main(int argc, char const *argv[])
{
    Interface interface;
    try
    {
        interface.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    
    return 0;
}
