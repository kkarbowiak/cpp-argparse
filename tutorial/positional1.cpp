#include "argparse.h"
#include <iostream>

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("echo").help("echo the string you use here");
    auto parsed = parser.parse_args(argc, argv);
    std::cout << parsed.get_value("echo") << '\n';
}
