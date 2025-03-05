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
inline auto operator==(Custom const & lhs, Custom const & rhs) -> bool
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

TEST_CASE("Parsing a positional argument with invalid value throws an exception")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("pos").type<int>();

    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "not-a-number"}), "argument pos: invalid value: 'not-a-number'", argparse::parsing_error);
    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "10gibberish"}), "argument pos: invalid value: '10gibberish'", argparse::parsing_error);
}

TEST_CASE("Parsing missing positional argument throws an exception for one missing argument")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("p1");

    CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: p1", argparse::parsing_error);
}

TEST_CASE("Parsing missing positional argument throws an exception for two missing arguments")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("p1");
    parser.add_argument("p2");

    CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: p1 p2", argparse::parsing_error);
}

TEST_CASE("Parsing missing positional argument throws an exception for three missing arguments")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("p1");
    parser.add_argument("p2");
    parser.add_argument("p3");

    CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: p1 p2 p3", argparse::parsing_error);
}

TEST_CASE("Parsing unrecognised positional argument throws an exception for one unrecognised argument")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("p1");

    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "v1", "v2"}), "unrecognised arguments: v2", argparse::parsing_error);
}

TEST_CASE("Parsing unrecognised positional argument throws an exception for two unrecognised arguments")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("p1");

    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "v1", "v2", "v3"}), "unrecognised arguments: v2 v3", argparse::parsing_error);
}

TEST_CASE("Parsing unrecognised positional argument throws an exception for three unrecognised arguments")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("p1");
    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "v1", "v2", "v3", "v4"}), "unrecognised arguments: v2 v3 v4", argparse::parsing_error);
}

TEST_CASE("The resulting attribute name for positional argument is based on its name")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("foo");

    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "val"});

    CHECK(parsed.get("foo"));
}

TEST_CASE("The resulting attribute name for positional argument is based on dest parameter")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("foo").dest("bar");

    auto const parsed = parser.parse_args(2, cstr_arr{"prog", "val"});

    CHECK(parsed.get("bar"));
}

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set accepts one of the values", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string, foo::Custom, bar::Custom)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set throws an exception on incorrect value", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string, foo::Custom, bar::Custom)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with nargs set as a number consumes the number of arguments for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing a positional argument with nargs set as a number consumes the number of arguments for two arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing a positional argument with nargs set as a number consumes the number of arguments for three arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing a positional argument with nargs set as a number yields a list of arguments for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing a positional argument with nargs set as a number yields a list of arguments for two arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing a positional argument with nargs set as a number yields a list of arguments for three arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing a positional argument with nargs set as a number throws an exception for incorrect arguments number for one argument and no provided", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("pos").nargs(1).template type<T>();

    CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: pos", argparse::parsing_error);
}

TEST_CASE_TEMPLATE("Parsing a positional argument with nargs set as a number throws an exception for incorrect arguments number for two arguments and one provided", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing a positional argument with nargs set as a number throws an exception for incorrect arguments number for three arguments and two provided", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing a positional argument with nargs set as a number throws an exception for incorrect arguments number for one argument and two provided", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing a positional argument with nargs set as a number throws an exception for incorrect arguments number for one arguments and three provided", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing a positional argument with nargs set as a number throws an exception for incorrect arguments number for two arguments and three provided", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing a positional argument with nargs set as zero_or_one consumes single argument and yields it as a single item", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing a positional argument with nargs set as zero_or_one yields default value if no argument is provided", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with nargs set as zero_or_more yields an empty list if no arguments provided", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("pos").nargs(argparse::zero_or_more).template type<T>();

    auto const parsed = parser.parse_args(1, cstr_arr{"prog"});

    CHECK(parsed.get_value<std::vector<T>>("pos") == std::vector<T>());
}

TEST_CASE_TEMPLATE("Parsing a positional argument with nargs set as zero_or_more consumes single argument and yields it as a list", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing a positional argument with nargs set as zero_or_more consumes all available arguments and yields them as a list", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing a positional argument with nargs set as one_or_more throws an exception if no arguments provided", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("pos").nargs(argparse::one_or_more).template type<T>();

    CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: pos", argparse::parsing_error);
}

TEST_CASE_TEMPLATE("Parsing a positional argument with nargs set as one_or_more consumes single argument and yields it as a list", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing a positional argument with nargs set as one_or_more consumes all available arguments and yields them as a list", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set consumes the number of arguments for nargs set as a number for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set consumes the number of arguments for nargs set as a number for two arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set consumes the number of arguments for nargs set as a number for three arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set consumes the number of arguments for nargs set as zero_or_one for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set consumes the number of arguments for nargs set as zero_or_more for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set consumes the number of arguments for nargs set as zero_or_more for two arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set consumes the number of arguments for nargs set as zero_or_more for three arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set consumes the number of arguments for nargs set as one_or_more for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set consumes the number of arguments for nargs set as one_or_more for two arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set consumes the number of arguments for nargs set as one_or_more for three arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set yields the arguments for nargs set as a number for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set yields the arguments for nargs set as a number for two arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set yields the arguments for nargs set as a number for three arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set yields the arguments for nargs set as zero_or_one for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set yields the arguments for nargs set as zero_or_more for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set yields the arguments for nargs set as zero_or_more for two arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set yields the arguments for nargs set as zero_or_more for three arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set yields the arguments for nargs set as one_or_more for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set yields the arguments for nargs set as one_or_more for two arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set yields the arguments for nargs set as one_or_more for three arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set throws an exception on incorrect value for nargs set as a number for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set throws an exception on incorrect value for nargs set as a number for two arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set throws an exception on incorrect value for nargs set as a number for three arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set throws an exception on incorrect value for nargs set as zero_or_one for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set throws an exception on incorrect value for nargs set as zero_or_more for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set throws an exception on incorrect value for nargs set as zero_or_more for two arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set throws an exception on incorrect value for nargs set as zero_or_more for three arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set throws an exception on incorrect value for nargs set as one_or_more for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set throws an exception on incorrect value for nargs set as one_or_more for two arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing a positional argument with choices set throws an exception on incorrect value for nargs set as one_or_more for three arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE("Parsing missing positional argument with nargs set throws an exception for one missing argument")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("p1").nargs(2);

    CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: p1", argparse::parsing_error);
}

TEST_CASE("Parsing missing positional argument with nargs set throws an exception for two missing arguments")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("p1").nargs(2);
    parser.add_argument("p2").nargs(2);

    CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: p1 p2", argparse::parsing_error);
}

TEST_CASE("Parsing missing positional argument with nargs set throws an exception for three missing arguments")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("p1").nargs(2);
    parser.add_argument("p2").nargs(2);
    parser.add_argument("p3").nargs(2);

    CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: p1 p2 p3", argparse::parsing_error);
}
