#include "argparse.hpp"

#include "cstring_array.h"

#include "doctest.h"

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

TEST_CASE("ArgumentParser adds help argument automatically with short name")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-h"}));
}

TEST_CASE("ArgumentParser adds help argument automatically with long name")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "--help"}));
}

TEST_CASE("ArgumentParser adds help argument when requested with short name")
{
    auto parser = argparse::ArgumentParser().add_help(true).handle(argparse::Handle::none);

    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-h"}));
}

TEST_CASE("ArgumentParser adds help argument when requested with long name")
{
    auto parser = argparse::ArgumentParser().add_help(true).handle(argparse::Handle::none);

    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "--help"}));
}

TEST_CASE("ArgumentParser does not add help argument when requested not to")
{
    auto parser = argparse::ArgumentParser().add_help(false).handle(argparse::Handle::none);

    CHECK_THROWS(parser.parse_args(2, cstr_arr{"prog", "-h"}));
}

TEST_CASE("ArgumentParser does not handle help when requested to handle nothing")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "-h"});

    CHECK(parsed.get_value<bool>("help") == true);
}

TEST_CASE("ArgumentParser does not handle help when requested to handle parsing errors")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::errors);

    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "-h"});

    CHECK(parsed.get_value<bool>("help") == true);
}

TEST_CASE("ArgumentParser does not handle help when requested to handle version")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::version);

    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "-h"});

    CHECK(parsed.get_value<bool>("help") == true);
}

TEST_CASE("ArgumentParser does not handle parsing errors when requested to handle nothing")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("pos");

    CHECK_THROWS_AS(parser.parse_args(1, cstr_arr{"prog"}), argparse::parsing_error);
}

TEST_CASE("ArgumentParser does not handle parsing errors when requested to handle help")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::help);
    parser.add_argument("pos");

    CHECK_THROWS_AS(parser.parse_args(1, cstr_arr{"prog"}), argparse::parsing_error);
}

TEST_CASE("ArgumentParser does not handle parsing errors when requested to handle version")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::version);
    parser.add_argument("pos");

    CHECK_THROWS_AS(parser.parse_args(1, cstr_arr{"prog"}), argparse::parsing_error);
}

TEST_CASE("ArgumentParser does not handle version when requested to handle nothing")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-v").action(argparse::version);

    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "-v"});

    CHECK(parsed.get_value<bool>("v") == true);
}

TEST_CASE("ArgumentParser does not handle version when requested to handle errors")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::errors);
    parser.add_argument("-v").action(argparse::version);

    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "-v"});

    CHECK(parsed.get_value<bool>("v") == true);
}

TEST_CASE("ArgumentParser does not handle version when requested to handle help")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::help);
    parser.add_argument("-v").action(argparse::version);

    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "-v"});

    CHECK(parsed.get_value<bool>("v") == true);
}

TEST_CASE("ArgumentParser uses first command-line parameter as its name when executed from current directory using no separators in usage message")
{
    auto parser = argparse::ArgumentParser().add_help(false);

    parser.parse_args(1, cstr_arr{"prog"});

    CHECK(parser.format_usage() == "usage: prog"s);
}

TEST_CASE("ArgumentParser uses first command-line parameter as its name when executed from current directory using no separators in help message")
{
    auto parser = argparse::ArgumentParser().add_help(false);

    parser.parse_args(1, cstr_arr{"prog"});

    CHECK(parser.format_help() == "usage: prog"s);
}

TEST_CASE("ArgumentParser uses first command-line parameter as its name when executed from current directory using slash separators in usage message")
{
    auto parser = argparse::ArgumentParser().add_help(false);

    parser.parse_args(1, cstr_arr{"./prog"});

    CHECK(parser.format_usage() == "usage: prog"s);
}

TEST_CASE("ArgumentParser uses first command-line parameter as its name when executed from current directory using slash separators in help message")
{
    auto parser = argparse::ArgumentParser().add_help(false);

    parser.parse_args(1, cstr_arr{"./prog"});

    CHECK(parser.format_help() == "usage: prog"s);
}

TEST_CASE("ArgumentParser uses first command-line parameter as its name when executed from current directory using backslash separators in usage message")
{
    auto parser = argparse::ArgumentParser().add_help(false);

    parser.parse_args(1, cstr_arr{".\\prog"});

    CHECK(parser.format_usage() == "usage: prog"s);
}

TEST_CASE("ArgumentParser uses first command-line parameter as its name when executed from current directory using backslash separators in help message")
{
    auto parser = argparse::ArgumentParser().add_help(false);

    parser.parse_args(1, cstr_arr{".\\prog"});

    CHECK(parser.format_help() == "usage: prog"s);
}

TEST_CASE("ArgumentParser uses first command-line parameter as its name without preceding path when executed from nested directory using slash separators in usage message")
{
    auto parser = argparse::ArgumentParser().add_help(false);

    parser.parse_args(1, cstr_arr{"./utils/prog"});

    CHECK(parser.format_usage() == "usage: prog"s);
}

TEST_CASE("ArgumentParser uses first command-line parameter as its name without preceding path when executed from nested directory using slash separators in help message")
{
    auto parser = argparse::ArgumentParser().add_help(false);

    parser.parse_args(1, cstr_arr{"./utils/prog"});

    CHECK(parser.format_help() == "usage: prog"s);
}

TEST_CASE("ArgumentParser uses first command-line parameter as its name without preceding path when executed from nested directory using backslash separators in usage message")
{
    auto parser = argparse::ArgumentParser().add_help(false);

    parser.parse_args(1, cstr_arr{".\\utils\\prog"});

    CHECK(parser.format_usage() == "usage: prog"s);
}

TEST_CASE("ArgumentParser uses first command-line parameter as its name without preceding path when executed from nested directory using backslash separators in help message")
{
    auto parser = argparse::ArgumentParser().add_help(false);

    parser.parse_args(1, cstr_arr{".\\utils\\prog"});

    CHECK(parser.format_help() == "usage: prog"s);
}

TEST_CASE("ArgumentParser uses first command-line parameter as its name without preceding path when executed from upper directory using slash separators in usage message")
{
    auto parser = argparse::ArgumentParser().add_help(false);

    parser.parse_args(1, cstr_arr{"../prog"});

    CHECK(parser.format_usage() == "usage: prog"s);
}

TEST_CASE("ArgumentParser uses first command-line parameter as its name without preceding path when executed from upper directory using slash separators in help message")
{
    auto parser = argparse::ArgumentParser().add_help(false);

    parser.parse_args(1, cstr_arr{"../prog"});

    CHECK(parser.format_help() == "usage: prog"s);
}

TEST_CASE("ArgumentParser uses first command-line parameter as its name without preceding path when executed from upper directory using backslash separators in usage message")
{
    auto parser = argparse::ArgumentParser().add_help(false);

    parser.parse_args(1, cstr_arr{"..\\prog"});

    CHECK(parser.format_usage() == "usage: prog"s);
}

TEST_CASE("ArgumentParser uses first command-line parameter as its name without preceding path when executed from upper directory using backslash separators in help message")
{
    auto parser = argparse::ArgumentParser().add_help(false);

    parser.parse_args(1, cstr_arr{"..\\prog"});

    CHECK(parser.format_help() == "usage: prog"s);
}

TEST_CASE("ArgumentParser uses prog parameter as its name in usage message")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);

    CHECK(parser.format_usage() == "usage: prog"s);
}

TEST_CASE("ArgumentParser uses prog parameter as its name in help message")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);

    CHECK(parser.format_help() == "usage: prog"s);
}

TEST_CASE("ArgumentParser uses usage parameter as its usage in usage message")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false).usage("program [options]");

    CHECK(parser.format_usage() == "usage: program [options]"s);
}

TEST_CASE("ArgumentParser uses usage parameter as its usage in help message")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false).usage("program [options]");

    CHECK(parser.format_help() == "usage: program [options]"s);
}

TEST_CASE("ArgumentParser replaces '{prog}' with program name in usage text")
{
    auto parser = argparse::ArgumentParser().prog("program").add_help(false).usage("{prog} [options]");

    CHECK(parser.format_usage() == "usage: program [options]"s);
}

TEST_CASE("ArgumentParser replaces '{prog}' with program name in usage text")
{
    auto parser = argparse::ArgumentParser().prog("program").add_help(false).usage("{prog} [options]");

    CHECK(parser.format_help() == "usage: program [options]"s);
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
    CHECK_NOTHROW(parser.add_argument("-o").required(false));
}

TEST_CASE("ArgumentParser supports mutually exclusive groups")
{
    auto parser = argparse::ArgumentParser();
    auto group = parser.add_mutually_exclusive_group();
    group.add_argument("-o");
}

TEST_CASE("Test combining Handle values")
{
    CHECK((argparse::Handle::none | argparse::Handle::errors) == argparse::Handle::errors);
}

TEST_CASE("Requesting non-existing argument throws an exception")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("foo");
    auto args = parser.parse_args(2, cstr_arr{"prog", "val"});

    CHECK_THROWS_WITH(args.get("boo"), "no such argument: 'boo'");
}

TEST_CASE("Requesting non-existing argument throws an exception")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("--foo");
    auto args = parser.parse_args(3, cstr_arr{"prog", "--foo", "val"});

    CHECK_THROWS_WITH(args.get("boo"), "no such argument: 'boo'");
}
