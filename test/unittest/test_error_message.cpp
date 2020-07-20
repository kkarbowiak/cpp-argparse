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

TEST_CASE("Invalid choice message lists all optional argument's names")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").choices({"a"s, "b"s});

    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "c"}), "argument -o: invalid choice: \"c\" (choose from \"a\", \"b\")", argparse::parsing_error);
}
