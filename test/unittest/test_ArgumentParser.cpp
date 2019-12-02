#include "argparse.h"

#include <doctest.h>

#include <string>


using namespace std::string_literals;

TEST_CASE("Parser creates a positional argument when adding an argument with unprefixed name")
{
    auto parser = argparse::ArgumentParser();

    auto const & argument = parser.add_argument("p1");

    CHECK(argument.is_positional());
}

TEST_CASE("Parser creates an optional argument when adding an argument with prefixed...")
{
    auto parser = argparse::ArgumentParser();

    SUBCASE("...short name")
    {
        auto const & argument = parser.add_argument("-p");

        CHECK(!argument.is_positional());
    }

    SUBCASE("...long name")
    {
        auto const & argument = parser.add_argument("--long-name");

        CHECK(!argument.is_positional());
    }

    SUBCASE("...short and long name")
    {
        auto const & argument = parser.add_argument("-l", "--long-name");

        CHECK(!argument.is_positional());
    }
}

TEST_CASE("Parsing single positional argument yields its value")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("p1");
    
    auto const parsed = parser.parse_args({"v1"});

    CHECK(parsed.get_value("p1") == "v1");
}

TEST_CASE("Parsing single optional argument...")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o");

    SUBCASE("...yields false when it's missing")
    {
        auto const parsed = parser.parse_args({});

        CHECK(!parsed.get("o"));
    }

    SUBCASE("...throws an exception when it's missing argument")
    {
        CHECK_THROWS_WITH_AS(parser.parse_args({"-o"}), "argument -o: expected one argument", std::runtime_error);
    }

    SUBCASE("...yields its value")
    {
        auto const parsed = parser.parse_args({"-o", "v1"});

        CHECK(parsed.get_value("o") == "v1");
    }
}

TEST_CASE("Parsing single optional argument with store true action...")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o").action(argparse::store_true);

    SUBCASE("...yields false when it's missing")
    {
        auto const parsed = parser.parse_args({});

        CHECK(parsed.get_value<bool>("o") == false);
    }

    SUBCASE("...yields true when it's present")
    {
        auto const parsed = parser.parse_args({"-o"});

        CHECK(parsed.get_value<bool>("o") == true);
    }
}

TEST_CASE("Parsing single optional argument with store false action...")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o").action(argparse::store_false);

    SUBCASE("...yields true when it's missing")
    {
        auto const parsed = parser.parse_args({});

        CHECK(parsed.get_value<bool>("o") == true);
    }

    SUBCASE("...yields true when it's present")
    {
        auto const parsed = parser.parse_args({"-o"});

        CHECK(parsed.get_value<bool>("o") == false);
    }
}

TEST_CASE("Parsing single optional argument with help action...")
{
    auto parser = argparse::ArgumentParser().add_help(false);
    parser.add_argument("-h").action(argparse::help);

    SUBCASE("...yields false when it's missing")
    {
        auto const parsed = parser.parse_args({});

        CHECK(parsed.get_value<bool>("h") == false);
    }

    SUBCASE("...yields true when it's present")
    {
        auto const parsed = parser.parse_args({"-h"});

        CHECK(parsed.get_value<bool>("h") == true);
    }
}

TEST_CASE("Optional arguments support short names")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-s");

    auto const parsed = parser.parse_args({"-s", "ess"});

    CHECK(parsed.get("s"));
}

TEST_CASE("Optional arguments support long names")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("--long-arg");

    auto const parsed = parser.parse_args({"--long-arg", "value"});

    CHECK(parsed.get("long_arg"));
}

TEST_CASE("Parsing missing positional argument throws an exception...")
{
    auto parser = argparse::ArgumentParser();

    SUBCASE("...for one missing argument")
    {
        parser.add_argument("p1");

        CHECK_THROWS_WITH_AS(parser.parse_args({}), "missing arguments: p1", std::runtime_error);
    }

    SUBCASE("...for two missing arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("p2");
        
        CHECK_THROWS_WITH_AS(parser.parse_args({}), "missing arguments: p1 p2", std::runtime_error);
    }

    SUBCASE("...for five missing arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("p2");
        parser.add_argument("p3");
        parser.add_argument("p4");
        parser.add_argument("p5");
        
        CHECK_THROWS_WITH_AS(parser.parse_args({}), "missing arguments: p1 p2 p3 p4 p5", std::runtime_error);
    }
}

TEST_CASE("Parsing arguments with help requested disregards parsing errors...")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-h").action(argparse::help);

    SUBCASE("...for missing positional argument")
    {
        parser.add_argument("p1");

        CHECK_NOTHROW(parser.parse_args({"-h"}));
    }

    SUBCASE("...for unrecognised positional argument")
    {
        CHECK_NOTHROW(parser.parse_args({"p1", "-h"}));
    }

    SUBCASE("...for unrecognised optional argument")
    {
        CHECK_NOTHROW(parser.parse_args({"-a", "-h"}));
    }
}

TEST_CASE("Parsing unrecognised positional argument throws an exception...")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("p1");
    
    SUBCASE("...for one unrecognised argument")
    {
        CHECK_THROWS_WITH_AS(parser.parse_args({"v1", "v2"}), "unrecognised arguments: v2", std::runtime_error);
    }

    SUBCASE("...for two unrecognised arguments")
    {
        CHECK_THROWS_WITH_AS(parser.parse_args({"v1", "v2", "v3"}), "unrecognised arguments: v2 v3", std::runtime_error);
    }

    SUBCASE("...for five unrecognised arguments")
    {
        CHECK_THROWS_WITH_AS(parser.parse_args({"v1", "v2", "v3", "v4", "v5", "v6"}), "unrecognised arguments: v2 v3 v4 v5 v6", std::runtime_error);
    }
}

TEST_CASE("Parsing unrecognised optional argument throws an exception...")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-a");

    SUBCASE("...for one unrecognised argument")
    {
        CHECK_THROWS_WITH_AS(parser.parse_args({"-a", "v1", "-b"}), "unrecognised arguments: -b", std::runtime_error);
    }

    SUBCASE("...for two unrecognised arguments")
    {
        CHECK_THROWS_WITH_AS(parser.parse_args({"-a", "v1", "-b", "-c"}), "unrecognised arguments: -b -c", std::runtime_error);
    }

    SUBCASE("...for five unrecognised arguments")
    {
        CHECK_THROWS_WITH_AS(parser.parse_args({"-a", "v1", "-b", "-c", "-d", "-e", "-f"}), "unrecognised arguments: -b -c -d -e -f", std::runtime_error);
    }
}

TEST_CASE("Parsing mixed positional and optional arguments give same result no matter the order...")
{
    auto parser1 = argparse::ArgumentParser();
    parser1.add_argument("pos1");
    parser1.add_argument("-f");

    auto parser2 = argparse::ArgumentParser();
    parser2.add_argument("-f");
    parser2.add_argument("pos1");

    SUBCASE("...for positional and optional")
    {
        char const * argv[] = {"prog", "val1", "-f", "a"};
        auto const parsed1 = parser1.parse_args(4, argv);
        auto const parsed2 = parser2.parse_args(4, argv);

        CHECK(parsed1.get_value("pos1") == parsed2.get_value("pos1"));
        CHECK(parsed1.get_value("f") == parsed2.get_value("f"));
    }

    SUBCASE("...for optional and positional")
    {
        char const * argv[] = {"prog", "-f", "a", "val1"};
        auto const parsed1 = parser1.parse_args(4, argv);
        auto const parsed2 = parser2.parse_args(4, argv);

        CHECK(parsed1.get_value("pos1") == parsed2.get_value("pos1"));
        CHECK(parsed1.get_value("f") == parsed2.get_value("f"));
    }
}

TEST_CASE("ArgumentParser adds help argument automatically")
{
    auto parser = argparse::ArgumentParser();

    CHECK_NOTHROW(parser.parse_args({"-h"}));
}

TEST_CASE("ArgumentParser adds help argument when requested")
{
    auto parser = argparse::ArgumentParser().add_help(true);

    CHECK_NOTHROW(parser.parse_args({"-h"}));
}

TEST_CASE("ArgumentParser does not add help argument when requested not to")
{
    auto parser = argparse::ArgumentParser().add_help(false);

    CHECK_THROWS(parser.parse_args({"-h"}));
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
