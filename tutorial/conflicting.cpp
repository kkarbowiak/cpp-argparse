#include "argparse.h"
#include <iostream>
#include <cmath>

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser();
    auto group = parser.add_mutually_exclusive_group();
    group.add_argument("-v", "--verbose").action(argparse::store_true);
    group.add_argument("-q", "--quiet").action(argparse::store_true);
    parser.add_argument("x").type<int>().help("the base");
    parser.add_argument("y").type<int>().help("the exponent");
    auto parsed = parser.parse_args(argc, argv);
    auto base = parsed.get_value<int>("x");
    auto exp = parsed.get_value<int>("y");
    auto answer = std::pow(base, exp);
    if (parsed.get_value<bool>("quiet"))
    {
        std::cout << answer << '\n';
    }
    else if (parsed.get_value<bool>("verbose"))
    {
        std::cout << base << " to the power " << exp << " equals " << answer << '\n';
    }
    else
    {
        std::cout << base << "^" << exp << " == " << answer << '\n';
    }
}
