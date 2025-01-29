#include "argparse.h"

#include "doctest.h"

#include <string>


using namespace std::string_literals;

TEST_CASE("ArgumentParser provides program version")
{
    auto parser = argparse::ArgumentParser();

    parser.add_argument("-v").action(argparse::version).version("0.0.1");

    CHECK(parser.format_version() == "0.0.1"s);
}
