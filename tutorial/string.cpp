#include "argparse.hpp"
#include <string>
#include <iostream>

auto main(int argc, char * argv[]) -> int
{
    using namespace std::string_literals;

    auto parser = argparse::ArgumentParser();
    parser.add_argument("name").type<std::string>().choices({"John"s, "Lukas"s, "Gregory"s});
    auto parsed = parser.parse_args(argc, argv);
    std::cout << "Hello, " << parsed.get_value("name") << "!\n";
}
