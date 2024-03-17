#if defined(IMPORT_STD)
    import std;
#else
    #include <filesystem>
    #include <vector>
#endif

namespace FileToCpp
{
// Read an entire file into a sequence of bytes
// There are probably many better ways to do this so it's kept in a separate implementation file to make it easier to change
std::vector<unsigned char> read_file( std::filesystem::path const& );
}