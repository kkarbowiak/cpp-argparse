#include "argparse.hpp"
#include <iostream>
#include <cmath>

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("x").type<int>().help("the base");
    parser.add_argument("y").type<int>().help("the exponent");
    parser.add_argument("-v", "--verbosity").type<int>().default_(0);
    auto parsed = parser.parse_args(argc, argv);
    auto base = parsed.get_value<int>("x");
    auto exp = parsed.get_value<int>("y");
    auto answer = std::pow(base, exp);
    auto verbosity = parsed.get_value<int>("verbosity");
    if (verbosity >= 2)
    {
        std::cout << base << " to the power " << exp << " equals " << answer << '\n';
    }
    else if (verbosity >= 1)
    {
        std::cout << base << "^" << exp << " == " << answer << '\n';
    }
    else
    {
        std::cout << answer << '\n';
    }
}
