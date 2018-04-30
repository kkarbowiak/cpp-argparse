#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include "argparse.h"

#include <string>


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

TEST_CASE("Checking reporting missing arguments")
{
    auto parser = argparse::ArgumentParser();

    SUBCASE("one missing argument")
    {
        parser.add_argument("pos1");

        REQUIRE_THROWS_AS(parser.parse_args({}), std::runtime_error);

        try
        {
            (void) parser.parse_args({});
        }
        catch (std::exception const & e)
        {
            CHECK(std::string(e.what()) == std::string("missing arguments: pos1"));
        }
    }

    SUBCASE("two missing arguments")
    {
        parser.add_argument("pos1");
        parser.add_argument("pos2");
        
        REQUIRE_THROWS_AS(parser.parse_args({}), std::runtime_error);

        try
        {
            (void) parser.parse_args({});
        }
        catch (std::exception const & e)
        {
            CHECK(std::string(e.what()) == std::string("missing arguments: pos1 pos2"));
        }
    }

    SUBCASE("five missing arguments")
    {
        parser.add_argument("pos1");
        parser.add_argument("pos2");
        parser.add_argument("pos3");
        parser.add_argument("pos4");
        parser.add_argument("pos5");
        
        REQUIRE_THROWS_AS(parser.parse_args({}), std::runtime_error);

        try
        {
            (void) parser.parse_args({});
        }
        catch (std::exception const & e)
        {
            CHECK(std::string(e.what()) == std::string("missing arguments: pos1 pos2 pos3 pos4 pos5"));
        }
    }
}

TEST_CASE("Checking reporting unrecognised arguments")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("pos1");
    
    SUBCASE("one unrecognised argument")
    {
        REQUIRE_THROWS_AS(parser.parse_args({"value1", "value2"}), std::runtime_error);

        try
        {
            (void) parser.parse_args({"value1", "value2"});
        }
        catch (std::exception const & e)
        {
            CHECK(std::string(e.what()) == std::string("unrecognised arguments: value2"));
        }
    }

    SUBCASE("two unrecognised arguments")
    {
        REQUIRE_THROWS_AS(parser.parse_args({"value1", "value2", "value3"}), std::runtime_error);

        try
        {
            (void) parser.parse_args({"value1", "value2", "value3"});
        }
        catch (std::exception const & e)
        {
            CHECK(std::string(e.what()) == std::string("unrecognised arguments: value2 value3"));
        }
    }

    SUBCASE("five unrecognised arguments")
    {
        REQUIRE_THROWS_AS(parser.parse_args({"value1", "value2", "value3", "value4", "value5", "value6"}), std::runtime_error);

        try
        {
            (void) parser.parse_args({"value1", "value2", "value3", "value4", "value5", "value6"});
        }
        catch (std::exception const & e)
        {
            CHECK(std::string(e.what()) == std::string("unrecognised arguments: value2 value3 value4 value5 value6"));
        }
    }
}
