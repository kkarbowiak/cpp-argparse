#include <doctest.h>

#include "argparse.h"

#include <string>
#include <list>


TEST_CASE("Optional argument consumes only single command-line arg when its action is store_true")
{
    auto parser = argparse::ArgumentParser();
    auto & optional = parser.add_argument("-a").action(argparse::store_true);

    CHECK(optional.parse_args({"prog", "pos1", "-a", "pos2"}) == std::list<std::string>{"prog", "pos1", "pos2"});
}

TEST_CASE("Optional argument consumes only single command-line arg when its action is store_false")
{
    auto parser = argparse::ArgumentParser();
    auto & optional = parser.add_argument("-a").action(argparse::store_false);

    CHECK(optional.parse_args({"prog", "pos1", "-a", "pos2"}) == std::list<std::string>{"prog", "pos1", "pos2"});
}
