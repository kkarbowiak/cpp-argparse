#include "argparse.h"

#include <string>
#include <iostream>


using namespace std::string_literals;

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("arg1").help("argument");
    parser.add_argument("arg2").help("argument of int type").type<int>();
    parser.add_argument("arg3").help("argument that only accepts 'foo' or 'bar' values").choices({"foo"s, "bar"s});

    try
    {
        auto args = parser.parse_args(argc, argv);

        auto const arg1_value = args.get_value("arg1");
        auto const arg2_value = args.get_value<int>("arg2");
        auto const arg3_value = args.get_value("arg3");

        std::cout << "args:\n";
        std::cout << "  arg1: " << arg1_value << '\n';
        std::cout << "  arg2: " << arg2_value << '\n';
        std::cout << "  arg3: " << arg3_value << '\n';
    }
    catch (std::exception const & e)
    {
        std::cout << e.what() << '\n';
    }

    return 0;
}
