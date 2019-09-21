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

        try
        {
            (void) parser.parse_args({"-o"});
        }
        catch (std::exception const & e)
        {
            CHECK(std::string(e.what()) == "argument -o: expected one argument"s);
        }
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

TEST_CASE("Parsing missing positional argument throws an exception...")
{
    auto parser = argparse::ArgumentParser();

    SUBCASE("...for one missing argument")
    {
        parser.add_argument("p1");

        REQUIRE_THROWS_AS(parser.parse_args({}), std::runtime_error);

        try
        {
            (void) parser.parse_args({});
        }
        catch (std::exception const & e)
        {
            CHECK(std::string(e.what()) == "missing arguments: p1"s);
        }
    }

    SUBCASE("...for two missing arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("p2");
        
        REQUIRE_THROWS_AS(parser.parse_args({}), std::runtime_error);

        try
        {
            (void) parser.parse_args({});
        }
        catch (std::exception const & e)
        {
            CHECK(std::string(e.what()) == "missing arguments: p1 p2"s);
        }
    }

    SUBCASE("...for five missing arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("p2");
        parser.add_argument("p3");
        parser.add_argument("p4");
        parser.add_argument("p5");
        
        REQUIRE_THROWS_AS(parser.parse_args({}), std::runtime_error);

        try
        {
            (void) parser.parse_args({});
        }
        catch (std::exception const & e)
        {
            CHECK(std::string(e.what()) == "missing arguments: p1 p2 p3 p4 p5"s);
        }
    }
}

TEST_CASE("Parsing unrecognised argument throws an exception...")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("p1");
    
    SUBCASE("..for one unrecognised argument")
    {
        REQUIRE_THROWS_AS(parser.parse_args({"v1", "v2"}), std::runtime_error);

        try
        {
            (void) parser.parse_args({"v1", "v2"});
        }
        catch (std::exception const & e)
        {
            CHECK(std::string(e.what()) == "unrecognised arguments: v2"s);
        }
    }

    SUBCASE("...for two unrecognised arguments")
    {
        REQUIRE_THROWS_AS(parser.parse_args({"v1", "v2", "v3"}), std::runtime_error);

        try
        {
            (void) parser.parse_args({"v1", "v2", "v3"});
        }
        catch (std::exception const & e)
        {
            CHECK(std::string(e.what()) == "unrecognised arguments: v2 v3"s);
        }
    }

    SUBCASE("...for five unrecognised arguments")
    {
        REQUIRE_THROWS_AS(parser.parse_args({"v1", "v2", "v3", "v4", "v5", "v6"}), std::runtime_error);

        try
        {
            (void) parser.parse_args({"v1", "v2", "v3", "v4", "v5", "v6"});
        }
        catch (std::exception const & e)
        {
            CHECK(std::string(e.what()) == "unrecognised arguments: v2 v3 v4 v5 v6");
        }
    }
}
