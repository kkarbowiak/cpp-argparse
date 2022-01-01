#include "argparse.h"

#include "cstring_array.h"

#include "doctest.h"


TEST_CASE("Parsing mutually exclusive group...")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    auto group = parser.add_mutually_exclusive_group();

    SUBCASE("...of arguments with store action...")
    {
        group.add_argument("-a");
        group.add_argument("-b");

        SUBCASE("...does not throw for no arguments")
        {
            CHECK_NOTHROW(parser.parse_args(1, cstr_arr{"prog"}));
        }

        SUBCASE("...does not throw for a single argument")
        {
            CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-a", "a"}));
            CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-b", "b"}));
        }

        SUBCASE("..does not throw for arguments in separate groups")
        {
            auto other_group = parser.add_mutually_exclusive_group();
            other_group.add_argument("-c");

            CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-a", "a", "-c", "c"}));
        }

        SUBCASE("...throws for exclusive arguments")
        {
            CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-a", "a", "-b", "b"}), "argument -b: not allowed with argument -a", argparse::parsing_error);
        }
    }
}
