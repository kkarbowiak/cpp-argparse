#include "argparse.h"

#include <doctest.h>

#include <string>


using namespace std::string_literals;

TEST_CASE("ArgumentParser provides help message...")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);

    SUBCASE("...with just usage section for no arguments")
    {
        CHECK(parser.format_help() == "usage: prog"s);
    }

    SUBCASE("...with usage section and description for description and no arguments")
    {
        auto parser = argparse::ArgumentParser().prog("prog").description("A foo that bars").add_help(false);

        CHECK(parser.format_help() == "usage: prog\n\nA foo that bars"s);
    }

    SUBCASE("...with usage section and epilog for epilog and no arguments")
    {
        auto parser = argparse::ArgumentParser().prog("prog").epilog("And that's how you'd foo a bar").add_help(false);

        CHECK(parser.format_help() == "usage: prog\n\nAnd that's how you'd foo a bar"s);
    }

    SUBCASE("...with usage section and positional arguments section for one positional argument")
    {
        parser.add_argument("p1");

        CHECK(parser.format_help() == "usage: prog p1\n\npositional arguments:\n  p1"s);
    }

    SUBCASE("...with usage section and positional arguments section for two positional arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("p2");

        CHECK(parser.format_help() == "usage: prog p1 p2\n\npositional arguments:\n  p1\n  p2"s);
    }

    SUBCASE("...with usage section and positional arguments section for three positional arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("p2");
        parser.add_argument("p3");

        CHECK(parser.format_help() == "usage: prog p1 p2 p3\n\npositional arguments:\n  p1\n  p2\n  p3"s);
    }

    SUBCASE("...with usage section and optional arguments section for one optional argument")
    {
        parser.add_argument("-o");

        CHECK(parser.format_help() == "usage: prog [-o O]\n\noptional arguments:\n  -o O"s);
    }

    SUBCASE("...with usage section and optional arguments section for two optional arguments")
    {
        parser.add_argument("-o");
        parser.add_argument("--option");

        CHECK(parser.format_help() == "usage: prog [-o O] [--option OPTION]\n\noptional arguments:\n  -o O\n  --option OPTION"s);
    }

    SUBCASE("...with usage section and optional arguments section for three optional arguments")
    {
        parser.add_argument("-o");
        parser.add_argument("--option");
        parser.add_argument("--very-long-name");

        CHECK(parser.format_help() == "usage: prog [-o O] [--option OPTION] [--very-long-name VERY_LONG_NAME]\n\noptional arguments:\n  -o O\n  --option OPTION\n  --very-long-name VERY_LONG_NAME"s);
    }

    SUBCASE("...with usage section, positional arguments section, and optional arguments section for one positional and one optional argument")
    {
        parser.add_argument("p1");
        parser.add_argument("-o");

        CHECK(parser.format_help() == "usage: prog [-o O] p1\n\npositional arguments:\n  p1\n\noptional arguments:\n  -o O"s);
    }

    SUBCASE("...with usage section, positional arguments section, and optional arguments section for three positional and three optional arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("-o");
        parser.add_argument("p2");
        parser.add_argument("-a");
        parser.add_argument("p3");
        parser.add_argument("-z");

        CHECK(parser.format_help() == "usage: prog [-o O] [-a A] [-z Z] p1 p2 p3\n\npositional arguments:\n  p1\n  p2\n  p3\n\noptional arguments:\n  -o O\n  -a A\n  -z Z"s);
    }
}

TEST_CASE("Help message contains...")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);

    SUBCASE("...for positional argument...")
    {
        SUBCASE("...name for argument without help string")
        {
            parser.add_argument("p1");

            CHECK(parser.format_help() == "usage: prog p1\n\npositional arguments:\n  p1"s);
        }

        SUBCASE("...metavar for argument with metavar set and without help string")
        {
            parser.add_argument("p1").metavar("metap1");

            CHECK(parser.format_help() == "usage: prog metap1\n\npositional arguments:\n  metap1"s);
        }

        SUBCASE("...name and help for argument with help string")
        {
            parser.add_argument("p1").help("help1");

            CHECK(parser.format_help() == "usage: prog p1\n\npositional arguments:\n  p1 help1"s);
        }
    }

    SUBCASE("...for optional argument...")
    {
        SUBCASE("...name for argument with store true action and without help string")
        {
            parser.add_argument("-o").action(argparse::store_true);

            CHECK(parser.format_help() == "usage: prog [-o]\n\noptional arguments:\n  -o"s);
        }

        SUBCASE("...name and help for argument with store true action and help string")
        {
            parser.add_argument("-o").action(argparse::store_true).help("help1");

            CHECK(parser.format_help() == "usage: prog [-o]\n\noptional arguments:\n  -o help1"s);
        }

        SUBCASE("...name for argument with store false action and without help string")
        {
            parser.add_argument("-o").action(argparse::store_false);

            CHECK(parser.format_help() == "usage: prog [-o]\n\noptional arguments:\n  -o"s);
        }

        SUBCASE("...name and help for argument with store false action and help string")
        {
            parser.add_argument("-o").action(argparse::store_false).help("help1");

            CHECK(parser.format_help() == "usage: prog [-o]\n\noptional arguments:\n  -o help1"s);
        }

        SUBCASE("...name for argument with help action and without help string")
        {
            parser.add_argument("-h").action(argparse::help);

            CHECK(parser.format_help() == "usage: prog [-h]\n\noptional arguments:\n  -h"s);
        }

        SUBCASE("...name and help for argument with help action and help string")
        {
            parser.add_argument("-h").action(argparse::help).help("help1");

            CHECK(parser.format_help() == "usage: prog [-h]\n\noptional arguments:\n  -h help1"s);
        }

        SUBCASE("...name and help for automatically added help argument")
        {
            auto parser = argparse::ArgumentParser().prog("prog");

            CHECK(parser.format_help() == "usage: prog [-h]\n\noptional arguments:\n  -h show this help message and exit"s);
        }

        SUBCASE("...name and automatic metavar for argument without help string")
        {
            parser.add_argument("-o");

            CHECK(parser.format_help() == "usage: prog [-o O]\n\noptional arguments:\n  -o O"s);
        }

        SUBCASE("...name and metavar for argument with metavar set and without help string")
        {
            parser.add_argument("-o").metavar("METAVARO");

            CHECK(parser.format_help() == "usage: prog [-o METAVARO]\n\noptional arguments:\n  -o METAVARO"s);
        }

        SUBCASE("name, automatic metavar, and help for argument with help string")
        {
            parser.add_argument("-o").help("help1");

            CHECK(parser.format_help() == "usage: prog [-o O]\n\noptional arguments:\n  -o O help1"s);
        }
    }
}
