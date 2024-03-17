#if defined(IMPORT_STD)
    import std;
#else
    #include <filesystem>
    #include <map>
    #include <ostream>
    #include <span>
    #include <string>
#endif

namespace FileToCpp
{
// This contain the cofiguration of the output cpp and header file
// It does not contain information about the paths to the input or output files
struct Configuration
{
    enum class Type
    {
        String = 1,
        VectorOfUnsignedChar
    };

    std::string output_variable_name;

    Type type = Type::String;

    bool use_std_library_module = false;

    std::string namespace_name;

    bool output_header_file     = true;
    bool use_pragma_once        = true;

    std::string declspec_header;
    std::string declspec_macro;

    size_t line_break_every_n_values = 10;
};

// The path is assumed to be complete, so it includes a ".cpp", or ".h" or whater extension is wanted
// The span contains the data to output in the cpp file, presumably this came from some source like a file you want to embed
void output_cpp   ( std::filesystem::path const&, Configuration const&, std::span<unsigned char> );
void output_header( std::filesystem::path const&, Configuration const& );

// These take a stream rather than a path, this is useful for testing as you can stream to something like a sts::stringstream
void output_cpp   ( std::ostream&, Configuration const&, std::span<unsigned char> );
void output_header( std::ostream&, Configuration const& );

}