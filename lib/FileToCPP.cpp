#include "FileToCPP.h"

#include "ReadFile.h"

#if !defined(IMPORT_STD)
    #include <fstream>
#endif

namespace FileToCpp
{
namespace
{
// Maybe there is a faster way, ideally some sort of SIMD thing
// This is not thread safe
char const* convert_to_hex_string( unsigned char value )
{
    constexpr char hex_digits[] {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

    // We use this buffer, all hex representation are 4 byte, i.e 0xB4, plus a comma and a null terminator, so 6 characters
    static char hex[ 6 ] { '0', 'x', ' ', ' ', ',', 0  };

    hex[ 2 ] = hex_digits[ ( value & 0xF0) >> 4 ];
    hex[ 3 ] = hex_digits[ value & 0x0F ];

    return hex;
}

void ouput_as_hex_strings( std::ostream& os, Configuration const& configuration, std::span<unsigned char> data )
{
    size_t line_count { 0 };

    for( auto v : data )
    {
        os << convert_to_hex_string( v );

        // Output a new line every so often
        if( ++line_count == configuration.line_break_every_n_values )
        {
            os <<  "\n    ";
        }
    }
}

void output_open_namespace( std::ostream& os, Configuration const& configuration )
{
    if( configuration.namespace_name.empty( ) == false )
    {
        os << "namespace " << configuration.namespace_name << "\n{\n";
    }
}

void output_close_namespace( std::ostream& os, Configuration const& configuration )
{
    if( configuration.namespace_name.empty( ) == false )
    {
        os << "}\n";
    }
}

std::string type_name( Configuration const& configuration )
{
    switch( configuration.type )
    {
    case Configuration::Type::String :               return "std::string";
    case Configuration::Type::VectorOfUnsignedChar : return "std::vector<unsigned char>";
    }

    return "";
}
}

void output_cpp( std::filesystem::path const& filename, Configuration const& configuration, std::span<unsigned char> data )
{
    std::ofstream ofs;

    ofs.open( filename.string( ) );

    if( ofs.good( ) == false )
    {
        throw std::runtime_error( "Opening of output file produced an error" );
    }

    output_cpp( ofs, configuration, data );
}

void output_cpp( std::ostream& os, Configuration const& configuration, std::span<unsigned char> data )
{
    if( configuration.use_std_library_module == true )
    {
        os << "import std;\n\n";
    }
    else
    {
        os << "#include <string>\n";
        os << "#include <vector>\n\n";
    }

    output_open_namespace( os, configuration );

    // This is the actual data written out as hexadecimal test values into an array
    os << type_name( configuration ) << " " << configuration.output_variable_name << "\n";
    os << "{\n    ";
    ouput_as_hex_strings( os, configuration, data );
    os << "\n};\n" << std::endl;

    // Function to get the data
    os << type_name( configuration ) << " const& get_" << configuration.output_variable_name << "( )\n";
    os << "{\n";
    os << "    return " << configuration.output_variable_name << ";\n";
    os << "}\n";

    output_close_namespace( os, configuration );
}


void output_header( std::filesystem::path const& filename, Configuration const& configuration )
{
    std::ofstream ofs;

    ofs.open( filename.string( ) );

    if( ofs.good( ) == false )
    {
        throw std::runtime_error( "Opening of output file produced an error" );
    }

    output_header( ofs, configuration );
}

void output_header( std::ostream& os, Configuration const& configuration )
{
    if( configuration.output_header_file == false ) return;

    if( configuration.use_pragma_once == true )
    {
        os << "#pragma once\n\n";
    }

    if( configuration.declspec_header.empty( ) == false )
    {
        os << "#include \"" << configuration.declspec_header << "\"\n\n";
    }

    if( configuration.use_std_library_module == true )
    {
        os << "import std;\n\n";
    }
    else
    {
        os << "#include <string>\n";
        os << "#include <vector>\n\n";
    }

    output_open_namespace( os, configuration );

    if( configuration.declspec_macro.empty( ) == false )
    {
        os << configuration.declspec_macro << " ";
    }

    os << type_name( configuration ) << " const& get_" << configuration.output_variable_name << "( );\n";

    output_close_namespace( os, configuration );
}
}
