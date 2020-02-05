#include "argparse.h"

#include "cstring_array.h"

#include <doctest.h>

#include <string>


using namespace std::string_literals;

TEST_CASE("Optional arguments support short names")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-s");

    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-s", "ess"});

    CHECK(parsed.get("s"));
}

TEST_CASE("Optional arguments support long names")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("--long-arg");

    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "--long-arg", "value"});

    CHECK(parsed.get("long_arg"));
}

TEST_CASE("ArgumentParser adds help argument automatically...")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    SUBCASE("...with short name")
    {
        CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-h"}));
    }

    SUBCASE("...with long name")
    {
        CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "--help"}));
    }
}

TEST_CASE("ArgumentParser adds help argument when requested...")
{
    auto parser = argparse::ArgumentParser().add_help(true).handle(argparse::Handle::none);

    SUBCASE("...with short name")
    {
        CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-h"}));
    }

    SUBCASE("...with long name")
    {
        CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "--help"}));
    }
}

TEST_CASE("ArgumentParser does not add help argument when requested not to")
{
    auto parser = argparse::ArgumentParser().add_help(false).handle(argparse::Handle::none);

    CHECK_THROWS(parser.parse_args(2, cstr_arr{"prog", "-h"}));
}

TEST_CASE("ArgumentParser does not handle help when requested to...")
{
    SUBCASE("...handle nothing")
    {
        auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

        auto const parsed = parser.parse_args(2, cstr_arr{"prog", "-h"});

        CHECK(parsed.get_value<bool>("help") == true);
    }

    SUBCASE("...handle parsing errors")
    {
        auto parser = argparse::ArgumentParser().handle(argparse::Handle::errors);

        auto const parsed = parser.parse_args(2, cstr_arr{"prog", "-h"});

        CHECK(parsed.get_value<bool>("help") == true);
    }
}

TEST_CASE("ArgumentParser does not handle parsing errors when requested to...")
{
    SUBCASE("...handle nothing")
    {
        auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
        parser.add_argument("pos");

        CHECK_THROWS_AS(parser.parse_args(1, cstr_arr{"prog"}), argparse::parsing_error);
    }

    SUBCASE("...handle help")
    {
        auto parser = argparse::ArgumentParser().handle(argparse::Handle::help);
        parser.add_argument("pos");

        CHECK_THROWS_AS(parser.parse_args(1, cstr_arr{"prog"}), argparse::parsing_error);
    }
}

TEST_CASE("ArgumentParser uses prog parameter as its name...")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);

    SUBCASE("...in usage message")
    {
        CHECK(parser.format_usage() == "usage: prog"s);
    }

    SUBCASE("...in help message")
    {
        CHECK(parser.format_help() == "usage: prog"s);
    }
}

TEST_CASE("Adding a positional argument with required option set results in error")
{
    auto parser = argparse::ArgumentParser();

    CHECK_THROWS_WITH(parser.add_argument("pos").required(true), "'required' is an invalid argument for positionals");
    CHECK_THROWS_WITH(parser.add_argument("pos").required(false), "'required' is an invalid argument for positionals");
}

TEST_CASE("Adding an optional argument with required option does not result in error")
{
    auto parser = argparse::ArgumentParser();

    CHECK_NOTHROW(parser.add_argument("-o").required(true));
}
