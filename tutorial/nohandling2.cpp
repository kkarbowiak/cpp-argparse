#include "argparse.h"
#include <iostream>

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::errors_and_help);
    parser.add_argument("--version").action(argparse::version);
    auto parsed = parser.parse_args(argc, argv);
    if (parsed.get_value<bool>("version"))
    {
        std::cout << "This is program version 1.0.0\n";
    }
    return 0;
}
