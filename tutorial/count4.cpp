#include "argparse.hpp"
#include <iostream>

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("square").help("display a square of a given number").type<int>();
    parser.add_argument("-v", "--verbosity").help("increase output verbosity").action(argparse::count).default_(0);
    auto parsed = parser.parse_args(argc, argv);
    auto value = parsed.get_value<int>("square");
    auto answer = value * value;
    auto verbosity = parsed.get_value<int>("verbosity");
    if (verbosity >= 2)
    {
        std::cout << "the square of " << value << " equals " << answer << '\n';
    }
    else if (verbosity >= 1)
    {
        std::cout << value << "^2 == " << answer << '\n';
    }
    else
    {
        std::cout << answer << '\n';
    }
}
