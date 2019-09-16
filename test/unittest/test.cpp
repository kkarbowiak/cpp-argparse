#include <doctest.h>

#include "argparse.h"

#include <string>


TEST_CASE("Parsing single positional argument yields its value")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("p1");
    
    auto const parsed = parser.parse_args({"v1"});

    CHECK(parsed.at("p1") == "v1");
}

TEST_CASE("Checking parsing single optional argument")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o1");

    SUBCASE("missing")
    {
        auto const parsed = parser.parse_args({});

        CHECK(parsed.at("o1") == std::nullopt);
    }

    SUBCASE("missing argument")
    {
        REQUIRE_THROWS_AS(parser.parse_args({"-o1"}), std::runtime_error);

        try
        {
            (void) parser.parse_args({"-o1"});
        }
        catch (std::exception const & e)
        {
            CHECK(std::string(e.what()) == std::string("argument -o1: expected one argument"));
        }
    }

    SUBCASE("present")
    {
        auto const parsed = parser.parse_args({"-o1", "v1"});

        CHECK(parsed.at("o1") == "v1");
    }
}

TEST_CASE("Checking reporting missing arguments")
{
    auto parser = argparse::ArgumentParser();

    SUBCASE("one missing argument")
    {
        parser.add_argument("p1");

        REQUIRE_THROWS_AS(parser.parse_args({}), std::runtime_error);

        try
        {
            (void) parser.parse_args({});
        }
        catch (std::exception const & e)
        {
            CHECK(std::string(e.what()) == std::string("missing arguments: p1"));
        }
    }

    SUBCASE("two missing arguments")
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
            CHECK(std::string(e.what()) == std::string("missing arguments: p1 p2"));
        }
    }

    SUBCASE("five missing arguments")
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
            CHECK(std::string(e.what()) == std::string("missing arguments: p1 p2 p3 p4 p5"));
        }
    }
}

TEST_CASE("Checking reporting unrecognised arguments")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("p1");
    
    SUBCASE("one unrecognised argument")
    {
        REQUIRE_THROWS_AS(parser.parse_args({"v1", "v2"}), std::runtime_error);

        try
        {
            (void) parser.parse_args({"v1", "v2"});
        }
        catch (std::exception const & e)
        {
            CHECK(std::string(e.what()) == std::string("unrecognised arguments: v2"));
        }
    }

    SUBCASE("two unrecognised arguments")
    {
        REQUIRE_THROWS_AS(parser.parse_args({"v1", "v2", "v3"}), std::runtime_error);

        try
        {
            (void) parser.parse_args({"v1", "v2", "v3"});
        }
        catch (std::exception const & e)
        {
            CHECK(std::string(e.what()) == std::string("unrecognised arguments: v2 v3"));
        }
    }

    SUBCASE("five unrecognised arguments")
    {
        REQUIRE_THROWS_AS(parser.parse_args({"v1", "v2", "v3", "v4", "v5", "v6"}), std::runtime_error);

        try
        {
            (void) parser.parse_args({"v1", "v2", "v3", "v4", "v5", "v6"});
        }
        catch (std::exception const & e)
        {
            CHECK(std::string(e.what()) == std::string("unrecognised arguments: v2 v3 v4 v5 v6"));
        }
    }
}
