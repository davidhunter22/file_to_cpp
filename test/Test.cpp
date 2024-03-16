#include <iostream>

#include "HelloWorld.h"

int main()
{
    auto data { get_HelloWorld( ) };

    std::cout << "Data " << data.size( ) << " " << data << std::endl;

    return 0;
}