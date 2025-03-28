#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("square").help("display a square of a given number").type<int>();
    parser.add_argument("-v", "--verbosity").help("increase output verbosity").type<int>().choices({0, 1, 2});
    auto parsed = parser.parse_args(argc, argv);
    auto value = parsed.get_value<int>("square");
    auto answer = value * value;
    auto verbosity = parsed.get("verbosity");
    if (verbosity && verbosity.get<int>() == 2)
    {
        std::cout << "the square of " << value << " equals " << answer << '\n';
    }
    else if (verbosity && verbosity.get<int>() == 1)
    {
        std::cout << value << "^2 == " << answer << '\n';
    }
    else
    {
        std::cout << answer << '\n';
    }
}
