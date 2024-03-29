#include "argparse.h"

#include "cstring_array.h"

#include "doctest.h"


using namespace std::string_literals;

TEST_CASE("Missing required arguments message lists all optional argument's names...")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    SUBCASE("...for one argument...")
    {
        SUBCASE("...with one name")
        {
            parser.add_argument("-o").required(true);

            CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: -o", argparse::parsing_error);
        }

        SUBCASE("...with two names")
        {
            parser.add_argument("-o", "--option").required(true);

            CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: -o/--option", argparse::parsing_error);
        }

        SUBCASE("...with three names")
        {
            parser.add_argument("-o", "--option", "--long-option").required(true);

            CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: -o/--option/--long-option", argparse::parsing_error);
        }
    }

    SUBCASE("...for two arguments...")
    {
        SUBCASE("...with one name")
        {
            parser.add_argument("-o").required(true);
            parser.add_argument("-r").required(true);

            CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: -o -r", argparse::parsing_error);
        }

        SUBCASE("...with two names")
        {
            parser.add_argument("-o", "--option").required(true);
            parser.add_argument("-r", "--required").required(true);

            CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: -o/--option -r/--required", argparse::parsing_error);
        }

        SUBCASE("...with three names")
        {
            parser.add_argument("-o", "--option", "--long-option").required(true);
            parser.add_argument("-r", "--required", "--really-required").required(true);

            CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: -o/--option/--long-option -r/--required/--really-required", argparse::parsing_error);
        }
    }
}

TEST_CASE("Invalid choice message lists all optional argument's names...")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    SUBCASE("...for argument with one name")
    {
        parser.add_argument("-o").choices({"a"s, "b"s});

        CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "c"}), "argument -o: invalid choice: \"c\" (choose from \"a\", \"b\")", argparse::parsing_error);
    }

    SUBCASE("...for argument with two names")
    {
        parser.add_argument("-o", "--option").choices({"a"s, "b"s});

        CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "c"}), "argument -o/--option: invalid choice: \"c\" (choose from \"a\", \"b\")", argparse::parsing_error);
    }

    SUBCASE("...for argument with three names")
    {
        parser.add_argument("-o", "--option", "--long-option").choices({"a"s, "b"s});

        CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "c"}), "argument -o/--option/--long-option: invalid choice: \"c\" (choose from \"a\", \"b\")", argparse::parsing_error);
    }
}

TEST_CASE("Expected number of arguments message lists all optional argument's names...")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    SUBCASE("...for argument with one name")
    {
        parser.add_argument("-o").nargs(1);

        CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "-o"}), "argument -o: expected 1 argument", argparse::parsing_error);
    }

    SUBCASE("...for argument with two names")
    {
        parser.add_argument("-o", "--option").nargs(1);

        CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "-o"}), "argument -o/--option: expected 1 argument", argparse::parsing_error);
    }

    SUBCASE("...for argument with three names")
    {
        parser.add_argument("-o", "--option", "--long-option").nargs(1);

        CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "-o"}), "argument -o/--option/--long-option: expected 1 argument", argparse::parsing_error);
    }
}

TEST_CASE("Expected at least one argument message lists all optional argument's names...")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    SUBCASE("...for argument with one name")
    {
        parser.add_argument("-o").nargs(argparse::one_or_more);

        CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "-o"}), "argument -o: expected at least one argument", argparse::parsing_error);
    }

    SUBCASE("...for argument with two names")
    {
        parser.add_argument("-o", "--option").nargs(argparse::one_or_more);

        CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "-o"}), "argument -o/--option: expected at least one argument", argparse::parsing_error);
    }

    SUBCASE("...for argument with three names")
    {
        parser.add_argument("-o", "--option", "--long-option").nargs(argparse::one_or_more);

        CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "-o"}), "argument -o/--option/--long-option: expected at least one argument", argparse::parsing_error);
    }
}

TEST_CASE("Expected one argument message lists all optional argument's names...")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    SUBCASE("...for argument with one name")
    {
        parser.add_argument("-o");

        CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "-o"}), "argument -o: expected one argument", argparse::parsing_error);
    }

    SUBCASE("...for argument with two names")
    {
        parser.add_argument("-o", "--option");

        CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "-o"}), "argument -o/--option: expected one argument", argparse::parsing_error);
    }

    SUBCASE("...for argument with three names")
    {
        parser.add_argument("-o", "--option", "--long-option");

        CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "-o"}), "argument -o/--option/--long-option: expected one argument", argparse::parsing_error);
    }
}

TEST_CASE("Argument not allowed with argument message lists all optional argument's names...")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    auto group = parser.add_mutually_exclusive_group();

    SUBCASE("...for arguments with one name")
    {
        group.add_argument("-a");
        group.add_argument("-b");

        CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-a", "a", "-b", "b"}), "argument -b: not allowed with argument -a", argparse::parsing_error);
    }

    SUBCASE("...for arguments with two names")
    {
        group.add_argument("-a", "--aaa");
        group.add_argument("-b", "--bbb");

        CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-a", "a", "-b", "b"}), "argument -b/--bbb: not allowed with argument -a/--aaa", argparse::parsing_error);
    }

    SUBCASE("...for arguments with three names")
    {
        group.add_argument("-a", "--aaa", "--more-aaa");
        group.add_argument("-b", "--bbb", "--more-bbb");

        CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-a", "a", "-b", "b"}), "argument -b/--bbb/--more-bbb: not allowed with argument -a/--aaa/--more-aaa", argparse::parsing_error);
    }
}
