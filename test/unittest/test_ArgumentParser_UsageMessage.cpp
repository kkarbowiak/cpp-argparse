#include "argparse.h"

#include <doctest.h>

#include <string>


using namespace std::string_literals;

TEST_CASE("ArgumentParser provides usage message...")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);

    SUBCASE("...with just prog name for no arguments")
    {
        CHECK(parser.format_usage() == "usage: prog"s);
    }

    SUBCASE("...with prog name and list of positional arguments for one positional argument")
    {
        parser.add_argument("p1");

        CHECK(parser.format_usage() == "usage: prog p1"s);
    }

    SUBCASE("...with prog name and list of positional arguments for two positional arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("p2");

        CHECK(parser.format_usage() == "usage: prog p1 p2"s);
    }

    SUBCASE("...with prog name and list of positional arguments for three positional arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("p2");
        parser.add_argument("p3");

        CHECK(parser.format_usage() == "usage: prog p1 p2 p3"s);
    }

    SUBCASE("...with prog name and list of optional arguments for one optional argument")
    {
        parser.add_argument("-o");

        CHECK(parser.format_usage() == "usage: prog [-o O]"s);
    }

    SUBCASE("...with prog name and list of optional arguments for two optional arguments")
    {
        parser.add_argument("-o");
        parser.add_argument("--option");

        CHECK(parser.format_usage() == "usage: prog [-o O] [--option OPTION]"s);
    }

    SUBCASE("...with prog name and list of optional arguments for three optional arguments")
    {
        parser.add_argument("-o");
        parser.add_argument("--option");
        parser.add_argument("--very-long-name");

        CHECK(parser.format_usage() == "usage: prog [-o O] [--option OPTION] [--very-long-name VERY_LONG_NAME]"s);
    }

    SUBCASE("...for one optional argument with short and long name")
    {
        parser.add_argument("-f", "--foo");

        CHECK(parser.format_usage() == "usage: prog [-f FOO]"s);
    }

    SUBCASE("... for one optional argument with store true action")
    {
        parser.add_argument("-o").action(argparse::store_true);

        CHECK(parser.format_usage() == "usage: prog [-o]"s);
    }

    SUBCASE("...for two optional arguments with store true action")
    {
        parser.add_argument("-o").action(argparse::store_true);
        parser.add_argument("--option").action(argparse::store_true);

        CHECK(parser.format_usage() == "usage: prog [-o] [--option]"s);
    }

    SUBCASE("...for one optional argument with store false action")
    {
        parser.add_argument("-o").action(argparse::store_false);

        CHECK(parser.format_usage() == "usage: prog [-o]"s);
    }

    SUBCASE("...for two optional arguments with store false action")
    {
        parser.add_argument("-o").action(argparse::store_false);
        parser.add_argument("--option").action(argparse::store_false);

        CHECK(parser.format_usage() == "usage: prog [-o] [--option]"s);
    }

    SUBCASE("...for one optional argument with help action")
    {
        parser.add_argument("-h").action(argparse::help);

        CHECK(parser.format_usage() == "usage: prog [-h]"s);
    }

    SUBCASE("...for one positional and one optional argument")
    {
        parser.add_argument("p1");
        parser.add_argument("-o");

        CHECK(parser.format_usage() == "usage: prog [-o O] p1"s);
    }

    SUBCASE("...for three positional and three optional arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("-o");
        parser.add_argument("p2");
        parser.add_argument("-a");
        parser.add_argument("p3");
        parser.add_argument("-z");

        CHECK(parser.format_usage() == "usage: prog [-o O] [-a A] [-z Z] p1 p2 p3"s);
    }
}

TEST_CASE("Usage message contains...")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);

    SUBCASE("...for positional argument...")
    {
        SUBCASE("...its name")
        {
            parser.add_argument("p1");

            CHECK(parser.format_usage() == "usage: prog p1"s);
        }
    }

    SUBCASE("...for optional argument...")
    {
        SUBCASE("...name for argument with store true action")
        {
            parser.add_argument("-o").action(argparse::store_true);

            CHECK(parser.format_usage() == "usage: prog [-o]");
        }

        SUBCASE("...name for argument with store false action")
        {
            parser.add_argument("-o").action(argparse::store_false);

            CHECK(parser.format_usage() == "usage: prog [-o]");
        }

        SUBCASE("...name for argument with help action")
        {
            parser.add_argument("-h").action(argparse::store_false);

            CHECK(parser.format_usage() == "usage: prog [-h]");
        }

        SUBCASE("...name and metavar")
        {
            parser.add_argument("-o");

            CHECK(parser.format_usage() == "usage: prog [-o O]"s);
        }
    }
}
