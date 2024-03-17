#include <iostream>

#include "HelloWorld.h"

int main()
{
    auto data { Foo::get_stuff( ) };

    std::cout << "Data " << data.size( ) << " " << data.data( ) << std::endl;

    return 0;
}