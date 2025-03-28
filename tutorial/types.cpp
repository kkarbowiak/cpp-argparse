#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("pos").type<int>().choices({1, 2, 3});
    parser.add_argument("-c").type<double>().const_(3.14).action(argparse::store_const);
    parser.add_argument("-d").type<int>().default_(7);
    auto parsed = parser.parse_args(argc, argv);
    std::cout << "pos:\t" << parsed.get_value<int>("pos") << '\n';
    std::cout << "c:\t" << (parsed.get("c") ? parsed.get_value<double>("c") : 0) << '\n';
    std::cout << "d:\t" << parsed.get_value<int>("d") << '\n';
}
