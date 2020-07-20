#include "argparse.h"

#include "cstring_array.h"

#include "doctest.h"


TEST_CASE("Missing required arguments message lists all argument's names...")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    SUBCASE("...for argument with one name")
    {
        parser.add_argument("-o").required(true);

        CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: -o", argparse::parsing_error);
    }

    SUBCASE("...for argument with two names")
    {
        parser.add_argument("-o", "--option").required(true);

        CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: -o/--option", argparse::parsing_error);
    }

    SUBCASE("...for argument with three names")
    {
        parser.add_argument("-o", "--option", "--long-option").required(true);

        CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: -o/--option/--long-option", argparse::parsing_error);
    }
}
