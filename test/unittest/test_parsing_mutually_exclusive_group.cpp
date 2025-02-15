#include "argparse.h"

#include "cstring_array.h"

#include "doctest.h"


TEST_CASE("Parsing mutually exclusive group of arguments with store action does not throw for no arguments")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    auto group = parser.add_mutually_exclusive_group();
    group.add_argument("-a");
    group.add_argument("-b");

    CHECK_NOTHROW(parser.parse_args(1, cstr_arr{"prog"}));
}

TEST_CASE("Parsing mutually exclusive group of arguments with store action does not throw for a single argument")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    auto group = parser.add_mutually_exclusive_group();
    group.add_argument("-a");
    group.add_argument("-b");

    SUBCASE("")
    {
        CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-a", "a"}));
    }
    SUBCASE("")
    {
        CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-b", "b"}));
    }
}

TEST_CASE("Parsing mutually exclusive group of arguments with store action does not throw for arguments in separate groups")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    auto group = parser.add_mutually_exclusive_group();
    group.add_argument("-a");
    group.add_argument("-b");
    auto other_group = parser.add_mutually_exclusive_group();
    other_group.add_argument("-c");

    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-a", "a", "-c", "c"}));
}

TEST_CASE("Parsing mutually exclusive group of arguments with store action throws for exclusive arguments")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    auto group = parser.add_mutually_exclusive_group();
    group.add_argument("-a");
    group.add_argument("-b");

    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-a", "a", "-b", "b"}), "argument -b: not allowed with argument -a", argparse::parsing_error);
}

TEST_CASE("Parsing mutually exclusive group of arguments with store true action does not throw for no arguments")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    auto group = parser.add_mutually_exclusive_group();
    group.add_argument("-a").action(argparse::store_true);
    group.add_argument("-b").action(argparse::store_true);

    CHECK_NOTHROW(parser.parse_args(1, cstr_arr{"prog"}));
}

TEST_CASE("Parsing mutually exclusive group of arguments with store true action does not throw for a single argument")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    auto group = parser.add_mutually_exclusive_group();
    group.add_argument("-a").action(argparse::store_true);
    group.add_argument("-b").action(argparse::store_true);

    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-a"}));
}

TEST_CASE("Parsing mutually exclusive group of arguments with store true action does not throw for a single argument")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    auto group = parser.add_mutually_exclusive_group();
    group.add_argument("-a").action(argparse::store_true);
    group.add_argument("-b").action(argparse::store_true);

    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-b"}));
}

TEST_CASE("Parsing mutually exclusive group of arguments with store true action does not throw for arguments in separate groups")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    auto group = parser.add_mutually_exclusive_group();
    group.add_argument("-a").action(argparse::store_true);
    group.add_argument("-b").action(argparse::store_true);
    auto other_group = parser.add_mutually_exclusive_group();
    other_group.add_argument("-c").action(argparse::store_true);

    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-a", "-c"}));
}

TEST_CASE("Parsing mutually exclusive group of arguments with store true action throws for exclusive arguments")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    auto group = parser.add_mutually_exclusive_group();
    group.add_argument("-a").action(argparse::store_true);
    group.add_argument("-b").action(argparse::store_true);

    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-a", "-b"}), "argument -b: not allowed with argument -a", argparse::parsing_error);
}

TEST_CASE("Parsing mutually exclusive group of arguments with store false action does not throw for no arguments")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    auto group = parser.add_mutually_exclusive_group();
    group.add_argument("-a").action(argparse::store_false);
    group.add_argument("-b").action(argparse::store_false);

    CHECK_NOTHROW(parser.parse_args(1, cstr_arr{"prog"}));
}

TEST_CASE("Parsing mutually exclusive group of arguments with store false action does not throw for a single argument")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    auto group = parser.add_mutually_exclusive_group();
    group.add_argument("-a").action(argparse::store_false);
    group.add_argument("-b").action(argparse::store_false);

    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-a"}));
}

TEST_CASE("Parsing mutually exclusive group of arguments with store false action does not throw for a single argument")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    auto group = parser.add_mutually_exclusive_group();
    group.add_argument("-a").action(argparse::store_false);
    group.add_argument("-b").action(argparse::store_false);

    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-b"}));
}

TEST_CASE("Parsing mutually exclusive group of arguments with store false action does not throw for arguments in separate groups")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    auto group = parser.add_mutually_exclusive_group();
    group.add_argument("-a").action(argparse::store_false);
    group.add_argument("-b").action(argparse::store_false);
    auto other_group = parser.add_mutually_exclusive_group();
    other_group.add_argument("-c").action(argparse::store_false);

    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-a", "-c"}));
}

TEST_CASE("Parsing mutually exclusive group of arguments with store false action throws for exclusive arguments")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    auto group = parser.add_mutually_exclusive_group();
    group.add_argument("-a").action(argparse::store_false);
    group.add_argument("-b").action(argparse::store_false);

    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-a", "-b"}), "argument -b: not allowed with argument -a", argparse::parsing_error);
}
