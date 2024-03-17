#include <iostream>

#include "HelloWorld.h"

int main()
{
    auto data { Foo::Bar::get_hello_world_data( ) };

    std::cout << "Data " << data.size( ) << " " << data << std::endl;

    return 0;
}