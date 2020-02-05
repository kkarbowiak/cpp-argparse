#include "argparse.h"

#include "cstring_array.h"
#include "custom.h"

#include <doctest.h>

#include <string>


using namespace std::string_literals;

TEST_CASE("Parsing a positional argument yields its value")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("p1");

    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "v1"});

    CHECK(parsed.get_value("p1") == "v1");
}

TEST_CASE("Parsing an optional argument...")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o");

    SUBCASE("...yields false when it's missing")
    {
        auto const parsed = parser.parse_args(1, cstr_arr{"prog"});

        CHECK(!parsed.get("o"));
    }

    SUBCASE("...throws an exception when it's missing argument")
    {
        CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "-o"}), "argument -o: expected one argument", argparse::parsing_error);
    }

    SUBCASE("...yields its value")
    {
        auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "v1"});

        CHECK(parsed.get_value("o") == "v1");
    }
}

TEST_CASE("Parsing an optional argument with store true action...")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o").action(argparse::store_true);

    SUBCASE("...yields false when it's missing")
    {
        auto const parsed = parser.parse_args(1, cstr_arr{"prog"});

        CHECK(parsed.get_value<bool>("o") == false);
    }

    SUBCASE("...yields true when it's present")
    {
        auto const parsed = parser.parse_args(2, cstr_arr{"prog", "-o"});

        CHECK(parsed.get_value<bool>("o") == true);
    }
}

TEST_CASE("Parsing an optional argument with store false action...")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o").action(argparse::store_false);

    SUBCASE("...yields true when it's missing")
    {
        auto const parsed = parser.parse_args(1, cstr_arr{"prog"});

        CHECK(parsed.get_value<bool>("o") == true);
    }

    SUBCASE("...yields false when it's present")
    {
        auto const parsed = parser.parse_args(2, cstr_arr{"prog", "-o"});

        CHECK(parsed.get_value<bool>("o") == false);
    }
}

TEST_CASE("Parsing an optional argument with store const action...")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o").action(argparse::store_const).const_("v1"s);

    SUBCASE("...yields false when it's missing")
    {
        auto const parsed = parser.parse_args(1, cstr_arr{"prog"});

        CHECK(!parsed.get("o"));
    }

    SUBCASE("...yields const value")
    {
        parser.add_argument("-i").action(argparse::store_const).const_(23);
        parser.add_argument("-d").action(argparse::store_const).const_(3.14);

        auto const parsed = parser.parse_args(4, cstr_arr{"prog", "-o", "-i", "-d"});

        CHECK(parsed.get_value("o") == "v1");
        CHECK(parsed.get_value<int>("i") == 23);
        CHECK(parsed.get_value<double>("d") == 3.14);
    }
}

TEST_CASE("Parsing an optional argument with help action...")
{
    auto parser = argparse::ArgumentParser().add_help(false).handle(argparse::Handle::none);
    parser.add_argument("-h").action(argparse::help);

    SUBCASE("...yields false when it's missing")
    {
        auto const parsed = parser.parse_args(1, cstr_arr{"prog"});

        CHECK(parsed.get_value<bool>("h") == false);
    }

    SUBCASE("...yields true when it's present")
    {
        auto const parsed = parser.parse_args(2, cstr_arr{"prog", "-h"});

        CHECK(parsed.get_value<bool>("h") == true);
    }
}

TEST_CASE("Optional argument can be used with either...")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o", "--option");

    SUBCASE("...short name")
    {
        auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "val"});

        CHECK(parsed.get_value("option") == "val");
    }

    SUBCASE("...long name")
    {
        auto const parsed = parser.parse_args(3, cstr_arr{"prog", "--option", "val"});

        CHECK(parsed.get_value("option") == "val");
    }
}

TEST_CASE("Parsing a positional argument yields its requested type")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("c").type<char>();
    parser.add_argument("sc").type<signed char>();
    parser.add_argument("uc").type<unsigned char>();
    parser.add_argument("si").type<short int>();
    parser.add_argument("usi").type<unsigned short int>();
    parser.add_argument("i").type<int>();
    parser.add_argument("li").type<long int>();
    parser.add_argument("lli").type<long long int>();
    parser.add_argument("uli").type<unsigned long int>();
    parser.add_argument("ulli").type<unsigned long long int>();
    parser.add_argument("f").type<float>();
    parser.add_argument("d").type<double>();
    parser.add_argument("ld").type<long double>();
    parser.add_argument("fc").type<foo::Custom>();

    auto const parsed = parser.parse_args(
        15, cstr_arr{
            "prog",
            "65",
            "-12",
            "250",
            "5000",
            "64000",
            "123",
            "2000000000",
            "16000000000",
            "3000000000",
            "24000000000",
            "2.71",
            "3.14",
            "0.111",
            "bar"});

    CHECK(parsed.get_value<char>("c") == 65);
    CHECK(parsed.get_value<signed char>("sc") == -12);
    CHECK(parsed.get_value<unsigned char>("uc") == 250);
    CHECK(parsed.get_value<short int>("si") == 5000);
    CHECK(parsed.get_value<unsigned short int>("usi") == 64000);
    CHECK(parsed.get_value<int>("i") == 123);
    CHECK(parsed.get_value<long int>("li") == 2000000000l);
    CHECK(parsed.get_value<long long int>("lli") == 16000000000ll);
    CHECK(parsed.get_value<unsigned long int>("uli") == 3000000000l);
    CHECK(parsed.get_value<unsigned long long int>("ulli") == 24000000000l);
    CHECK(parsed.get_value<float>("f") == 2.71f);
    CHECK(parsed.get_value<double>("d") == 3.14);
    CHECK(parsed.get_value<long double>("ld") == 0.111l);
    CHECK(parsed.get_value<foo::Custom>("fc") == foo::Custom("bar"));
}

TEST_CASE("Parsing an optional argument yields its requested type")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("--c").type<char>();
    parser.add_argument("--sc").type<signed char>();
    parser.add_argument("--uc").type<unsigned char>();
    parser.add_argument("--si").type<short int>();
    parser.add_argument("--usi").type<unsigned short int>();
    parser.add_argument("--i").type<int>();
    parser.add_argument("--li").type<long int>();
    parser.add_argument("--lli").type<long long int>();
    parser.add_argument("--uli").type<unsigned long int>();
    parser.add_argument("--ulli").type<unsigned long long int>();
    parser.add_argument("--f").type<float>();
    parser.add_argument("--d").type<double>();
    parser.add_argument("--ld").type<long double>();
    parser.add_argument("--fc").type<foo::Custom>();

    auto const parsed = parser.parse_args(
        29, cstr_arr{
            "prog",
            "--c", "65",
            "--sc", "-12",
            "--uc", "250",
            "--si", "5000",
            "--usi", "64000",
            "--i", "123",
            "--li", "2000000000",
            "--lli", "16000000000",
            "--uli", "3000000000",
            "--ulli", "24000000000",
            "--f", "2.71",
            "--d", "3.14",
            "--ld", "0.111",
            "--fc", "bar"});

    CHECK(parsed.get_value<char>("c") == 65);
    CHECK(parsed.get_value<signed char>("sc") == -12);
    CHECK(parsed.get_value<unsigned char>("uc") == 250);
    CHECK(parsed.get_value<short int>("si") == 5000);
    CHECK(parsed.get_value<unsigned short int>("usi") == 64000);
    CHECK(parsed.get_value<int>("i") == 123);
    CHECK(parsed.get_value<long int>("li") == 2000000000l);
    CHECK(parsed.get_value<long long int>("lli") == 16000000000ll);
    CHECK(parsed.get_value<unsigned long int>("uli") == 3000000000l);
    CHECK(parsed.get_value<unsigned long long int>("ulli") == 24000000000l);
    CHECK(parsed.get_value<float>("f") == 2.71f);
    CHECK(parsed.get_value<double>("d") == 3.14);
    CHECK(parsed.get_value<long double>("ld") == 0.111l);
    CHECK(parsed.get_value<foo::Custom>("fc") == foo::Custom("bar"));
}

TEST_CASE("Parsing an optional argument with default value...")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o").default_("foo"s);
    parser.add_argument("--di").default_(10).type<double>();
    parser.add_argument("--id").default_(3.14).type<int>();

    SUBCASE("...yields the default value when it's missing")
    {
        auto const parsed = parser.parse_args(1, cstr_arr{"prog"});

        CHECK(parsed.get_value("o") == "foo");
        CHECK(parsed.get_value<int>("di") == 10);
        CHECK(parsed.get_value<double>("id") == 3.14);
    }

    SUBCASE("...yields value of the argument's type")
    {
        auto const parsed = parser.parse_args(
            7, cstr_arr{
                "prog",
                "-o", "bar",
                "--di", "2.71",
                "--id", "23"});

        CHECK(parsed.get_value("o") == "bar");
        CHECK(parsed.get_value<double>("di") == 2.71);
        CHECK(parsed.get_value<int>("id") == 23);
    }
}

TEST_CASE("Parsing missing positional argument throws an exception...")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    SUBCASE("...for one missing argument")
    {
        parser.add_argument("p1");

        CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "missing arguments: p1", argparse::parsing_error);
    }

    SUBCASE("...for two missing arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("p2");

        CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "missing arguments: p1 p2", argparse::parsing_error);
    }

    SUBCASE("...for three missing arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("p2");
        parser.add_argument("p3");

        CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "missing arguments: p1 p2 p3", argparse::parsing_error);
    }
}

TEST_CASE("Parsing arguments with help requested disregards parsing errors...")
{
    auto parser = argparse::ArgumentParser().add_help(false).handle(argparse::Handle::none);
    parser.add_argument("-h").action(argparse::help);

    SUBCASE("...for missing positional argument")
    {
        parser.add_argument("p1");

        CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-h"}));
    }

    SUBCASE("...for unrecognised positional argument")
    {
        CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "p1", "-h"}));
    }

    SUBCASE("...for unrecognised optional argument")
    {
        CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-a", "-h"}));
    }
}

TEST_CASE("Parsing unrecognised positional argument throws an exception...")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("p1");

    SUBCASE("...for one unrecognised argument")
    {
        CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "v1", "v2"}), "unrecognised arguments: v2", argparse::parsing_error);
    }

    SUBCASE("...for two unrecognised arguments")
    {
        CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "v1", "v2", "v3"}), "unrecognised arguments: v2 v3", argparse::parsing_error);
    }

    SUBCASE("...for three unrecognised arguments")
    {
        CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "v1", "v2", "v3", "v4"}), "unrecognised arguments: v2 v3 v4", argparse::parsing_error);
    }
}

TEST_CASE("Parsing unrecognised optional argument throws an exception...")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-a");

    SUBCASE("...for one unrecognised argument")
    {
        CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-a", "v1", "-b"}), "unrecognised arguments: -b", argparse::parsing_error);
    }

    SUBCASE("...for two unrecognised arguments")
    {
        CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-a", "v1", "-b", "-c"}), "unrecognised arguments: -b -c", argparse::parsing_error);
    }

    SUBCASE("...for three unrecognised arguments")
    {
        CHECK_THROWS_WITH_AS(parser.parse_args(6, cstr_arr{"prog", "-a", "v1", "-b", "-c", "-d"}), "unrecognised arguments: -b -c -d", argparse::parsing_error);
    }
}

TEST_CASE("Parsing mixed positional and optional arguments give same result no matter the order...")
{
    auto parser1 = argparse::ArgumentParser();
    parser1.add_argument("pos1");
    parser1.add_argument("-f");

    auto parser2 = argparse::ArgumentParser();
    parser2.add_argument("-f");
    parser2.add_argument("pos1");

    SUBCASE("...for positional and optional")
    {
        char const * argv[] = {"prog", "val1", "-f", "a"};
        auto const parsed1 = parser1.parse_args(4, argv);
        auto const parsed2 = parser2.parse_args(4, argv);

        CHECK(parsed1.get_value("pos1") == parsed2.get_value("pos1"));
        CHECK(parsed1.get_value("f") == parsed2.get_value("f"));
    }

    SUBCASE("...for optional and positional")
    {
        char const * argv[] = {"prog", "-f", "a", "val1"};
        auto const parsed1 = parser1.parse_args(4, argv);
        auto const parsed2 = parser2.parse_args(4, argv);

        CHECK(parsed1.get_value("pos1") == parsed2.get_value("pos1"));
        CHECK(parsed1.get_value("f") == parsed2.get_value("f"));
    }
}

TEST_CASE("The resulting attribute name is based on...")
{
    auto parser = argparse::ArgumentParser();

    SUBCASE("...for positional argument...")
    {
        SUBCASE("...on its name")
        {
            parser.add_argument("foo");

            auto const parsed = parser.parse_args(2, cstr_arr{"prog", "val"});

            CHECK(parsed.get("foo"));
        }

        SUBCASE("...on dest parameter")
        {
            parser.add_argument("foo").dest("bar");

            auto const parsed = parser.parse_args(2, cstr_arr{"prog", "val"});

            CHECK(parsed.get("bar"));
        }
    }

    SUBCASE("...for optional argument...")
    {
        SUBCASE("...on its long name")
        {
            parser.add_argument("-f", "--foo");

            auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-f", "val"});

            CHECK(parsed.get("foo"));
        }

        SUBCASE("...on its short name")
        {
            parser.add_argument("-f");

            auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-f", "val"});

            CHECK(parsed.get("f"));
        }

        SUBCASE("...on dest parameter")
        {
            parser.add_argument("-f", "--foo").dest("bar");

            auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-f", "val"});

            CHECK(parsed.get("bar"));
        }
    }
}

TEST_CASE("Parsing a missing optional argument with required true throws an exception")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").required(true);

    CHECK_THROWS(parser.parse_args(1, cstr_arr{"prog"}));
}
