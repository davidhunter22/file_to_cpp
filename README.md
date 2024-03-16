# file_to_cpp
Convert a file to cpp code, so another xxd or #embed like thing 

The main difference is that the executable aims to do everything you need. This includes producing a header file which contains things like declspec macros.
There should be no need for post processing.
The program should be usable without CMake or other input files, other than the one you are trying use as input.

