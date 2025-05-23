#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-v", "--verbose").help("increase output verbosity").action(argparse::store_true);
    auto parsed = parser.parse_args(argc, argv);
    if (parsed.get_value<bool>("verbose"))
    {
        std::cout << "verbosity turned on\n";
    }
}
