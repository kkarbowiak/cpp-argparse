#include "argparse.h"

#include <iostream>


int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("pos1").help("positional argument #1");
    parser.add_argument("pos2").help("positional argument #2");
    parser.add_argument("pos3").help("positional argument #3");
    parser.add_argument("-f").help("optional argument #1");
    parser.add_argument("-g").help("optional argument #2");
    parser.add_argument("-h").help("optional argument #3");
    parser.add_argument("--long-option").help("optional argument #4");
    parser.add_argument("--very-long-option").help("optional argument #5");

    try
    {
        auto args = parser.parse_args(argc, argv);

        std::cout << "args:\n";
        for (auto & a : args)
        {
            std::cout << a.first << ": ";
            if (a.second)
            {
                std::cout << *a.second;
            }
            else
            {
                std::cout << "<?>";
            }
            std::cout << '\n';
        }
    }
    catch (std::exception const & e)
    {
        std::cout << parser.format_help();
        return 0;
    }

    return 0;
}
