#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("--verbosity").help("increase output verbosity");
    auto parsed = parser.parse_args(argc, argv);
    if (parsed.get("verbosity"))
    {
        std::cout << "verbosity turned on\n";
    }
}
