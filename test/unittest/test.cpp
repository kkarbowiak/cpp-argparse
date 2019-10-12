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
        CHECK_THROWS_WITH_AS(parser.parse_args({"-o"}), "argument -o: expected one argument", std::runtime_error);
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

TEST_CASE("ArgumentParser provides usage message...")
{
    auto parser = argparse::ArgumentParser();

    SUBCASE("...for no arguments")
    {
        char const * args[] = {"prog"};
        (void) parser.parse_args(1, args);

        CHECK(parser.format_usage() == "usage: prog"s);
    }

    SUBCASE("...for one positional argument")
    {
        parser.add_argument("p1");

        char const * args[] = {"prog", "v1"};
        (void) parser.parse_args(2, args);

        CHECK(parser.format_usage() == "usage: prog p1"s);
    }

    SUBCASE("...for two positional arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("p2");

        char const * args[] = {"prog", "v1", "v2"};
        (void) parser.parse_args(3, args);

        CHECK(parser.format_usage() == "usage: prog p1 p2"s);
    }

    SUBCASE("...for five positional arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("p2");
        parser.add_argument("p3");
        parser.add_argument("p4");
        parser.add_argument("p5");

        char const * args[] = {"prog", "v1", "v2", "v3", "v4", "v5"};
        (void) parser.parse_args(6, args);

        CHECK(parser.format_usage() == "usage: prog p1 p2 p3 p4 p5"s);
    }

    SUBCASE("...for one optional argument")
    {
        parser.add_argument("-o");

        char const * args[] = {"prog"};
        (void) parser.parse_args(1, args);

        CHECK(parser.format_usage() == "usage: prog [-o O]"s);
    }

    SUBCASE("...for two optional arguments")
    {
        parser.add_argument("-o");
        parser.add_argument("--option");

        char const * args[] = {"prog"};
        (void) parser.parse_args(1, args);

        CHECK(parser.format_usage() == "usage: prog [-o O] [--option OPTION]"s);
    }

    SUBCASE("...for five optional arguments")
    {
        parser.add_argument("-o");
        parser.add_argument("--option");
        parser.add_argument("-q");
        parser.add_argument("--long-name");
        parser.add_argument("--very-long-name");

        char const * args[] = {"prog"};
        (void) parser.parse_args(1, args);

        CHECK(parser.format_usage() == "usage: prog [-o O] [--option OPTION] [-q Q] [--long-name LONG_NAME] [--very-long-name VERY_LONG_NAME]"s);
    }

    SUBCASE("...for one positional and one optional argument")
    {
        parser.add_argument("p1");
        parser.add_argument("-o");

        char const * args[] = {"prog", "p1"};
        (void) parser.parse_args(2, args);

        CHECK(parser.format_usage() == "usage: prog [-o O] p1"s);
    }

    SUBCASE("...for five positional and five optional arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("-o");
        parser.add_argument("p2");
        parser.add_argument("-a");
        parser.add_argument("p3");
        parser.add_argument("-z");
        parser.add_argument("p4");
        parser.add_argument("-e");
        parser.add_argument("p5");
        parser.add_argument("-f");

        char const * args[] = {"prog", "p1", "p2", "p3", "p4", "p5"};
        (void) parser.parse_args(6, args);

        CHECK(parser.format_usage() == "usage: prog [-o O] [-a A] [-z Z] [-e E] [-f F] p1 p2 p3 p4 p5"s);
    }
}

TEST_CASE("ArgumentParser provides help message...")
{
    auto parser = argparse::ArgumentParser();

    SUBCASE("...for no arguments")
    {
        char const * args[] = {"prog"};
        (void) parser.parse_args(1, args);

        CHECK(parser.format_help() == "usage: prog"s);
    }

    SUBCASE("...for one positional argument")
    {
        parser.add_argument("p1");

        char const * args[] = {"prog", "v1"};
        (void) parser.parse_args(2, args);

        CHECK(parser.format_help() == "usage: prog p1\n\npositional arguments:\n  p1"s);
    }

    SUBCASE("...for two positional arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("p2");

        char const * args[] = {"prog", "v1", "v2"};
        (void) parser.parse_args(3, args);

        CHECK(parser.format_help() == "usage: prog p1 p2\n\npositional arguments:\n  p1\n  p2"s);
    }

    SUBCASE("...for five positional arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("p2");
        parser.add_argument("p3");
        parser.add_argument("p4");
        parser.add_argument("p5");

        char const * args[] = {"prog", "v1", "v2", "v3", "v4", "v5"};
        (void) parser.parse_args(6, args);

        CHECK(parser.format_help() == "usage: prog p1 p2 p3 p4 p5\n\npositional arguments:\n  p1\n  p2\n  p3\n  p4\n  p5"s);
    }

    SUBCASE("...for one optional argument")
    {
        parser.add_argument("-o");

        char const * args[] = {"prog"};
        (void) parser.parse_args(1, args);

        CHECK(parser.format_help() == "usage: prog [-o O]\n\noptional arguments:\n  -o O"s);
    }

    SUBCASE("...for two optional arguments")
    {
        parser.add_argument("-o");
        parser.add_argument("--option");

        char const * args[] = {"prog"};
        (void) parser.parse_args(1, args);

        CHECK(parser.format_help() == "usage: prog [-o O] [--option OPTION]\n\noptional arguments:\n  -o O\n  --option OPTION"s);
    }

    SUBCASE("...for five optional arguments")
    {
        parser.add_argument("-o");
        parser.add_argument("--option");
        parser.add_argument("-q");
        parser.add_argument("--long-name");
        parser.add_argument("--very-long-name");

        char const * args[] = {"prog"};
        (void) parser.parse_args(1, args);

        CHECK(parser.format_help() == "usage: prog [-o O] [--option OPTION] [-q Q] [--long-name LONG_NAME] [--very-long-name VERY_LONG_NAME]\n\noptional arguments:\n  -o O\n  --option OPTION\n  -q Q\n  --long-name LONG_NAME\n  --very-long-name VERY_LONG_NAME"s);
    }

    SUBCASE("...for one positional and one optional argument")
    {
        parser.add_argument("p1");
        parser.add_argument("-o");

        char const * args[] = {"prog", "p1"};
        (void) parser.parse_args(2, args);

        CHECK(parser.format_help() == "usage: prog [-o O] p1\n\npositional arguments:\n  p1\n\noptional arguments:\n  -o O"s);
    }

    SUBCASE("...for five positional and five optional arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("-o");
        parser.add_argument("p2");
        parser.add_argument("-a");
        parser.add_argument("p3");
        parser.add_argument("-z");
        parser.add_argument("p4");
        parser.add_argument("-e");
        parser.add_argument("p5");
        parser.add_argument("-f");

        char const * args[] = {"prog", "p1", "p2", "p3", "p4", "p5"};
        (void) parser.parse_args(6, args);

        CHECK(parser.format_help() == "usage: prog [-o O] [-a A] [-z Z] [-e E] [-f F] p1 p2 p3 p4 p5\n\npositional arguments:\n  p1\n  p2\n  p3\n  p4\n  p5\n\noptional arguments:\n  -o O\n  -a A\n  -z Z\n  -e E\n  -f F"s);
    }
}

TEST_CASE("Help message contains...")
{
    auto parser = argparse::ArgumentParser();

    SUBCASE("...for positional argument...")
    {
        SUBCASE("...name for argument without help string")
        {
            parser.add_argument("p1");

            char const * args[] = {"prog", "p1"};
            (void) parser.parse_args(2, args);

            CHECK(parser.format_help() == "usage: prog p1\n\npositional arguments:\n  p1"s);
        }

        SUBCASE("...name and help for argument with help string")
        {
            parser.add_argument("p1").help("help1");

            char const * args[] = {"prog", "p1"};
            (void) parser.parse_args(2, args);

            CHECK(parser.format_help() == "usage: prog p1\n\npositional arguments:\n  p1 help1"s);
        }
    }

    SUBCASE("...for optional argument...")
    {
        SUBCASE("...name and metavar for argument without help string")
        {
            parser.add_argument("-o");

            char const * args[] = {"prog"};
            (void) parser.parse_args(1, args);

            CHECK(parser.format_help() == "usage: prog [-o O]\n\noptional arguments:\n  -o O"s);
        }

        SUBCASE("name, metavar, and help for argument with help string")
        {
            parser.add_argument("-o").help("help1");

            char const * args[] = {"prog"};
            (void) parser.parse_args(1, args);

            CHECK(parser.format_help() == "usage: prog [-o O]\n\noptional arguments:\n  -o O help1"s);
        }
    }
}
