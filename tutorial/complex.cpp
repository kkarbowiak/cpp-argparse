#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("square").help("display a square of a given number").type<int>();
    parser.add_argument("-v", "--verbose").help("increase output verbosity").action(argparse::store_true);
    auto parsed = parser.parse_args(argc, argv);
    auto value = parsed.get_value<int>("square");
    auto answer = value * value;
    if (parsed.get_value<bool>("verbose"))
    {
        std::cout << "the square of " << value << " equals " << answer << '\n';
    }
    else
    {
        std::cout << answer << '\n';
    }
}
