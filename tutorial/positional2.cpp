#include "argparse.h"
#include <iostream>

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("square").help("display a square of a given number");
    auto parsed = parser.parse_args(argc, argv);
    auto value = parsed.get_value("square");
    std::cout << value * value << '\n';
}
