#include "ReadFile.h"

#include <fstream>

using std::vector;

namespace FileToCpp
{
// GCC throw at runtime with a std::bad_cast for some unknown reason
#if defined(_MSC_VER )
std::vector<unsigned char> read_file( std::filesystem::path const& filename )
{
    std::basic_ifstream<unsigned char> ifs { filename.c_str( ), std::ios::binary };

    if( ifs.good( ) == false )
    {
        throw std::runtime_error( "Failed to open file " + filename.string( ) );
    }

    std::vector<unsigned char> data( ( std::istreambuf_iterator<unsigned char>( ifs ) ), std::istreambuf_iterator<unsigned char>( ) );

    return data;
}
#else
std::vector<unsigned char> read_file( std::filesystem::path const& filename )
{
    std::basic_ifstream<char> ifs { filename.c_str( ), std::ios::binary };

    if( ifs.good( ) == false )
    {
        throw std::runtime_error( "Failed to open file " + filename.string( ) );
    }

    std::vector<unsigned char> data;
    while(ifs.peek() != EOF)
    {
       data.push_back( static_cast<unsigned char>( ifs.get() ) );
    }

    return data;
}
#endif

}
