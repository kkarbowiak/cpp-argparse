#include "argparse.h"

#include <string>
#include <iostream>


int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("pos1").help("positional argument #1");
    parser.add_argument("pos2").help("positional argument #2");
    parser.add_argument("pos3").help("positional argument #3");
    parser.add_argument("-f").help("optional argument #1");
    parser.add_argument("-g").help("optional argument #2").store_true();
    parser.add_argument("-h").help("optional argument #3");
    parser.add_argument("--long-option").help("optional argument #4");
    parser.add_argument("--very-long-option").help("optional argument #5").store_true();

    try
    {
        auto args = parser.parse_args(argc, argv);

        std::cout << std::boolalpha << "args:\n";
        for (std::string a : {"pos1", "pos2", "pos3", "f", "g", "h", "long_option", "very_long_option"})
        {
            std::cout << a << ": ";
            if (auto v = args.get(a))
            {
                if (a == "g" || a == "very_long_option")
                {
                    std::cout << v.get<bool>();
                }
                else
                {
                    std::cout << v.get();
                }
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
        std::cout << e.what() << '\n';
        std::cout << parser.format_help();
        return 0;
    }

    return 0;
}
