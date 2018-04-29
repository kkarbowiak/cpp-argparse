#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include "argparse.h"


TEST_CASE("Checking existence of ArgumentParser class")
{
    argparse::ArgumentParser();
}

TEST_CASE("Checking existence of ArgumentParser::add_argument function")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("dummy");
}

TEST_CASE("Checking existence of ArgumentParser::parse_args function")
{
    auto parser = argparse::ArgumentParser();
    parser.parse_args(0, nullptr);
}

TEST_CASE("Checking parsing single positional argument")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("pos1");
    
    auto const parsed = parser.parse_args({"value1"});

    CHECK(parsed.at("pos1") == "value1");
}

TEST_CASE("Checking reporting unrecognised argument")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("pos1");
    
    CHECK_THROWS(parser.parse_args({"value1", "value2"}));
}
