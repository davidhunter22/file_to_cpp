#include <CLI/CLI.hpp>

// import std;

#include <filesystem>
#include <span>

using std::vector;

char const* convert_to_hex_string( unsigned char );

std::vector<unsigned char> read_file( std::filesystem::path const& );

struct Configuration
{
    std::filesystem::path input_file_path;
    std::filesystem::path output_file_name;

    std::string output_variable_name;

    bool use_std_library_module = false;

    std::string namespace_name;

    bool output_header_file     = true;
    bool use_pragma_once        = true;

    std::string declspec_header;
    std::string declspec_macro;

    size_t line_break_every_n_values = 10;
};


void ouput_as_hex_strings( std::ofstream& ofs, Configuration const& configuration, std::span<unsigned char> data )
{
    size_t line_count { 0 };

    for( auto v : data )
    {
        ofs << convert_to_hex_string( v );

        // Output a new line every so often
        if( ++line_count == configuration.line_break_every_n_values )
        {
            ofs <<  "\n    ";
        }
    }
}

void output_open_namespace( std::ofstream& ofs, Configuration const& configuration )
{
    if( configuration.namespace_name.empty( ) == false )
    {
        ofs << "namespace " << configuration.namespace_name << "\n{\n";
    }
}

void output_close_namespace( std::ofstream& ofs, Configuration const& configuration )
{
    if( configuration.namespace_name.empty( ) == false )
    {
        ofs << "}\n";
    }
}

void output_cpp_file( Configuration const& configuration, std::span<unsigned char> data )
{
    std::ofstream ofs;

    ofs.open( configuration.output_file_name.string( ) + ".cpp" );

    if( ofs.good( ) == false )
    {
        throw std::runtime_error( "Opening of output file produced an error" );
    }

    if( configuration.use_std_library_module == true )
    {
        ofs << "import std;\n\n";
    }
    else
    {
        ofs << "#include <span>\n";
        ofs << "#include <vector>\n\n";
    }

    output_open_namespace( ofs, configuration );

    // This is the actual data written out as hexadecimal test values into an array
    ofs << "std::vector<unsigned char> " << configuration.output_variable_name << "\n";
    ofs << "{\n    ";
    ouput_as_hex_strings( ofs, configuration, data );
    ofs << "\n};\n" << std::endl;

    // Function to get the data as a span
    ofs << "std::span<unsigned char> get_" << configuration.output_variable_name << "( )\n";
    ofs << "{\n";
    ofs << "    return " << configuration.output_variable_name << ";\n";
    ofs << "}\n";

    output_close_namespace( ofs, configuration );
}



void output_header_file( Configuration const& configuration )
{
    if( configuration.output_header_file == false ) return;

    std::ofstream ofs;

    ofs.open( configuration.output_file_name.string( ) + ".h" );

    if( configuration.use_pragma_once == true )
    {
        ofs << "#pragma once\n\n";
    }

    if( configuration.declspec_header.empty( ) == false )
    {
        ofs << "#include \"" << configuration.declspec_header << "\"\n\n";
    }

    if( configuration.use_std_library_module == true )
    {
        ofs << "import std;\n\n";
    }
    else
    {
        ofs << "#include <span>\n\n";
    }

    output_open_namespace( ofs, configuration );

    if( configuration.declspec_macro.empty( ) == false )
    {
        ofs << configuration.declspec_macro << " ";
    }

    ofs << "std::span<unsigned char> get_" << configuration.output_variable_name << "( );\n";

    output_close_namespace( ofs, configuration );
}

Configuration create_configuration_from_command_line_arguments( int argc, char** argv )
{
    CLI::App app { "file_to_cpp, convert a file into a cpp file, basically like xxd or #embed" };

    Configuration configuration;

    app.add_option( "-i,--input",        configuration.input_file_path,        "Input file path" )->required( );
    app.add_option( "-o,--output",       configuration.output_file_name,       "Output file name" );
    app.add_option( "-v,--variable",     configuration.output_variable_name,   "Output variable name" );
    app.add_option( "--namespace",       configuration.namespace_name,         "Namespace to wrap all the code in" );
    app.add_flag  ( "--use_std_module",  configuration.use_std_library_module, "Use std library module rath than #includes" );
    app.add_flag  ( "--output_header",   configuration.output_header_file,     "Output a header file" );
    app.add_option( "--declspec_macro",  configuration.declspec_macro,         "Macro containing declspec(export) or import" );
    app.add_option( "--declspec_header", configuration.declspec_header,        "Header containing definition of declspec macro, this is option as the macro may be defined on the command line" );

    try
    {
        app.parse( argc, argv );

        // The rest of this sets up reasonable defaults for things that the caller did not set

        // If the output name is not set we use the stem of the input name
        if( configuration.output_file_name.empty( ) )
        {
            configuration.output_file_name = configuration.input_file_path.stem( );
        }

        if( configuration.output_variable_name.empty( ) )
        {
            configuration.output_variable_name = configuration.input_file_path.stem( ).string( );

            std::ranges::replace( configuration.output_variable_name, '.', '_');
        }
    }
    catch( CLI::ParseError const& exception )
    {
        app.exit( exception );

        throw;
    }

    return configuration;
}

int main( int argc, char** argv )
{
    int return_code = 0;


    try
    {
        Configuration configuration { create_configuration_from_command_line_arguments( argc, argv ) };

        // This is the actual byte data contained in the input file.
        vector<unsigned char> data { read_file( configuration.input_file_path ) };

        output_cpp_file   ( configuration, data );

        output_header_file( configuration );
    }
    catch( CLI::ParseError const& exception )
    {
        return_code = 1;
    }
    catch( std::exception const& exception )
    {
        std::cout << "file_to_cpp failed with error : " << exception.what( ) << std::endl;

        return_code = 1;
    }

    return return_code;
}



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

