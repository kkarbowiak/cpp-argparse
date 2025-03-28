#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("echo");
    auto parsed = parser.parse_args(argc, argv);
    std::cout << parsed.get_value("echo") << '\n';
}
