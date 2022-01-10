#include "argparse.h"
#include <string>
#include <iostream>

int main(int argc, char * argv[])
{
    using namespace std::string_literals;

    auto parser = argparse::ArgumentParser();
    parser.add_argument("name").type<std::string>().choices({"John"s, "Lukas"s, "Gregory"s});
    auto parsed = parser.parse_args(argc, argv);
    std::cout << "Hello, " << parsed.get_value("name") << "!\n";
}
