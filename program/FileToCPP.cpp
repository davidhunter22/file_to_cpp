#include <FileToCPP.h>
#include <ReadFile.h>

#include <CLI/CLI.hpp>

#if !defined(IMPORT_STD)
    #include <filesystem>
    #include <span>
#endif

using std::vector;

namespace FileToCpp
{
struct Program
{
    Program( int argc, char** argv ) :
        app { "file_to_cpp, convert a file into a cpp file, basically like xxd or #embed" }
    { 
        app.add_option( "-i,--input",        input_file,                          "Input file path" )->required( );
        app.add_option( "-o,--output",       output_file_base,                     "Output file name, this should not include a .cpp or .h extension" );
        app.add_option( "-f,--function",     configuration.output_function_name,   "Output function name to get the data" );
        app.add_option( "-t,--type",         configuration.type,                   "Type to use in output" )->transform( CLI::CheckedTransformer(TypeNameMapper, CLI::ignore_case ) );

        app.add_option( "--namespace",       configuration.namespace_name,         "Namespace to wrap all the code in" );
        app.add_flag  ( "--use_std_module",  configuration.use_std_library_module, "Use std library module rath than #includes" );

        // Header file only options
        app.add_flag  ( "--output_header",   configuration.output_header_file,     "Output a header file" );
        app.add_option( "--declspec_macro",  configuration.declspec_macro,         "Macro containing declspec(export) or import" );
        app.add_option( "--declspec_header", configuration.declspec_header,        "Header containing definition of declspec macro, this is option as the macro may be defined on the command line" );

        try
        {
            app.parse( argc, argv );

            // The rest of this sets up reasonable defaults for things that the caller did not set

            // If the output name is not set we use the stem of the input name
            if( output_file_base.empty( ) )
            {
                output_file_base = input_file.stem( );
            }

            output_file_cpp    = output_file_base.string( ) + ".cpp";
            output_file_header = output_file_base.string( ) + ".h";

            if( configuration.output_function_name.empty( ) )
            {
                // We only use the stem here so "/foo/bar.txt" just becomes "bar" 
                configuration.output_function_name = input_file.stem( ).string( );

                // We need to replace any charracters that can appear in a file name but not in a function name
                std::ranges::replace( configuration.output_function_name, '.', '_');
            }
        }
        catch( CLI::ParseError const& exception )
        {
            app.exit( exception );

            throw;
        }

        data = read_file( input_file );
    }

    void output_cpp( )
    {
        FileToCpp::output_cpp( output_file_cpp, configuration, data );
    }

    void output_header( )
    {
        FileToCpp::output_header( output_file_header, configuration );
    }

    inline static std::map<std::string, Configuration::Type> TypeNameMapper
    {
        { "vui",    Configuration::Type::VectorOfUnsignedChar },
        { "string", Configuration::Type::String}
    };

    CLI::App app;

    Configuration configuration;

    std::filesystem::path input_file;
    std::filesystem::path output_file_base;
    std::filesystem::path output_file_cpp;
    std::filesystem::path output_file_header;

    std::vector<unsigned char> data;
};
}

int main( int argc, char** argv )
{
    int return_code = 0;


    try
    {
        FileToCpp::Program program { argc, argv };

        program.output_cpp( );
        program.output_header( );
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
