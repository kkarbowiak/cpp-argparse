#include <doctest.h>

#include "argparse.h"

#include <string>


using namespace std::string_literals;

TEST_CASE("Parsing single positional argument yields its value")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("p1");
    
    auto const parsed = parser.parse_args({"v1"});

    CHECK(parsed.at("p1") == "v1");
}

TEST_CASE("Parsing single optional argument...")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o");

    SUBCASE("...yields nullopt when it's missing")
    {
        auto const parsed = parser.parse_args({});

        CHECK(parsed.at("o") == std::nullopt);
    }

    SUBCASE("...throws an exception when it's missing argument")
    {
        REQUIRE_THROWS_AS(parser.parse_args({"-o"}), std::runtime_error);
        CHECK_THROWS_WITH(parser.parse_args({"-o"}), "argument -o: expected one argument");
    }

    SUBCASE("...yields its value")
    {
        auto const parsed = parser.parse_args({"-o", "v1"});

        CHECK(parsed.at("o") == "v1");
    }
}

TEST_CASE("Optional arguments support short names")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-s");

    auto const parsed = parser.parse_args({"-s", "ess"});

    CHECK(parsed.at("s"));
}

TEST_CASE("Optional arguments support long names")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("--long-arg");

    auto const parsed = parser.parse_args({"--long-arg", "value"});

    CHECK(parsed.at("long_arg"));
}

TEST_CASE("Parsing missing positional argument throws an exception...")
{
    auto parser = argparse::ArgumentParser();

    SUBCASE("...for one missing argument")
    {
        parser.add_argument("p1");

        REQUIRE_THROWS_AS(parser.parse_args({}), std::runtime_error);
        CHECK_THROWS_WITH(parser.parse_args({}), "missing arguments: p1");
    }

    SUBCASE("...for two missing arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("p2");
        
        REQUIRE_THROWS_AS(parser.parse_args({}), std::runtime_error);
        CHECK_THROWS_WITH(parser.parse_args({}), "missing arguments: p1 p2");
    }

    SUBCASE("...for five missing arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("p2");
        parser.add_argument("p3");
        parser.add_argument("p4");
        parser.add_argument("p5");
        
        REQUIRE_THROWS_AS(parser.parse_args({}), std::runtime_error);
        CHECK_THROWS_WITH(parser.parse_args({}), "missing arguments: p1 p2 p3 p4 p5");
    }
}

TEST_CASE("Parsing unrecognised positional argument throws an exception...")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("p1");
    
    SUBCASE("...for one unrecognised argument")
    {
        REQUIRE_THROWS_AS(parser.parse_args({"v1", "v2"}), std::runtime_error);
        CHECK_THROWS_WITH(parser.parse_args({"v1", "v2"}), "unrecognised arguments: v2");
    }

    SUBCASE("...for two unrecognised arguments")
    {
        REQUIRE_THROWS_AS(parser.parse_args({"v1", "v2", "v3"}), std::runtime_error);
        CHECK_THROWS_WITH(parser.parse_args({"v1", "v2", "v3"}), "unrecognised arguments: v2 v3");
    }

    SUBCASE("...for five unrecognised arguments")
    {
        REQUIRE_THROWS_AS(parser.parse_args({"v1", "v2", "v3", "v4", "v5", "v6"}), std::runtime_error);
        CHECK_THROWS_WITH(parser.parse_args({"v1", "v2", "v3", "v4", "v5", "v6"}), "unrecognised arguments: v2 v3 v4 v5 v6");
    }
}

TEST_CASE("Parsing unrecognised optional argument throws an exception...")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-a");

    SUBCASE("...for one unrecognised argument")
    {
        REQUIRE_THROWS_AS(parser.parse_args({"-a", "v1", "-b"}), std::runtime_error);
        CHECK_THROWS_WITH(parser.parse_args({"-a", "v1", "-b"}), "unrecognised arguments: -b");
    }

    SUBCASE("...for two unrecognised arguments")
    {
        REQUIRE_THROWS_AS(parser.parse_args({"-a", "v1", "-b", "-c"}), std::runtime_error);
        CHECK_THROWS_WITH(parser.parse_args({"-a", "v1", "-b", "-c"}), "unrecognised arguments: -b -c");
    }

    SUBCASE("...for five unrecognised arguments")
    {
        REQUIRE_THROWS_AS(parser.parse_args({"-a", "v1", "-b", "-c", "-d", "-e", "-f"}), std::runtime_error);
        CHECK_THROWS_WITH(parser.parse_args({"-a", "v1", "-b", "-c", "-d", "-e", "-f"}), "unrecognised arguments: -b -c -d -e -f");
    }
}
