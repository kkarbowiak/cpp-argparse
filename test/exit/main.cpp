#include "argparse.h"
#include <iostream>


auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("positional");
    parser.add_argument("--optional");
    parser.add_argument("-v", "--version").action(argparse::version).version("1.0.0");
    auto const args = parser.parse_args(argc, argv);
    std::cout << "positional: " << args.get_value("positional") << '\n';
    std::cout << "optional: " << (args.get("optional") ? args.get_value("optional") : "<none>") << '\n';
}
