#include "argparse.h"

#include "cstring_array.h"
#include "custom_a.h"
#include "custom_b.h"

#include "doctest.h"

#include <string>
#include <type_traits>


using namespace std::string_literals;

namespace bar
{
auto operator==(Custom const & lhs, Custom const & rhs) -> bool
{
    return lhs.m_text == rhs.m_text;
}
}

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

TEST_CASE_TEMPLATE("Parsing an optional argument with store const action...", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string, foo::Custom, bar::Custom)
{
    auto parser = argparse::ArgumentParser();

    if constexpr (std::is_integral_v<T>)
    {
        parser.add_argument("-o").action(argparse::store_const).const_(T(65));
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        parser.add_argument("-o").action(argparse::store_const).const_(T(1.125));
    }
    else
    {
        parser.add_argument("-o").action(argparse::store_const).const_(T("bar"));
    }

    SUBCASE("...yields false when it's missing")
    {
        auto const parsed = parser.parse_args(1, cstr_arr{"prog"});

        CHECK(!parsed.get("o"));
    }

    SUBCASE("...yields const value")
    {
        auto const parsed = parser.parse_args(2, cstr_arr{"prog", "-o"});

        if constexpr (std::is_integral_v<T>)
        {
            CHECK(parsed.get_value<T>("o") == T(65));
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            CHECK(parsed.get_value<T>("o") == T(1.125));
        }
        else
        {
            CHECK(parsed.get_value<T>("o") == T("bar"));
        }
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

TEST_CASE_TEMPLATE("Parsing a positional argument yields its requested type", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, foo::Custom, bar::Custom)
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("pos").type<T>();

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        auto const parsed = parser.parse_args(2, cstr_arr{"prog", "A"});

        CHECK(parsed.get_value<T>("pos") == T(65));
    }
    else if constexpr (std::is_integral_v<T>)
    {
        auto const parsed = parser.parse_args(2, cstr_arr{"prog", "65"});

        CHECK(parsed.get_value<T>("pos") == T(65));
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        auto const parsed = parser.parse_args(2, cstr_arr{"prog", "1.125"});

        CHECK(parsed.get_value<T>("pos") == T(1.125));
    }
    else
    {
        auto const parsed = parser.parse_args(2, cstr_arr{"prog", "bar"});

        CHECK(parsed.get_value<T>("pos") == T("bar"));
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument yields its requested type", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, foo::Custom, bar::Custom)
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o").type<T>();

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "A"});

        CHECK(parsed.get_value<T>("o") == T(65));
    }
    else if constexpr (std::is_integral_v<T>)
    {
        auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "65"});

        CHECK(parsed.get_value<T>("o") == T(65));
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "1.125"});

        CHECK(parsed.get_value<T>("o") == T(1.125));
    }
    else
    {
        auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "bar"});

        CHECK(parsed.get_value<T>("o") == T("bar"));
    }
}

TEST_CASE("Parsing a positional argument with invalid value throws an exception")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("pos").type<int>();

    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "not-a-number"}), "argument pos: invalid value: 'not-a-number'", argparse::parsing_error);
    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "10gibberish"}), "argument pos: invalid value: '10gibberish'", argparse::parsing_error);
}

TEST_CASE("Parsing an optional argument with invalid value throws an exception")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").type<int>();

    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "not-a-number"}), "argument -o: invalid value: 'not-a-number'", argparse::parsing_error);
    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "10gibberish"}), "argument -o: invalid value: '10gibberish'", argparse::parsing_error);
}

TEST_CASE_TEMPLATE("Parsing an optional argument with default value...", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string, foo::Custom, bar::Custom)
{
    auto parser = argparse::ArgumentParser();

    if constexpr (std::is_integral_v<T>)
    {
        parser.add_argument("-o").default_(T(54)).template type<T>();
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        parser.add_argument("-o").default_(T(0.125)).template type<T>();
    }
    else
    {
        parser.add_argument("-o").default_(T("foo")).template type<T>();
    }

    SUBCASE("...yields the default value when it's missing")
    {
        auto const parsed = parser.parse_args(1, cstr_arr{"prog"});

        if constexpr (std::is_integral_v<T>)
        {
            CHECK(parsed.get_value<T>("o") == T(54));
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            CHECK(parsed.get_value<T>("o") == T(0.125));
        }
        else
        {
            CHECK(parsed.get_value<T>("o") == T("foo"));
        }
    }

    SUBCASE("...yields value of the argument's type")
    {
        if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
        {
            auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "A"});

            CHECK(parsed.get_value<T>("o") == T(65));
        }
        else if constexpr (std::is_integral_v<T>)
        {
            auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "65"});

            CHECK(parsed.get_value<T>("o") == T(65));
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "1.125"});

            CHECK(parsed.get_value<T>("o") == T(1.125));
        }
        else
        {
            auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "bar"});

            CHECK(parsed.get_value<T>("o") == T("bar"));
        }
    }
}

TEST_CASE("Parsing missing positional argument throws an exception...")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    SUBCASE("...for one missing argument")
    {
        parser.add_argument("p1");

        CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: p1", argparse::parsing_error);
    }

    SUBCASE("...for two missing arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("p2");

        CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: p1 p2", argparse::parsing_error);
    }

    SUBCASE("...for three missing arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("p2");
        parser.add_argument("p3");

        CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: p1 p2 p3", argparse::parsing_error);
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
        auto const argv = cstr_arr{"prog", "val1", "-f", "a"};
        auto const parsed1 = parser1.parse_args(4, argv);
        auto const parsed2 = parser2.parse_args(4, argv);

        CHECK(parsed1.get_value("pos1") == parsed2.get_value("pos1"));
        CHECK(parsed1.get_value("f") == parsed2.get_value("f"));
    }

    SUBCASE("...for optional and positional")
    {
        auto const argv = cstr_arr{"prog", "-f", "a", "val1"};
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

TEST_CASE("Parsing a missing optional argument with required...")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    SUBCASE("...true throws an exception")
    {
        parser.add_argument("-o").required(true);

        CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: -o", argparse::parsing_error);
    }

    SUBCASE("...false does not throw")
    {
        parser.add_argument("-o").required(false);

        CHECK_NOTHROW(parser.parse_args(1, cstr_arr{"prog"}));
    }
}

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set...", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string, foo::Custom, bar::Custom)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    SUBCASE("...accepts one of the values...")
    {
        if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
        {
            parser.add_argument("pos").choices({T('A'), T('C')}).template type<T>();

            CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "A"}));
            CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "C"}));
        }
        else if constexpr (std::is_integral_v<T>)
        {
            parser.add_argument("pos").choices({T(23), T(34)}).template type<T>();

            CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "23"}));
            CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "34"}));
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            parser.add_argument("pos").choices({T(0.125), T(1.5)}).template type<T>();

            CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "0.125"}));
            CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "1.5"}));
        }
        else
        {
            parser.add_argument("pos").choices({T("foo"), T("bar")}).template type<T>();

            CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "foo"}));
            CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "bar"}));
        }
    }

    SUBCASE("...throws an exception on incorrect value...")
    {
        if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
        {
            parser.add_argument("pos").choices({T('A'), T('C')}).template type<T>();

            CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "B"}), "argument pos: invalid choice: B (choose from A, C)", argparse::parsing_error);
        }
        else if constexpr (std::is_integral_v<T>)
        {
            parser.add_argument("pos").choices({T(23), T(34)}).template type<T>();

            CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "3"}), "argument pos: invalid choice: 3 (choose from 23, 34)", argparse::parsing_error);
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            parser.add_argument("pos").choices({T(0.125), T(1.5)}).template type<T>();

            CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "0.5"}), "argument pos: invalid choice: 0.5 (choose from 0.125, 1.5)", argparse::parsing_error);
        }
        else if constexpr (std::is_same_v<std::string, T>)
        {
            parser.add_argument("pos").choices({T("foo"), T("bar")}).template type<T>();

            CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "baz"}), "argument pos: invalid choice: \"baz\" (choose from \"foo\", \"bar\")", argparse::parsing_error);
        }
        else
        {
            parser.add_argument("pos").choices({T("foo"), T("bar")}).template type<T>();

            CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "baz"}), "argument pos: invalid choice: <Custom: baz> (choose from <Custom: foo>, <Custom: bar>)", argparse::parsing_error);
        }
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set...", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string, foo::Custom, bar::Custom)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    SUBCASE("...accepts one of the values...")
    {
        if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
        {
            parser.add_argument("-o").choices({T('A'), T('C')}).template type<T>();

            CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "A"}));
            CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "C"}));
        }
        else if constexpr (std::is_integral_v<T>)
        {
            parser.add_argument("-o").choices({T(23), T(34)}).template type<T>();

            CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "23"}));
            CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "34"}));
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            parser.add_argument("-o").choices({T(0.125), T(1.5)}).template type<T>();

            CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "0.125"}));
            CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "1.5"}));
        }
        else
        {
            parser.add_argument("-o").choices({T("foo"), T("bar")}).template type<T>();

            CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "foo"}));
            CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "bar"}));
        }
    }

    SUBCASE("...throws an exception on incorrect value...")
    {
        if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
        {
            parser.add_argument("-o").choices({T('A'), T('C')}).template type<T>();

            CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "B"}), "argument -o: invalid choice: B (choose from A, C)", argparse::parsing_error);
        }
        else if constexpr (std::is_integral_v<T>)
        {
            parser.add_argument("-o").choices({T(23), T(34)}).template type<T>();

            CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "3"}), "argument -o: invalid choice: 3 (choose from 23, 34)", argparse::parsing_error);
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            parser.add_argument("-o").choices({T(0.125), T(1.5)}).template type<T>();

            CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "0.5"}), "argument -o: invalid choice: 0.5 (choose from 0.125, 1.5)", argparse::parsing_error);
        }
        else if constexpr (std::is_same_v<std::string, T>)
        {
            parser.add_argument("-o").choices({T("foo"), T("bar")}).template type<T>();

            CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "baz"}), "argument -o: invalid choice: \"baz\" (choose from \"foo\", \"bar\")", argparse::parsing_error);
        }
        else
        {
            parser.add_argument("-o").choices({T("foo"), T("bar")}).template type<T>();

            CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "baz"}), "argument -o: invalid choice: <Custom: baz> (choose from <Custom: foo>, <Custom: bar>)", argparse::parsing_error);
        }
    }
}

TEST_CASE_TEMPLATE("Parsing a positional argument with nargs set...", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    SUBCASE("...as a number...")
    {
        SUBCASE("...consumes the number of arguments...")
        {
            SUBCASE("...for one argument")
            {
                parser.add_argument("pos").nargs(1).template type<T>();

                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "A"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "42"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "0.5"}));
                }
                else
                {
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "foo"}));
                }
            }

            SUBCASE("...for two arguments")
            {
                parser.add_argument("pos").nargs(2).template type<T>();

                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "A", "C"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "42", "54"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "0.5", "1.125"}));
                }
                else
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "foo", "bar"}));
                }
            }

            SUBCASE("...for three arguments")
            {
                parser.add_argument("pos").nargs(3).template type<T>();

                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "A", "C", "E"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "42", "54", "65"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "0.5", "1.125", "2.375"}));
                }
                else
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "foo", "bar", "baz"}));
                }
            }
        }

        SUBCASE("...yields a list of arguments...")
        {
            SUBCASE("...for one argument")
            {
                parser.add_argument("pos").nargs(1).template type<T>();

                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "B"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{'B'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "42"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{42});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "0.5"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{0.5});
                }
                else
                {
                    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "foo"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{"foo"});
                }
            }

            SUBCASE("...for two arguments")
            {
                parser.add_argument("pos").nargs(2).template type<T>();

                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "B", "D"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{'B', 'D'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "42", "54"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{42, 54});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "0.5", "1.125"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{0.5, 1.125});
                }
                else
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "foo", "bar"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{"foo", "bar"});
                }
            }

            SUBCASE("...for three arguments")
            {
                parser.add_argument("pos").nargs(3).template type<T>();

                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "B", "D", "F"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{'B', 'D', 'F'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "42", "54", "65"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{42, 54, 65});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "0.5", "1.125", "2.375"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{0.5, 1.125, 2.375});
                }
                else
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "foo", "bar", "baz"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{"foo", "bar", "baz"});
                }
            }
        }

        SUBCASE("...throws an exception for incorrect arguments number...")
        {
            SUBCASE("...for one argument and no provided")
            {
                parser.add_argument("pos").nargs(1).template type<T>();

                CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: pos", argparse::parsing_error);
            }

            SUBCASE("...for two arguments and one provided")
            {
                parser.add_argument("pos").nargs(2).template type<T>();

                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "A"}), "the following arguments are required: pos", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "42"}), "the following arguments are required: pos", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "0.5"}), "the following arguments are required: pos", argparse::parsing_error);
                }
                else
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "foo"}), "the following arguments are required: pos", argparse::parsing_error);
                }
            }

            SUBCASE("...for three arguments and two provided")
            {
                parser.add_argument("pos").nargs(3).template type<T>();

                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "A", "B"}), "the following arguments are required: pos", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "42", "54"}), "the following arguments are required: pos", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "0.5", "1.125"}), "the following arguments are required: pos", argparse::parsing_error);
                }
                else
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "foo", "bar"}), "the following arguments are required: pos", argparse::parsing_error);
                }
            }

            SUBCASE("...for one argument and two provided")
            {
                parser.add_argument("pos").nargs(1).template type<T>();

                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "A", "B"}), "unrecognised arguments: B", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "42", "54"}), "unrecognised arguments: 54", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "0.5", "1.125"}), "unrecognised arguments: 1.125", argparse::parsing_error);
                }
                else
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "foo", "bar"}), "unrecognised arguments: bar", argparse::parsing_error);
                }
            }

            SUBCASE("...for one arguments and three provided")
            {
                parser.add_argument("pos").nargs(1).template type<T>();

                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "A", "B", "C"}), "unrecognised arguments: B C", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "42", "54", "65"}), "unrecognised arguments: 54 65", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "0.5", "1.125", "2.375"}), "unrecognised arguments: 1.125 2.375", argparse::parsing_error);
                }
                else
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "foo", "bar", "baz"}), "unrecognised arguments: bar baz", argparse::parsing_error);
                }
            }

            SUBCASE("...for two arguments and three provided")
            {
                parser.add_argument("pos").nargs(2).template type<T>();

                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "A", "B", "C"}), "unrecognised arguments: C", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "42", "54", "65"}), "unrecognised arguments: 65", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "0.5", "1.125", "2.375"}), "unrecognised arguments: 2.375", argparse::parsing_error);
                }
                else
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "foo", "bar", "baz"}), "unrecognised arguments: baz", argparse::parsing_error);
                }
            }
        }
    }

    SUBCASE("...as zero_or_one...")
    {
        SUBCASE("...consumes single argument and yields it as a single item")
        {
            parser.add_argument("pos").nargs(argparse::zero_or_one).template type<T>();

            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                auto const parsed = parser.parse_args(2, cstr_arr{"prog", "A"});

                CHECK(parsed.get_value<T>("pos") == T('A'));
            }
            else if constexpr (std::is_integral_v<T>)
            {
                auto const parsed = parser.parse_args(2, cstr_arr{"prog", "42"});

                CHECK(parsed.get_value<T>("pos") == T(42));
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                auto const parsed = parser.parse_args(2, cstr_arr{"prog", "0.5"});

                CHECK(parsed.get_value<T>("pos") == T(0.5));
            }
            else
            {
                auto const parsed = parser.parse_args(2, cstr_arr{"prog", "foo"});

                CHECK(parsed.get_value<T>("pos") == T("foo"));
            }
        }

        SUBCASE("...yields default value if no argument is provided")
        {
            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                parser.add_argument("pos").nargs(argparse::zero_or_one).default_(T('A'));

                auto const parsed = parser.parse_args(1, cstr_arr{"prog"});

                CHECK(parsed.get_value<T>("pos") == T('A'));
            }
            else if constexpr (std::is_integral_v<T>)
            {
                parser.add_argument("pos").nargs(argparse::zero_or_one).default_(T(10));

                auto const parsed = parser.parse_args(1, cstr_arr{"prog"});

                CHECK(parsed.get_value<T>("pos") == T(10));
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                parser.add_argument("pos").nargs(argparse::zero_or_one).default_(T(0.0625));

                auto const parsed = parser.parse_args(1, cstr_arr{"prog"});

                CHECK(parsed.get_value<T>("pos") == T(0.0625));
            }
            else
            {
                parser.add_argument("pos").nargs(argparse::zero_or_one).default_("foo"s);

                auto const parsed = parser.parse_args(1, cstr_arr{"prog"});

                CHECK(parsed.get_value<T>("pos") == T("foo"));
            }
        }
    }

    SUBCASE("...as zero_or_more...")
    {
        SUBCASE("...yields an empty list if no arguments provided")
        {
            parser.add_argument("pos").nargs(argparse::zero_or_more).template type<T>();

            auto const parsed = parser.parse_args(1, cstr_arr{"prog"});

            CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>());
        }

        SUBCASE("...consumes single argument and yields it as a list")
        {
            parser.add_argument("pos").nargs(argparse::zero_or_more).template type<T>();

            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                auto const parsed = parser.parse_args(2, cstr_arr{"prog", "A"});

                CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{'A'});
            }
            else if constexpr (std::is_integral_v<T>)
            {
                auto const parsed = parser.parse_args(2, cstr_arr{"prog", "42"});

                CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{42});
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                auto const parsed = parser.parse_args(2, cstr_arr{"prog", "0.5"});

                CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{0.5});
            }
            else
            {
                auto const parsed = parser.parse_args(2, cstr_arr{"prog", "foo"});

                CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{"foo"});
            }
        }

        SUBCASE("...consumes all available arguments and yields them as a list")
        {
            parser.add_argument("pos").nargs(argparse::zero_or_more).template type<T>();

            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                auto const parsed = parser.parse_args(4, cstr_arr{"prog", "A", "G", "J"});

                CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{'A', 'G', 'J'});
            }
            else if constexpr (std::is_integral_v<T>)
            {
                auto const parsed = parser.parse_args(4, cstr_arr{"prog", "42", "54", "65"});

                CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{42, 54, 65});
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                auto const parsed = parser.parse_args(4, cstr_arr{"prog", "0.5", "1.125", "2.375"});

                CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{0.5, 1.125, 2.375});
            }
            else
            {
                auto const parsed = parser.parse_args(4, cstr_arr{"prog", "foo", "bar", "baz"});

                CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{"foo", "bar", "baz"});
            }
        }
    }

    SUBCASE("...as one_or_more...")
    {
        SUBCASE("...throws an exception if no arguments provided")
        {
            parser.add_argument("pos").nargs(argparse::one_or_more).template type<T>();

            CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: pos", argparse::parsing_error);
        }

        SUBCASE("...consumes single argument and yields it as a list")
        {
            parser.add_argument("pos").nargs(argparse::one_or_more).template type<T>();

            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                auto const parsed = parser.parse_args(2, cstr_arr{"prog", "A"});

                CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{'A'});
            }
            else if constexpr (std::is_integral_v<T>)
            {
                auto const parsed = parser.parse_args(2, cstr_arr{"prog", "42"});

                CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{42});
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                auto const parsed = parser.parse_args(2, cstr_arr{"prog", "0.5"});

                CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{0.5});
            }
            else
            {
                auto const parsed = parser.parse_args(2, cstr_arr{"prog", "foo"});

                CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{"foo"});
            }
        }

        SUBCASE("...consumes all available arguments and yields them as a list")
        {
            parser.add_argument("pos").nargs(argparse::one_or_more).template type<T>();

            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                auto const parsed = parser.parse_args(4, cstr_arr{"prog", "A", "G", "J"});

                CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{'A', 'G', 'J'});
            }
            else if constexpr (std::is_integral_v<T>)
            {
                auto const parsed = parser.parse_args(4, cstr_arr{"prog", "42", "54", "65"});

                CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{42, 54, 65});
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                auto const parsed = parser.parse_args(4, cstr_arr{"prog", "0.5", "1.125", "2.375"});

                CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{0.5, 1.125, 2.375});
            }
            else
            {
                auto const parsed = parser.parse_args(4, cstr_arr{"prog", "foo", "bar", "baz"});

                CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{"foo", "bar", "baz"});
            }
        }
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with nargs set...", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    SUBCASE("...as a number...")
    {
        SUBCASE("...consumes the number of arguments...")
        {
            SUBCASE("...for one argument")
            {
                parser.add_argument("-o").nargs(1).template type<T>();

                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "A"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "42"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "0.5"}));
                }
                else
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "foo"}));
                }
            }

            SUBCASE("...for two arguments")
            {
                parser.add_argument("-o").nargs(2).template type<T>();

                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "A", "C"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "42", "54"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "0.5", "1.125"}));
                }
                else
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "foo", "bar"}));
                }
            }

            SUBCASE("...for three arguments")
            {
                parser.add_argument("-o").nargs(3).template type<T>();

                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "A", "C", "E"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "42", "54", "65"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "0.5", "1.125", "2.375"}));
                }
                else
                {
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "bar", "baz"}));
                }
            }
        }

        SUBCASE("...yields a list of arguments...")
        {
            SUBCASE("...for one argument")
            {
                parser.add_argument("-o").nargs(1).template type<T>();

                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "A"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{'A'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "42"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{42});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "0.5"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{0.5});
                }
                else
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "foo"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{"foo"});
                }
            }

            SUBCASE("...for two arguments")
            {
                parser.add_argument("-o").nargs(2).template type<T>();

                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "-o", "A", "C"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{'A', 'C'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "-o", "42", "54"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{42, 54});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "-o", "0.5", "1.125"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{0.5, 1.125});
                }
                else
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "-o", "foo", "bar"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{"foo", "bar"});
                }
            }

            SUBCASE("...for three arguments")
            {
                parser.add_argument("-o").nargs(3).template type<T>();

                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "A", "C", "E"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{'A', 'C', 'E'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "42", "54", "65"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{42, 54, 65});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "0.5", "1.125", "2.375"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{0.5, 1.125, 2.375});
                }
                else
                {
                    auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "bar", "baz"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{"foo", "bar", "baz"});
                }
            }
        }

        SUBCASE("...throws an exception for incorrect arguments number...")
        {
            SUBCASE("...for one argument and no provided")
            {
                parser.add_argument("-o").nargs(1).template type<T>();

                CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "-o"}), "argument -o: expected 1 argument", argparse::parsing_error);
            }

            SUBCASE("...for two arguments and one provided")
            {
                parser.add_argument("-o").nargs(2).template type<T>();

                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "A"}), "argument -o: expected 2 arguments", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "42"}), "argument -o: expected 2 arguments", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "0.5"}), "argument -o: expected 2 arguments", argparse::parsing_error);
                }
                else
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "foo"}), "argument -o: expected 2 arguments", argparse::parsing_error);
                }
            }

            SUBCASE("...for three arguments and two provided")
            {
                parser.add_argument("-o").nargs(3).template type<T>();

                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-o", "A", "B"}), "argument -o: expected 3 arguments", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-o", "42", "54"}), "argument -o: expected 3 arguments", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-o", "0.5", "1.125"}), "argument -o: expected 3 arguments", argparse::parsing_error);
                }
                else
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-o", "foo", "bar"}), "argument -o: expected 3 arguments", argparse::parsing_error);
                }
            }

            SUBCASE("...for one argument and two provided")
            {
                parser.add_argument("-o").nargs(1).template type<T>();

                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-o", "A", "B"}), "unrecognised arguments: B", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-o", "42", "54"}), "unrecognised arguments: 54", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-o", "0.5", "1.125"}), "unrecognised arguments: 1.125", argparse::parsing_error);
                }
                else
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-o", "foo", "bar"}), "unrecognised arguments: bar", argparse::parsing_error);
                }
            }

            SUBCASE("...for one argument and three provided")
            {
                parser.add_argument("-o").nargs(1).template type<T>();

                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-o", "A", "B", "C"}), "unrecognised arguments: B C", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-o", "42", "54", "65"}), "unrecognised arguments: 54 65", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-o", "0.5", "1.125", "2.375"}), "unrecognised arguments: 1.125 2.375", argparse::parsing_error);
                }
                else
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "bar", "baz"}), "unrecognised arguments: bar baz", argparse::parsing_error);
                }
            }

            SUBCASE("...for two arguments and three provided")
            {
                parser.add_argument("-o").nargs(2).template type<T>();

                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-o", "A", "B", "C"}), "unrecognised arguments: C", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-o", "42", "54", "65"}), "unrecognised arguments: 65", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-o", "0.5", "1.125", "2.375"}), "unrecognised arguments: 2.375", argparse::parsing_error);
                }
                else
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "bar", "baz"}), "unrecognised arguments: baz", argparse::parsing_error);
                }
            }
        }
    }

    SUBCASE("...as zero_or_one...")
    {
        SUBCASE("...consumes single argument and yields it as a single item")
        {
            parser.add_argument("-o").nargs(argparse::zero_or_one).template type<T>();

            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "A"});

                CHECK(parsed.get_value<T>("o") == T('A'));
            }
            else if constexpr (std::is_integral_v<T>)
            {
                auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "42"});

                CHECK(parsed.get_value<T>("o") == T(42));
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "0.5"});

                CHECK(parsed.get_value<T>("o") == T(0.5));
            }
            else
            {
                auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "foo"});

                CHECK(parsed.get_value<T>("o") == T("foo"));
            }
        }

        SUBCASE("...yields default value if no argument is provided")
        {
            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                parser.add_argument("-o").nargs(argparse::zero_or_one).default_(T('A'));

                auto const parsed = parser.parse_args(1, cstr_arr{"prog"});

                CHECK(parsed.get_value<T>("o") == T('A'));
            }
            else if constexpr (std::is_integral_v<T>)
            {
                parser.add_argument("-o").nargs(argparse::zero_or_one).default_(T(10));

                auto const parsed = parser.parse_args(1, cstr_arr{"prog"});

                CHECK(parsed.get_value<T>("o") == T(10));
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                parser.add_argument("-o").nargs(argparse::zero_or_one).default_(T(0.0625));

                auto const parsed = parser.parse_args(1, cstr_arr{"prog"});

                CHECK(parsed.get_value<T>("o") == T(0.0625));
            }
            else
            {
                parser.add_argument("-o").nargs(argparse::zero_or_one).default_("foo"s);

                auto const parsed = parser.parse_args(1, cstr_arr{"prog"});

                CHECK(parsed.get_value<T>("o") == "foo");
            }
        }

        SUBCASE("...yields const value if option string is present and no argument is provided")
        {
            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                parser.add_argument("-o").nargs(argparse::zero_or_one).const_(T('A'));

                auto const parsed = parser.parse_args(2, cstr_arr{"prog", "-o"});

                CHECK(parsed.get_value<T>("o") == T('A'));
            }
            else if constexpr (std::is_integral_v<T>)
            {
                parser.add_argument("-o").nargs(argparse::zero_or_one).const_(T(5));

                auto const parsed = parser.parse_args(2, cstr_arr{"prog", "-o"});

                CHECK(parsed.get_value<T>("o") == T(5));
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                parser.add_argument("-o").nargs(argparse::zero_or_one).const_(T(0.875));

                auto const parsed = parser.parse_args(2, cstr_arr{"prog", "-o"});

                CHECK(parsed.get_value<T>("o") == T(0.875));
            }
            else
            {
                parser.add_argument("-o").nargs(argparse::zero_or_one).const_("foo"s);

                auto const parsed = parser.parse_args(2, cstr_arr{"prog", "-o"});

                CHECK(parsed.get_value<T>("o") == "foo");
            }
        }
    }

    SUBCASE("...as zero_or_more...")
    {
        SUBCASE("...yields an empty list if no arguments provided")
        {
            parser.add_argument("-o").nargs(argparse::zero_or_more).template type<T>();

            auto const parsed = parser.parse_args(2, cstr_arr{"prog", "-o"});

            CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>());
        }

        SUBCASE("...consumes single argument and yields it as a list")
        {
            parser.add_argument("-o").nargs(argparse::zero_or_more).template type<T>();

            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "A"});

                CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{'A'});
            }
            else if constexpr (std::is_integral_v<T>)
            {
                auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "42"});

                CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{42});
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "0.5"});

                CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{0.5});
            }
            else
            {
                auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "foo"});

                CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{"foo"});
            }
        }

        SUBCASE("...consumes all available arguments and yields them as a list")
        {
            parser.add_argument("-o").nargs(argparse::zero_or_more).template type<T>();

            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "A", "C", "E"});

                CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{'A', 'C', 'E'});
            }
            else if constexpr (std::is_integral_v<T>)
            {
                auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "42", "54", "65"});

                CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{42, 54, 65});
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "0.5", "1.125", "2.375"});

                CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{0.5, 1.125, 2.375});
            }
            else
            {
                auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "bar", "baz"});

                CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{"foo", "bar", "baz"});
            }
        }
    }

    SUBCASE("...as one_or_more...")
    {
        SUBCASE("...throws an exception if no arguments provided")
        {
            parser.add_argument("-o").nargs(argparse::one_or_more).template type<T>();

            CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "-o"}), "argument -o: expected at least one argument", argparse::parsing_error);
        }

        SUBCASE("...consumes single argument and yields it as a list")
        {
            parser.add_argument("-o").nargs(argparse::one_or_more).template type<T>();

            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "A"});

                CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{'A'});
            }
            else if constexpr (std::is_integral_v<T>)
            {
                auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "42"});

                CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{42});
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "0.5"});

                CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{0.5});
            }
            else
            {
                auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "foo"});

                CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{"foo"});
            }
        }

        SUBCASE("...consumes all available arguments and yields them as a list")
        {
            parser.add_argument("-o").nargs(argparse::one_or_more).template type<T>();

            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "A", "C", "E"});

                CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{'A', 'C', 'E'});
            }
            else if constexpr (std::is_integral_v<T>)
            {
                auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "42", "54", "65"});

                CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{42, 54, 65});
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "0.5", "1.125", "2.375"});

                CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{0.5, 1.125, 2.375});
            }
            else
            {
                auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "bar", "baz"});

                CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{"foo", "bar", "baz"});
            }
        }
    }
}

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set...", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    SUBCASE("...consumes the number of arguments for nargs set...")
    {
        SUBCASE("...as a number...")
        {
            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("pos").choices({T('A'), T('C')}).nargs(1).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "A"}));
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "C"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("pos").choices({T(11), T(22)}).nargs(1).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "11"}));
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "22"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("pos").choices({T(0.125), T(0.25)}).nargs(1).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "0.125"}));
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "0.25"}));
                }
                else
                {
                    parser.add_argument("pos").choices({T("foo"), T("bar")}).nargs(1).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "foo"}));
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "bar"}));
                }
            }

            SUBCASE("...for two arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("pos").choices({T('A'), T('C')}).nargs(2).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "A", "A"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "C", "C"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("pos").choices({T(11), T(22)}).nargs(2).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "11", "11"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "22", "22"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("pos").choices({T(0.125), T(0.25)}).nargs(2).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "0.125", "0.125"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "0.25", "0.25"}));
                }
                else
                {
                    parser.add_argument("pos").choices({T("foo"), T("bar")}).nargs(2).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "foo", "foo"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "bar", "bar"}));
                }
            }

            SUBCASE("...for three arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("pos").choices({T('A'), T('C')}).nargs(3).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "A", "A", "A"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "C", "C", "C"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("pos").choices({T(11), T(22)}).nargs(3).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "11", "11", "11"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "22", "22", "22"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("pos").choices({T(0.125), T(0.25)}).nargs(3).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "0.125", "0.125", "0.125"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "0.25", "0.25", "0.25"}));
                }
                else
                {
                    parser.add_argument("pos").choices({T("foo"), T("bar")}).nargs(3).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "foo", "foo", "foo"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "bar", "bar", "bar"}));
                }
            }
        }

        SUBCASE("...as zero_or_one...")
        {
            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("pos").choices({T('A'), T('C')}).nargs(argparse::zero_or_one).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "A"}));
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "C"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("pos").choices({T(11), T(22)}).nargs(argparse::zero_or_one).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "11"}));
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "22"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("pos").choices({T(0.125), T(0.25)}).nargs(argparse::zero_or_one).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "0.125"}));
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "0.25"}));
                }
                else
                {
                    parser.add_argument("pos").choices({T("foo"), T("bar")}).nargs(argparse::zero_or_one).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "foo"}));
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "bar"}));
                }
            }
        }

        SUBCASE("...as zero_or_more...")
        {
            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                parser.add_argument("pos").choices({T('A'), T('C')}).nargs(argparse::zero_or_more).template type<T>();
            }
            else if constexpr (std::is_integral_v<T>)
            {
                parser.add_argument("pos").choices({T(11), T(22)}).nargs(argparse::zero_or_more).template type<T>();
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                parser.add_argument("pos").choices({T(0.125), T(0.25)}).nargs(argparse::zero_or_more).template type<T>();
            }
            else
            {
                parser.add_argument("pos").choices({T("foo"), T("bar")}).nargs(argparse::zero_or_more).template type<T>();
            }

            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "A"}));
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "C"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "11"}));
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "22"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "0.125"}));
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "0.25"}));
                }
                else
                {
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "foo"}));
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "bar"}));
                }
            }

            SUBCASE("...for two arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "A", "A"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "C", "C"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "11", "11"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "22", "22"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "0.125", "0.125"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "0.25", "0.25"}));
                }
                else
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "foo", "foo"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "bar", "bar"}));
                }
            }

            SUBCASE("...for three arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "A", "A", "A"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "C", "C", "C"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "11", "11", "11"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "22", "22", "22"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "0.125", "0.125", "0.125"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "0.25", "0.25", "0.25"}));
                }
                else
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "foo", "foo", "foo"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "bar", "bar", "bar"}));
                }
            }
        }

        SUBCASE("...as one_or_more...")
        {
            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                parser.add_argument("pos").choices({T('A'), T('C')}).nargs(argparse::one_or_more).template type<T>();
            }
            else if constexpr (std::is_integral_v<T>)
            {
                parser.add_argument("pos").choices({T(11), T(22)}).nargs(argparse::one_or_more).template type<T>();
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                parser.add_argument("pos").choices({T(0.125), T(0.25)}).nargs(argparse::one_or_more).template type<T>();
            }
            else
            {
                parser.add_argument("pos").choices({T("foo"), T("bar")}).nargs(argparse::one_or_more).template type<T>();
            }

            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "A"}));
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "C"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "11"}));
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "22"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "0.125"}));
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "0.25"}));
                }
                else
                {
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "foo"}));
                    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "bar"}));
                }
            }

            SUBCASE("...for two arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "A", "A"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "C", "C"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "11", "11"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "22", "22"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "0.125", "0.125"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "0.25", "0.25"}));
                }
                else
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "foo", "foo"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "bar", "bar"}));
                }
            }

            SUBCASE("...for three arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "A", "A", "A"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "C", "C", "C"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "11", "11", "11"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "22", "22", "22"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "0.125", "0.125", "0.125"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "0.25", "0.25", "0.25"}));
                }
                else
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "foo", "foo", "foo"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "bar", "bar", "bar"}));
                }
            }
        }
    }

    SUBCASE("...yields the arguments for nargs set...")
    {
        SUBCASE("...as a number...")
        {
            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("pos").choices({T('A'), T('C')}).nargs(1).template type<T>();

                    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "A"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{'A'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("pos").choices({T(11), T(22)}).nargs(1).template type<T>();

                    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "11"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{11});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("pos").choices({T(0.125), T(0.25)}).nargs(1).template type<T>();

                    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "0.125"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{0.125});
                }
                else
                {
                    parser.add_argument("pos").choices({T("foo"), T("bar")}).nargs(1).template type<T>();

                    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "foo"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{"foo"});
                }
            }

            SUBCASE("...for two arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("pos").choices({T('A'), T('C')}).nargs(2).template type<T>();

                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "A", "C"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{'A', 'C'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("pos").choices({T(11), T(22)}).nargs(2).template type<T>();

                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "11", "22"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{11, 22});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("pos").choices({T(0.125), T(0.25)}).nargs(2).template type<T>();

                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "0.125", "0.25"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{0.125, 0.25});
                }
                else
                {
                    parser.add_argument("pos").choices({T("foo"), T("bar")}).nargs(2).template type<T>();

                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "foo", "bar"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{"foo", "bar"});
                }
            }

            SUBCASE("...for three arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("pos").choices({T('A'), T('C')}).nargs(3).template type<T>();

                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "A", "C", "A"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{'A', 'C', 'A'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("pos").choices({T(11), T(22)}).nargs(3).template type<T>();

                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "11", "22", "11"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{11, 22, 11});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("pos").choices({T(0.125), T(0.25)}).nargs(3).template type<T>();

                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "0.125", "0.25", "0.125"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{0.125, 0.25, 0.125});
                }
                else
                {
                    parser.add_argument("pos").choices({T("foo"), T("bar")}).nargs(3).template type<T>();

                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "foo", "bar", "foo"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{"foo", "bar", "foo"});
                }
            }
        }

        SUBCASE("...as zero_or_one...")
        {
            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("pos").choices({T('A'), T('C')}).nargs(argparse::zero_or_one).template type<T>();

                    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "A"});

                    CHECK(parsed.get_value<T>("pos") == T('A'));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("pos").choices({T(11), T(22)}).nargs(argparse::zero_or_one).template type<T>();

                    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "11"});

                    CHECK(parsed.get_value<T>("pos") == T(11));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("pos").choices({T(0.125), T(0.25)}).nargs(argparse::zero_or_one).template type<T>();

                    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "0.125"});

                    CHECK(parsed.get_value<T>("pos") == T(0.125));
                }
                else
                {
                    parser.add_argument("pos").choices({T("foo"), T("bar")}).nargs(argparse::zero_or_one).template type<T>();

                    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "foo"});

                    CHECK(parsed.get_value<T>("pos") == T("foo"));
                }
            }
        }

        SUBCASE("...as zero_or_more...")
        {
            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                parser.add_argument("pos").choices({T('A'), T('C')}).nargs(argparse::zero_or_more).template type<T>();
            }
            else if constexpr (std::is_integral_v<T>)
            {
                parser.add_argument("pos").choices({T(11), T(22)}).nargs(argparse::zero_or_more).template type<T>();
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                parser.add_argument("pos").choices({T(0.125), T(0.25)}).nargs(argparse::zero_or_more).template type<T>();
            }
            else
            {
                parser.add_argument("pos").choices({T("foo"), T("bar")}).nargs(argparse::zero_or_more).template type<T>();
            }

            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "A"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{'A'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "11"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{11});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "0.125"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{0.125});
                }
                else
                {
                    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "foo"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{"foo"});
                }
            }

            SUBCASE("...for two arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "A", "A"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{'A', 'A'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "11", "11"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{11, 11});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "0.125", "0.125"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{0.125, 0.125});
                }
                else
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "foo", "foo"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{"foo", "foo"});
                }
            }

            SUBCASE("...for three arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "A", "A", "A"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{'A', 'A', 'A'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "11", "11", "11"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{11, 11, 11});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "0.125", "0.125", "0.125"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{0.125, 0.125, 0.125});
                }
                else
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "foo", "foo", "foo"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{"foo", "foo", "foo"});
                }
            }
        }

        SUBCASE("...as one_or_more...")
        {
            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                parser.add_argument("pos").choices({T('A'), T('C')}).nargs(argparse::one_or_more).template type<T>();
            }
            else if constexpr (std::is_integral_v<T>)
            {
                parser.add_argument("pos").choices({T(11), T(22)}).nargs(argparse::one_or_more).template type<T>();
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                parser.add_argument("pos").choices({T(0.125), T(0.25)}).nargs(argparse::one_or_more).template type<T>();
            }
            else
            {
                parser.add_argument("pos").choices({T("foo"), T("bar")}).nargs(argparse::one_or_more).template type<T>();
            }

            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "A"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{'A'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "11"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{11});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "0.125"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{0.125});
                }
                else
                {
                    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "foo"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{"foo"});
                }
            }

            SUBCASE("...for two arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "A", "A"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{'A', 'A'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "11", "11"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{11, 11});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "0.125", "0.125"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{0.125, 0.125});
                }
                else
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "foo", "foo"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{"foo", "foo"});
                }
            }

            SUBCASE("...for three arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "A", "A", "A"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{'A', 'A', 'A'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "11", "11", "11"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{11, 11, 11});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "0.125", "0.125", "0.125"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{0.125, 0.125, 0.125});
                }
                else
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "foo", "foo", "foo"});

                    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>{"foo", "foo", "foo"});
                }
            }
        }
    }

    SUBCASE("...throws an exception on incorrect value for nargs set...")
    {
        SUBCASE("...as a number...")
        {
            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("pos").choices({T('A'), T('C')}).nargs(1).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "B"}), "argument pos: invalid choice: B (choose from A, C)", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("pos").choices({T(11), T(22)}).nargs(1).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "33"}), "argument pos: invalid choice: 33 (choose from 11, 22)", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("pos").choices({T(0.125), T(0.25)}).nargs(1).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "0.5"}), "argument pos: invalid choice: 0.5 (choose from 0.125, 0.25)", argparse::parsing_error);
                }
                else
                {
                    parser.add_argument("pos").choices({T("foo"), T("bar")}).nargs(1).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "baz"}), "argument pos: invalid choice: \"baz\" (choose from \"foo\", \"bar\")", argparse::parsing_error);
                }
            }

            SUBCASE("...for two arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("pos").choices({T('A'), T('C')}).nargs(2).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "A", "B"}), "argument pos: invalid choice: B (choose from A, C)", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("pos").choices({T(11), T(22)}).nargs(2).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "11", "33"}), "argument pos: invalid choice: 33 (choose from 11, 22)", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("pos").choices({T(0.125), T(0.25)}).nargs(2).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "0.125", "0.5"}), "argument pos: invalid choice: 0.5 (choose from 0.125, 0.25)", argparse::parsing_error);
                }
                else
                {
                    parser.add_argument("pos").choices({T("foo"), T("bar")}).nargs(2).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "foo", "baz"}), "argument pos: invalid choice: \"baz\" (choose from \"foo\", \"bar\")", argparse::parsing_error);
                }
            }

            SUBCASE("...for three arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("pos").choices({T('A'), T('C')}).nargs(3).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "A", "C", "B"}), "argument pos: invalid choice: B (choose from A, C)", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("pos").choices({T(11), T(22)}).nargs(3).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "11", "22", "33"}), "argument pos: invalid choice: 33 (choose from 11, 22)", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("pos").choices({T(0.125), T(0.25)}).nargs(3).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "0.125", "0.25", "0.5"}), "argument pos: invalid choice: 0.5 (choose from 0.125, 0.25)", argparse::parsing_error);
                }
                else
                {
                    parser.add_argument("pos").choices({T("foo"), T("bar")}).nargs(3).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "foo", "bar", "baz"}), "argument pos: invalid choice: \"baz\" (choose from \"foo\", \"bar\")", argparse::parsing_error);
                }
            }
        }

        SUBCASE("...as zero_or_one...")
        {
            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("pos").choices({T('A'), T('C')}).nargs(argparse::zero_or_one).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "B"}), "argument pos: invalid choice: B (choose from A, C)", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("pos").choices({T(11), T(22)}).nargs(argparse::zero_or_one).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "33"}), "argument pos: invalid choice: 33 (choose from 11, 22)", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("pos").choices({T(0.125), T(0.25)}).nargs(argparse::zero_or_one).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "0.5"}), "argument pos: invalid choice: 0.5 (choose from 0.125, 0.25)", argparse::parsing_error);
                }
                else
                {
                    parser.add_argument("pos").choices({T("foo"), T("bar")}).nargs(argparse::zero_or_one).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "baz"}), "argument pos: invalid choice: \"baz\" (choose from \"foo\", \"bar\")", argparse::parsing_error);
                }
            }
        }

        SUBCASE("...as zero_or_more...")
        {
            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                parser.add_argument("pos").choices({T('A'), T('C')}).nargs(argparse::zero_or_more).template type<T>();
            }
            else if constexpr (std::is_integral_v<T>)
            {
                parser.add_argument("pos").choices({T(11), T(22)}).nargs(argparse::zero_or_more).template type<T>();
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                parser.add_argument("pos").choices({T(0.125), T(0.25)}).nargs(argparse::zero_or_more).template type<T>();
            }
            else
            {
                parser.add_argument("pos").choices({T("foo"), T("bar")}).nargs(argparse::zero_or_more).template type<T>();
            }

            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "B"}), "argument pos: invalid choice: B (choose from A, C)", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "33"}), "argument pos: invalid choice: 33 (choose from 11, 22)", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "0.5"}), "argument pos: invalid choice: 0.5 (choose from 0.125, 0.25)", argparse::parsing_error);
                }
                else
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "baz"}), "argument pos: invalid choice: \"baz\" (choose from \"foo\", \"bar\")", argparse::parsing_error);
                }
            }

            SUBCASE("...for two arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "A", "B"}), "argument pos: invalid choice: B (choose from A, C)", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "11", "33"}), "argument pos: invalid choice: 33 (choose from 11, 22)", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "0.125", "0.5"}), "argument pos: invalid choice: 0.5 (choose from 0.125, 0.25)", argparse::parsing_error);
                }
                else
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "foo", "baz"}), "argument pos: invalid choice: \"baz\" (choose from \"foo\", \"bar\")", argparse::parsing_error);
                }
            }

            SUBCASE("...for three arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "A", "C", "B"}), "argument pos: invalid choice: B (choose from A, C)", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "11", "22", "33"}), "argument pos: invalid choice: 33 (choose from 11, 22)", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "0.125", "0.25", "0.5"}), "argument pos: invalid choice: 0.5 (choose from 0.125, 0.25)", argparse::parsing_error);
                }
                else
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "foo", "bar", "baz"}), "argument pos: invalid choice: \"baz\" (choose from \"foo\", \"bar\")", argparse::parsing_error);
                }
            }
        }

        SUBCASE("...as one_or_more...")
        {
            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                parser.add_argument("pos").choices({T('A'), T('C')}).nargs(argparse::one_or_more).template type<T>();
            }
            else if constexpr (std::is_integral_v<T>)
            {
                parser.add_argument("pos").choices({T(11), T(22)}).nargs(argparse::one_or_more).template type<T>();
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                parser.add_argument("pos").choices({T(0.125), T(0.25)}).nargs(argparse::one_or_more).template type<T>();
            }
            else
            {
                parser.add_argument("pos").choices({T("foo"), T("bar")}).nargs(argparse::one_or_more).template type<T>();
            }

            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "B"}), "argument pos: invalid choice: B (choose from A, C)", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "33"}), "argument pos: invalid choice: 33 (choose from 11, 22)", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "0.5"}), "argument pos: invalid choice: 0.5 (choose from 0.125, 0.25)", argparse::parsing_error);
                }
                else
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "baz"}), "argument pos: invalid choice: \"baz\" (choose from \"foo\", \"bar\")", argparse::parsing_error);
                }
            }

            SUBCASE("...for two arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "A", "B"}), "argument pos: invalid choice: B (choose from A, C)", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "11", "33"}), "argument pos: invalid choice: 33 (choose from 11, 22)", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "0.125", "0.5"}), "argument pos: invalid choice: 0.5 (choose from 0.125, 0.25)", argparse::parsing_error);
                }
                else
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "foo", "baz"}), "argument pos: invalid choice: \"baz\" (choose from \"foo\", \"bar\")", argparse::parsing_error);
                }
            }

            SUBCASE("...for three arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "A", "C", "B"}), "argument pos: invalid choice: B (choose from A, C)", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "11", "22", "33"}), "argument pos: invalid choice: 33 (choose from 11, 22)", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "0.125", "0.25", "0.5"}), "argument pos: invalid choice: 0.5 (choose from 0.125, 0.25)", argparse::parsing_error);
                }
                else
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "foo", "bar", "baz"}), "argument pos: invalid choice: \"baz\" (choose from \"foo\", \"bar\")", argparse::parsing_error);
                }
            }
        }
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set...", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    SUBCASE("...consumes the number of arguments for nargs set...")
    {
        SUBCASE("...as a number...")
        {
            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("-o").choices({T('A'), T('C')}).nargs(1).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "A"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "C"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("-o").choices({T(11), T(22)}).nargs(1).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "11"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "22"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("-o").choices({T(0.125), T(0.25)}).nargs(1).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "0.125"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "0.25"}));
                }
                else
                {
                    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(1).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "foo"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "bar"}));
                }
            }

            SUBCASE("...for two arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("-o").choices({T('A'), T('C')}).nargs(2).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "A", "A"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "C", "C"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("-o").choices({T(11), T(22)}).nargs(2).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "11", "11"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "22", "22"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("-o").choices({T(0.125), T(0.25)}).nargs(2).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "0.125", "0.125"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "0.25", "0.25"}));
                }
                else
                {
                    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(2).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "foo", "foo"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "bar", "bar"}));
                }
            }

            SUBCASE("...for three arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("-o").choices({T('A'), T('C')}).nargs(3).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "A", "A", "A"}));
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "C", "C", "C"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("-o").choices({T(11), T(22)}).nargs(3).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "11", "11", "11"}));
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "22", "22", "22"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("-o").choices({T(0.125), T(0.25)}).nargs(3).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "0.125", "0.125", "0.125"}));
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "0.25", "0.25", "0.25"}));
                }
                else
                {
                    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(3).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "foo", "foo"}));
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "bar", "bar", "bar"}));
                }
            }
        }

        SUBCASE("...as zero_or_one...")
        {
            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("-o").choices({T('A'), T('C')}).nargs(argparse::zero_or_one).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "A"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "C"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("-o").choices({T(11), T(22)}).nargs(argparse::zero_or_one).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "11"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "22"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("-o").choices({T(0.125), T(0.25)}).nargs(argparse::zero_or_one).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "0.125"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "0.25"}));
                }
                else
                {
                    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_one).template type<T>();

                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "foo"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "bar"}));
                }
            }
        }

        SUBCASE("...as zero_or_more...")
        {
            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                parser.add_argument("-o").choices({T('A'), T('C')}).nargs(argparse::zero_or_more).template type<T>();
            }
            else if constexpr (std::is_integral_v<T>)
            {
                parser.add_argument("-o").choices({T(11), T(22)}).nargs(argparse::zero_or_more).template type<T>();
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                parser.add_argument("-o").choices({T(0.125), T(0.25)}).nargs(argparse::zero_or_more).template type<T>();
            }
            else
            {
                parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_more).template type<T>();
            }

            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "A"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "C"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "11"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "22"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "0.125"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "0.25"}));
                }
                else
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "foo"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "bar"}));
                }
            }

            SUBCASE("...for two arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "A", "A"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "C", "C"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "11", "11"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "22", "22"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "0.125", "0.125"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "0.25", "0.25"}));
                }
                else
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "foo", "foo"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "bar", "bar"}));
                }
            }

            SUBCASE("...for three arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "A", "A", "A"}));
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "C", "C", "C"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "11", "11", "11"}));
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "22", "22", "22"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "0.125", "0.125", "0.125"}));
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "0.25", "0.25", "0.25"}));
                }
                else
                {
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "foo", "foo"}));
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "bar", "bar", "bar"}));
                }
            }
        }

        SUBCASE("...as one_or_more...")
        {
            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                parser.add_argument("-o").choices({T('A'), T('C')}).nargs(argparse::one_or_more).template type<T>();
            }
            else if constexpr (std::is_integral_v<T>)
            {
                parser.add_argument("-o").choices({T(11), T(22)}).nargs(argparse::one_or_more).template type<T>();
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                parser.add_argument("-o").choices({T(0.125), T(0.25)}).nargs(argparse::one_or_more).template type<T>();
            }
            else
            {
                parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(argparse::one_or_more).template type<T>();
            }

            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "A"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "C"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "11"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "22"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "0.125"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "0.25"}));
                }
                else
                {
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "foo"}));
                    CHECK_NOTHROW(parser.parse_args(3, cstr_arr{"prog", "-o", "bar"}));
                }
            }

            SUBCASE("...for two arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "A", "A"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "C", "C"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "11", "11"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "22", "22"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "0.125", "0.125"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "0.25", "0.25"}));
                }
                else
                {
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "foo", "foo"}));
                    CHECK_NOTHROW(parser.parse_args(4, cstr_arr{"prog", "-o", "bar", "bar"}));
                }
            }

            SUBCASE("...for three arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "A", "A", "A"}));
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "C", "C", "C"}));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "11", "11", "11"}));
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "22", "22", "22"}));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "0.125", "0.125", "0.125"}));
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "0.25", "0.25", "0.25"}));
                }
                else
                {
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "foo", "foo"}));
                    CHECK_NOTHROW(parser.parse_args(5, cstr_arr{"prog", "-o", "bar", "bar", "bar"}));
                }
            }
        }
    }

    SUBCASE("...yields the arguments for nargs set...")
    {
        SUBCASE("...as a number...")
        {
            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("-o").choices({T('A'), T('C')}).nargs(1).template type<T>();

                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "A"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{'A'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("-o").choices({T(11), T(22)}).nargs(1).template type<T>();

                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "11"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{11});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("-o").choices({T(0.125), T(0.25)}).nargs(1).template type<T>();

                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "0.125"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{0.125});
                }
                else
                {
                    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(1).template type<T>();

                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "foo"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{"foo"});
                }
            }

            SUBCASE("...for two arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("-o").choices({T('A'), T('C')}).nargs(2).template type<T>();

                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "-o", "A", "C"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{'A', 'C'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("-o").choices({T(11), T(22)}).nargs(2).template type<T>();

                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "-o", "11", "22"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{11, 22});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("-o").choices({T(0.125), T(0.25)}).nargs(2).template type<T>();

                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "-o", "0.125", "0.25"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{0.125, 0.25});
                }
                else
                {
                    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(2).template type<T>();

                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "-o", "foo", "bar"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{"foo", "bar"});
                }
            }

            SUBCASE("...for three arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("-o").choices({T('A'), T('C')}).nargs(3).template type<T>();

                    auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "A", "C", "A"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{'A', 'C', 'A'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("-o").choices({T(11), T(22)}).nargs(3).template type<T>();

                    auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "11", "22", "11"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{11, 22, 11});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("-o").choices({T(0.125), T(0.25)}).nargs(3).template type<T>();

                    auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "0.125", "0.25", "0.125"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{0.125, 0.25, 0.125});
                }
                else
                {
                    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(3).template type<T>();

                    auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "bar", "foo"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{"foo", "bar", "foo"});
                }
            }
        }

        SUBCASE("...as zero_or_one...")
        {
            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("-o").choices({T('A'), T('C')}).nargs(argparse::zero_or_one).template type<T>();

                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "A"});

                    CHECK(parsed.get_value<T>("o") == T('A'));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("-o").choices({T(11), T(22)}).nargs(argparse::zero_or_one).template type<T>();

                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "11"});

                    CHECK(parsed.get_value<T>("o") == T(11));
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("-o").choices({T(0.125), T(0.25)}).nargs(argparse::zero_or_one).template type<T>();

                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "0.125"});

                    CHECK(parsed.get_value<T>("o") == T(0.125));
                }
                else
                {
                    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_one).template type<T>();

                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "foo"});

                    CHECK(parsed.get_value<T>("o") == T("foo"));
                }
            }
        }

        SUBCASE("...as zero_or_more...")
        {
            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                parser.add_argument("-o").choices({T('A'), T('C')}).nargs(argparse::zero_or_more).template type<T>();
            }
            else if constexpr (std::is_integral_v<T>)
            {
                parser.add_argument("-o").choices({T(11), T(22)}).nargs(argparse::zero_or_more).template type<T>();
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                parser.add_argument("-o").choices({T(0.125), T(0.25)}).nargs(argparse::zero_or_more).template type<T>();
            }
            else
            {
                parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_more).template type<T>();
            }

            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "A"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{'A'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "11"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{11});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "0.125"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{0.125});
                }
                else
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "foo"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{"foo"});
                }
            }

            SUBCASE("...for two arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "-o", "A", "A"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{'A', 'A'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "-o", "11", "11"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{11, 11});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "-o", "0.125", "0.125"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{0.125, 0.125});
                }
                else
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "-o", "foo", "foo"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{"foo", "foo"});
                }
            }

            SUBCASE("...for three arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "A", "A", "A"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{'A', 'A', 'A'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "11", "11", "11"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{11, 11, 11});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "0.125", "0.125", "0.125"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{0.125, 0.125, 0.125});
                }
                else
                {
                    auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "foo", "foo"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{"foo", "foo", "foo"});
                }
            }
        }

        SUBCASE("...as one_or_more...")
        {
            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                parser.add_argument("-o").choices({T('A'), T('C')}).nargs(argparse::one_or_more).template type<T>();
            }
            else if constexpr (std::is_integral_v<T>)
            {
                parser.add_argument("-o").choices({T(11), T(22)}).nargs(argparse::one_or_more).template type<T>();
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                parser.add_argument("-o").choices({T(0.125), T(0.25)}).nargs(argparse::one_or_more).template type<T>();
            }
            else
            {
                parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(argparse::one_or_more).template type<T>();
            }

            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "A"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{'A'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "11"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{11});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "0.125"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{0.125});
                }
                else
                {
                    auto const parsed = parser.parse_args(3, cstr_arr{"prog", "-o", "foo"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{"foo"});
                }
            }

            SUBCASE("...for two arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "-o", "A", "A"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{'A', 'A'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "-o", "11", "11"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{11, 11});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "-o", "0.125", "0.125"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{0.125, 0.125});
                }
                else
                {
                    auto const parsed = parser.parse_args(4, cstr_arr{"prog", "-o", "foo", "foo"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{"foo", "foo"});
                }
            }

            SUBCASE("...for three arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "A", "A", "A"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{'A', 'A', 'A'});
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "11", "11", "11"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{11, 11, 11});
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "0.125", "0.125", "0.125"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{0.125, 0.125, 0.125});
                }
                else
                {
                    auto const parsed = parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "foo", "foo"});

                    CHECK(parsed.get_value<std::vector<T>>("o") == std::vector<T>{"foo", "foo", "foo"});
                }
            }
        }
    }

    SUBCASE("...throws an exception on incorrect value for nargs set...")
    {
        SUBCASE("...as a number...")
        {
            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("-o").choices({T('A'), T('C')}).nargs(1).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "B"}), "argument -o: invalid choice: B (choose from A, C)", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("-o").choices({T(11), T(22)}).nargs(1).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "33"}), "argument -o: invalid choice: 33 (choose from 11, 22)", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("-o").choices({T(0.125), T(0.25)}).nargs(1).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "0.5"}), "argument -o: invalid choice: 0.5 (choose from 0.125, 0.25)", argparse::parsing_error);
                }
                else
                {
                    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(1).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "baz"}), "argument -o: invalid choice: \"baz\" (choose from \"foo\", \"bar\")", argparse::parsing_error);
                }
            }

            SUBCASE("...for two arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("-o").choices({T('A'), T('C')}).nargs(2).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-o", "A", "B"}), "argument -o: invalid choice: B (choose from A, C)", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("-o").choices({T(11), T(22)}).nargs(2).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-o", "11", "33"}), "argument -o: invalid choice: 33 (choose from 11, 22)", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("-o").choices({T(0.125), T(0.25)}).nargs(2).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-o", "0.125", "0.5"}), "argument -o: invalid choice: 0.5 (choose from 0.125, 0.25)", argparse::parsing_error);
                }
                else
                {
                    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(2).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-o", "foo", "baz"}), "argument -o: invalid choice: \"baz\" (choose from \"foo\", \"bar\")", argparse::parsing_error);
                }
            }

            SUBCASE("...for three arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("-o").choices({T('A'), T('C')}).nargs(3).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-o", "A", "C", "B"}), "argument -o: invalid choice: B (choose from A, C)", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("-o").choices({T(11), T(22)}).nargs(3).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-o", "11", "22", "33"}), "argument -o: invalid choice: 33 (choose from 11, 22)", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("-o").choices({T(0.125), T(0.25)}).nargs(3).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-o", "0.125", "0.25", "0.5"}), "argument -o: invalid choice: 0.5 (choose from 0.125, 0.25)", argparse::parsing_error);
                }
                else
                {
                    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(3).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "bar", "baz"}), "argument -o: invalid choice: \"baz\" (choose from \"foo\", \"bar\")", argparse::parsing_error);
                }
            }
        }

        SUBCASE("...as zero_or_one...")
        {
            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    parser.add_argument("-o").choices({T('A'), T('C')}).nargs(argparse::zero_or_one).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "B"}), "argument -o: invalid choice: B (choose from A, C)", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    parser.add_argument("-o").choices({T(11), T(22)}).nargs(argparse::zero_or_one).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "33"}), "argument -o: invalid choice: 33 (choose from 11, 22)", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    parser.add_argument("-o").choices({T(0.125), T(0.25)}).nargs(argparse::zero_or_one).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "0.5"}), "argument -o: invalid choice: 0.5 (choose from 0.125, 0.25)", argparse::parsing_error);
                }
                else
                {
                    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_one).template type<T>();

                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "baz"}), "argument -o: invalid choice: \"baz\" (choose from \"foo\", \"bar\")", argparse::parsing_error);
                }
            }
        }

        SUBCASE("...as zero_or_more...")
        {
            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                parser.add_argument("-o").choices({T('A'), T('C')}).nargs(argparse::zero_or_more).template type<T>();
            }
            else if constexpr (std::is_integral_v<T>)
            {
                parser.add_argument("-o").choices({T(11), T(22)}).nargs(argparse::zero_or_more).template type<T>();
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                parser.add_argument("-o").choices({T(0.125), T(0.25)}).nargs(argparse::zero_or_more).template type<T>();
            }
            else
            {
                parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_more).template type<T>();
            }

            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "B"}), "argument -o: invalid choice: B (choose from A, C)", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "33"}), "argument -o: invalid choice: 33 (choose from 11, 22)", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "0.5"}), "argument -o: invalid choice: 0.5 (choose from 0.125, 0.25)", argparse::parsing_error);
                }
                else
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "baz"}), "argument -o: invalid choice: \"baz\" (choose from \"foo\", \"bar\")", argparse::parsing_error);
                }
            }

            SUBCASE("...for two arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-o", "A", "B"}), "argument -o: invalid choice: B (choose from A, C)", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-o", "11", "33"}), "argument -o: invalid choice: 33 (choose from 11, 22)", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-o", "0.125", "0.5"}), "argument -o: invalid choice: 0.5 (choose from 0.125, 0.25)", argparse::parsing_error);
                }
                else
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-o", "foo", "baz"}), "argument -o: invalid choice: \"baz\" (choose from \"foo\", \"bar\")", argparse::parsing_error);
                }
            }

            SUBCASE("...for three arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-o", "A", "C", "B"}), "argument -o: invalid choice: B (choose from A, C)", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-o", "11", "22", "33"}), "argument -o: invalid choice: 33 (choose from 11, 22)", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-o", "0.125", "0.25", "0.5"}), "argument -o: invalid choice: 0.5 (choose from 0.125, 0.25)", argparse::parsing_error);
                }
                else
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "bar", "baz"}), "argument -o: invalid choice: \"baz\" (choose from \"foo\", \"bar\")", argparse::parsing_error);
                }
            }
        }

        SUBCASE("...as one_or_more...")
        {
            if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
            {
                parser.add_argument("-o").choices({T('A'), T('C')}).nargs(argparse::one_or_more).template type<T>();
            }
            else if constexpr (std::is_integral_v<T>)
            {
                parser.add_argument("-o").choices({T(11), T(22)}).nargs(argparse::one_or_more).template type<T>();
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                parser.add_argument("-o").choices({T(0.125), T(0.25)}).nargs(argparse::one_or_more).template type<T>();
            }
            else
            {
                parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(argparse::one_or_more).template type<T>();
            }

            SUBCASE("...for one argument")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "B"}), "argument -o: invalid choice: B (choose from A, C)", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "33"}), "argument -o: invalid choice: 33 (choose from 11, 22)", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "0.5"}), "argument -o: invalid choice: 0.5 (choose from 0.125, 0.25)", argparse::parsing_error);
                }
                else
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "baz"}), "argument -o: invalid choice: \"baz\" (choose from \"foo\", \"bar\")", argparse::parsing_error);
                }
            }

            SUBCASE("...for two arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-o", "A", "B"}), "argument -o: invalid choice: B (choose from A, C)", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-o", "11", "33"}), "argument -o: invalid choice: 33 (choose from 11, 22)", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-o", "0.125", "0.5"}), "argument -o: invalid choice: 0.5 (choose from 0.125, 0.25)", argparse::parsing_error);
                }
                else
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-o", "foo", "baz"}), "argument -o: invalid choice: \"baz\" (choose from \"foo\", \"bar\")", argparse::parsing_error);
                }
            }

            SUBCASE("...for three arguments")
            {
                if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-o", "A", "C", "B"}), "argument -o: invalid choice: B (choose from A, C)", argparse::parsing_error);
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-o", "11", "22", "33"}), "argument -o: invalid choice: 33 (choose from 11, 22)", argparse::parsing_error);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-o", "0.125", "0.25", "0.5"}), "argument -o: invalid choice: 0.5 (choose from 0.125, 0.25)", argparse::parsing_error);
                }
                else
                {
                    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "bar", "baz"}), "argument -o: invalid choice: \"baz\" (choose from \"foo\", \"bar\")", argparse::parsing_error);
                }
            }
        }
    }
}

TEST_CASE("Parsing missing positional argument with nargs set throws an exception...")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    SUBCASE("...for one missing argument")
    {
        parser.add_argument("p1").nargs(2);

        CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: p1", argparse::parsing_error);
    }

    SUBCASE("...for two missing arguments")
    {
        parser.add_argument("p1").nargs(2);
        parser.add_argument("p2").nargs(2);

        CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: p1 p2", argparse::parsing_error);
    }

    SUBCASE("...for three missing arguments")
    {
        parser.add_argument("p1").nargs(2);
        parser.add_argument("p2").nargs(2);
        parser.add_argument("p3").nargs(2);

        CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: p1 p2 p3", argparse::parsing_error);
    }
}

TEST_CASE("An optional argument does not consume another optional argument...")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    SUBCASE("...for simple argument")
    {
        parser.add_argument("-o");
        parser.add_argument("-p");

        CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "-p"}), "argument -o: expected one argument", argparse::parsing_error);
    }

    SUBCASE("...for argument with nargs set as number...")
    {
        SUBCASE("...1")
        {
            parser.add_argument("-o").nargs(1);
            parser.add_argument("-p");

            CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "-p"}), "argument -o: expected 1 argument", argparse::parsing_error);
        }

        SUBCASE("...2")
        {
            parser.add_argument("-o").nargs(2);
            parser.add_argument("-p");

            CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-o", "foo", "-p"}), "argument -o: expected 2 arguments", argparse::parsing_error);
        }

        SUBCASE("...3")
        {
            parser.add_argument("-o").nargs(3);
            parser.add_argument("-p");

            CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "bar", "-p"}), "argument -o: expected 3 arguments", argparse::parsing_error);
        }
    }

    SUBCASE("...for argument with nargs set as zero_or_one")
    {
        parser.add_argument("-o").nargs(argparse::zero_or_one);
        parser.add_argument("-p");

        CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "-p"}), "argument -p: expected one argument", argparse::parsing_error);
    }

    SUBCASE("...for argument with nargs set as zero_or_more")
    {
        parser.add_argument("-o").nargs(argparse::zero_or_more);
        parser.add_argument("-p");

        CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "-p"}), "argument -p: expected one argument", argparse::parsing_error);
    }

    SUBCASE("...for argument with nargs set as one_or_more")
    {
        parser.add_argument("-o").nargs(argparse::one_or_more);
        parser.add_argument("-p");

        CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "-p"}), "argument -o: expected at least one argument", argparse::parsing_error);
    }
}

TEST_CASE("Parsing -- pseudo argument does not throw")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "--"}));
}

TEST_CASE("Arguments past the -- pseudo argument are treated as positional...")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    SUBCASE("...for optional arguments")
    {
        parser.add_argument("-o").action(argparse::store_true);

        CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "--", "-o"}), "unrecognised arguments: -o", argparse::parsing_error);
    }

    SUBCASE("...for positional arguments")
    {
        parser.add_argument("pos");

        auto const parsed =parser.parse_args(3, cstr_arr{"prog", "--", "-o"});

        CHECK(parsed.get_value("pos") == "-o");
    }
}

TEST_CASE("An optional argument does not consume arguments past the -- pseudo argument...")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    SUBCASE("...for simple argument")
    {
        parser.add_argument("-o");

        CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "--"}), "argument -o: expected one argument", argparse::parsing_error);
    }

    SUBCASE("...for argument with nargs set as number...")
    {
        SUBCASE("...1")
        {
            parser.add_argument("-o").nargs(1);

            CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "--"}), "argument -o: expected 1 argument", argparse::parsing_error);
        }

        SUBCASE("...2")
        {
            parser.add_argument("-o").nargs(2);

            CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "--"}), "argument -o: expected 2 arguments", argparse::parsing_error);
        }

        SUBCASE("...3")
        {
            parser.add_argument("-o").nargs(3);

            CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "--"}), "argument -o: expected 3 arguments", argparse::parsing_error);
        }
    }

    SUBCASE("...for argument with nargs set as zero_or_one")
    {
        parser.add_argument("-o").nargs(argparse::zero_or_one);

        auto args = parser.parse_args(3, cstr_arr{"prog", "-o", "--"});

        CHECK(!args.get("o"));
    }

    SUBCASE("...for argument with nargs set as zero_or_more")
    {
        parser.add_argument("-o").nargs(argparse::zero_or_more);

        auto args = parser.parse_args(3, cstr_arr{"prog", "-o", "--"});

        CHECK(args.get_value<std::vector<std::string>>("o") == std::vector<std::string>());
    }

    SUBCASE("...for argument with nargs set as one_or_more")
    {
        parser.add_argument("-o").nargs(argparse::one_or_more);

        CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "--"}), "argument -o: expected at least one argument", argparse::parsing_error);
    }

    SUBCASE("...for argument with nargs set as zero_or_more and followed by a positional argument")
    {
        parser.add_argument("-o").nargs(argparse::zero_or_more);
        parser.add_argument("pos");

        auto args = parser.parse_args(4, cstr_arr{"prog", "-o", "--", "val"});

        CHECK(args.get_value<std::vector<std::string>>("o") == std::vector<std::string>());
        CHECK(args.get_value("pos") == "val");
    }
}

TEST_CASE("Parsing joined short options does not throw...")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    SUBCASE("...for arguments with store true action")
    {
        parser.add_argument("-a").action(argparse::store_true);
        parser.add_argument("-b").action(argparse::store_true);

        CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-ab"}));
    }

    SUBCASE("...for arguments with store false action")
    {
        parser.add_argument("-a").action(argparse::store_false);
        parser.add_argument("-b").action(argparse::store_false);

        CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-ab"}));
    }

    SUBCASE("...for arguments with store const action")
    {
        parser.add_argument("-a").action(argparse::store_const).const_(10);
        parser.add_argument("-b").action(argparse::store_const).const_(20);

        CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-ab"}));
    }
}

TEST_CASE("Parsing joined short options with store true action yields true for each of them")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-a").action(argparse::store_true);
    parser.add_argument("-b").action(argparse::store_true);

    auto args = parser.parse_args(2, cstr_arr{"prog", "-ab"});

    CHECK(args.get_value<bool>("a") == true);
    CHECK(args.get_value<bool>("b") == true);
}

TEST_CASE("Parsing joined short options with store false action yields false for each of them")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-a").action(argparse::store_false);
    parser.add_argument("-b").action(argparse::store_false);

    auto args = parser.parse_args(2, cstr_arr{"prog", "-ab"});

    CHECK(args.get_value<bool>("a") == false);
    CHECK(args.get_value<bool>("b") == false);
}

TEST_CASE("Parsing joined short options with store const action yields const value")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-a").action(argparse::store_const).const_(10);
    parser.add_argument("-b").action(argparse::store_const).const_(20);

    auto args = parser.parse_args(2, cstr_arr{"prog", "-ab"});

    CHECK(args.get_value<int>("a") == 10);
    CHECK(args.get_value<int>("b") == 20);
}

TEST_CASE("Parsing long option with joined argument does not throw")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("--long-opt");

    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "--long-opt=value"}));
}

TEST_CASE("Parsing long option with joined argument yields its value")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("--long");

    auto args = parser.parse_args(2, cstr_arr{"prog", "--long=value"});

    CHECK(args.get_value("long") == "value");
}

TEST_CASE("Parsing long options with same prefix correctly recognises them")
{
    auto parser = argparse::ArgumentParser();

    SUBCASE("plane options")
    {
        parser.add_argument("--same").action(argparse::store_true);
        parser.add_argument("--same-prefix").action(argparse::store_true);

        auto args = parser.parse_args(2, cstr_arr{"prog", "--same-prefix"});

        CHECK(args.get_value<bool>("same") == false);
    }

    SUBCASE("options with joined argument")
    {
        parser.add_argument("--same");
        parser.add_argument("--same-prefix");

        auto args = parser.parse_args(2, cstr_arr{"prog", "--same-prefix=value"});

        CHECK(!args.get("same"));
    }
}

TEST_CASE("Parsing short option with joined argument does not throw")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o");

    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-ovalue"}));
}

TEST_CASE("Parsing short option with joined argument yields its value")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o");

    auto args = parser.parse_args(2, cstr_arr{"prog", "-ovalue"});

    CHECK(args.get_value("o") == "value");
}

TEST_CASE("Parsing joined short options and short option joined with argument does not throw")
{
    auto parser = argparse::ArgumentParser();

    SUBCASE("order ao")
    {
        parser.add_argument("-a").action(argparse::store_true);
        parser.add_argument("-o");

        CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-aovalue"}));
    }

    SUBCASE("order oa")
    {
        parser.add_argument("-o");
        parser.add_argument("-a").action(argparse::store_true);

        CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-aovalue"}));
    }
}

TEST_CASE("Parsing joined short options and short option joined with argument yields their values")
{
    auto parser = argparse::ArgumentParser();

    SUBCASE("order ao")
    {
        parser.add_argument("-a").action(argparse::store_true);
        parser.add_argument("-o");

        auto args = parser.parse_args(2, cstr_arr{"prog", "-aovalue"});

        CHECK(args.get_value<bool>("a") == true);
        CHECK(args.get_value("o") == "value");
    }

    SUBCASE("order oa")
    {
        parser.add_argument("-o");
        parser.add_argument("-a").action(argparse::store_true);

        auto args = parser.parse_args(2, cstr_arr{"prog", "-aovalue"});

        CHECK(args.get_value<bool>("a") == true);
        CHECK(args.get_value("o") == "value");
    }
}

TEST_CASE("Parsing long options does not affect short options")
{
    auto parser = argparse::ArgumentParser().add_help(false);
    parser.add_argument("-b").action(argparse::store_true);
    parser.add_argument("--bar").action(argparse::store_true);

    auto args = parser.parse_args(2, cstr_arr{"prog", "--bar"});

    CHECK(args.get_value<bool>("b") == false);
    CHECK(args.get_value<bool>("bar") == true);
}
