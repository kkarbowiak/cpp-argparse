#include "argparse.h"
#include <iostream>

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("square").help("display a square of a given number").type<int>();
    auto parsed = parser.parse_args(argc, argv);
    auto value = parsed.get_value<int>("square");
    std::cout << value * value << '\n';
}
