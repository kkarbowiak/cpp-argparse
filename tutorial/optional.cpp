#include "argparse.h"
#include <iostream>

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("--verbosity").help("increase output verbosity");
    auto parsed = parser.parse_args(argc, argv);
    if (parsed.get("verbosity"))
    {
        std::cout << "verbosity turned on\n";
    }
}
