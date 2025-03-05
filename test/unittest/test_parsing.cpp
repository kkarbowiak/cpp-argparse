#include "argparse.h"

#include "cstring_array.h"

#include "doctest.h"

#include <string>
#include <type_traits>


TEST_CASE("Parsing arguments with help requested disregards parsing errors for missing positional argument")
{
    auto parser = argparse::ArgumentParser().add_help(false).handle(argparse::Handle::none);
    parser.add_argument("-h").action(argparse::help);
    parser.add_argument("p1");

    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-h"}));
}

TEST_CASE("Parsing arguments with help requested disregards parsing errors for unrecognised positional argument")
{
    auto parser = argparse::ArgumentParser().add_help(false).handle(argparse::Handle::none);
    parser.add_argument("-h").action(argparse::help);

    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "p1", "-h"}));
}

TEST_CASE("Parsing arguments with help requested disregards parsing errors for unrecognised optional argument")
{
    auto parser = argparse::ArgumentParser().add_help(false).handle(argparse::Handle::none);
    parser.add_argument("-h").action(argparse::help);

    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-a", "-h"}));
}

TEST_CASE("Parsing mixed positional and optional arguments give same result no matter the order for positional and optional")
{
    auto parser1 = argparse::ArgumentParser();
    parser1.add_argument("pos1");
    parser1.add_argument("-f");

    auto parser2 = argparse::ArgumentParser();
    parser2.add_argument("-f");
    parser2.add_argument("pos1");
    auto const argv = cstr_arr{"prog", "val1", "-f", "a"};
    auto const parsed1 = parser1.parse_args(4, argv);
    auto const parsed2 = parser2.parse_args(4, argv);

    CHECK(parsed1.get_value("pos1") == parsed2.get_value("pos1"));
    CHECK(parsed1.get_value("f") == parsed2.get_value("f"));
}

TEST_CASE("Parsing mixed positional and optional arguments give same result no matter the order for optional and positional")
{
    auto parser1 = argparse::ArgumentParser();
    parser1.add_argument("pos1");
    parser1.add_argument("-f");

    auto parser2 = argparse::ArgumentParser();
    parser2.add_argument("-f");
    parser2.add_argument("pos1");
    auto const argv = cstr_arr{"prog", "-f", "a", "val1"};
    auto const parsed1 = parser1.parse_args(4, argv);
    auto const parsed2 = parser2.parse_args(4, argv);

    CHECK(parsed1.get_value("pos1") == parsed2.get_value("pos1"));
    CHECK(parsed1.get_value("f") == parsed2.get_value("f"));
}

TEST_CASE("Parsing -- pseudo argument does not throw")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "--"}));
}

TEST_CASE("Arguments past the -- pseudo argument are treated as positional for optional arguments")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").action(argparse::store_true);

    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "--", "-o"}), "unrecognised arguments: -o", argparse::parsing_error);
}

TEST_CASE("Arguments past the -- pseudo argument are treated as positional for positional arguments")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("pos");

    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "--", "-o"});

    CHECK(parsed.get_value("pos") == "-o");
}

TEST_CASE("Positional argument does not consume unrecognised optional argument")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("pos");

    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "-o"}), "unrecognised arguments: -o", argparse::parsing_error);
}

TEST_CASE("Positional argument does not consume unrecognised optional argument")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("pos");

    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "--option"}), "unrecognised arguments: --option", argparse::parsing_error);
}

TEST_CASE("Positional argument does not consume unrecognised optional argument")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("pos").nargs(1);

    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "-o"}), "unrecognised arguments: -o", argparse::parsing_error);
}

TEST_CASE("Positional argument does not consume unrecognised optional argument")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("pos").nargs(2);

    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "-o"}), "unrecognised arguments: -o", argparse::parsing_error);
}

TEST_CASE("Positional argument does not consume unrecognised optional argument")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("pos").nargs(argparse::zero_or_one);

    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "-o"}), "unrecognised arguments: -o", argparse::parsing_error);
}

TEST_CASE("Positional argument does not consume unrecognised optional argument")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("pos").nargs(argparse::zero_or_more);

    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "-o"}), "unrecognised arguments: -o", argparse::parsing_error);
}
