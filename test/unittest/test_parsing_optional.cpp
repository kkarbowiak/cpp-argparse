#include "argparse.hpp"

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

TEST_CASE("Parsing an optional argument yields false when it's missing")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o");

    auto const args = parser.parse_args(1, cstr_arr{"prog"});

    CHECK(!args.get("o"));
}

TEST_CASE("Parsing an optional argument throws an exception when it's missing argument")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o");

    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "-o"}), "argument -o: expected one argument", argparse::parsing_error);
}

TEST_CASE("Parsing an optional argument yields its value")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o");

    auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "v1"});

    CHECK(args.get_value("o") == "v1");
}

TEST_CASE("Parsing an optional argument with store true action yields false when it's missing")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o").action(argparse::store_true);

    auto const args = parser.parse_args(1, cstr_arr{"prog"});

    CHECK(args.get_value<bool>("o") == false);
}

TEST_CASE("Parsing an optional argument with store true action yields true when it's present")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o").action(argparse::store_true);

    auto const args = parser.parse_args(2, cstr_arr{"prog", "-o"});

    CHECK(args.get_value<bool>("o") == true);
}

TEST_CASE("Parsing an optional argument with store false action yields true when it's missing")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o").action(argparse::store_false);

    auto const args = parser.parse_args(1, cstr_arr{"prog"});

    CHECK(args.get_value<bool>("o") == true);
}

TEST_CASE("Parsing an optional argument with store false action yields false when it's present")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o").action(argparse::store_false);

    auto const args = parser.parse_args(2, cstr_arr{"prog", "-o"});

    CHECK(args.get_value<bool>("o") == false);
}

TEST_CASE_TEMPLATE("Parsing an optional argument with store const action yields false when it's missing", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string, foo::Custom, bar::Custom)
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

    auto const args = parser.parse_args(1, cstr_arr{"prog"});

    CHECK(!args.get("o"));
}

TEST_CASE_TEMPLATE("Parsing an optional argument with store const action yields const value", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string, foo::Custom, bar::Custom)
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

    auto const args = parser.parse_args(2, cstr_arr{"prog", "-o"});

    if constexpr (std::is_integral_v<T>)
    {
        CHECK(args.get_value<T>("o") == T(65));
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        CHECK(args.get_value<T>("o") == T(1.125));
    }
    else
    {
        CHECK(args.get_value<T>("o") == T("bar"));
    }
}

TEST_CASE("Parsing an optional argument with count action yields false when it's missing")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-c").action(argparse::count);

    auto const args = parser.parse_args(1, cstr_arr{"prog"});

    CHECK(!args.get("c"));
}

TEST_CASE("Parsing an optional argument with count action yields default value when it's missing")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-c").action(argparse::count).default_(0);

    auto const args = parser.parse_args(1, cstr_arr{"prog"});

    CHECK(args.get_value<int>("c") == 0);
}

TEST_CASE("Parsing an optional argument with count action yields the argument count for one argument")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-c").action(argparse::count);

    auto const args = parser.parse_args(2, cstr_arr{"prog", "-c"});

    CHECK(args.get_value<int>("c") == 1);
}

TEST_CASE("Parsing an optional argument with count action yields the argument count for two arguments")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-c").action(argparse::count);

    auto const args = parser.parse_args(3, cstr_arr{"prog", "-c", "-c"});

    CHECK(args.get_value<int>("c") == 2);
}

TEST_CASE("Parsing an optional argument with count action yields the argument count for three arguments")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-c").action(argparse::count);

    auto const args = parser.parse_args(4, cstr_arr{"prog", "-c", "-c", "-c"});

    CHECK(args.get_value<int>("c") == 3);
}

TEST_CASE("Parsing an optional argument with append action yields false when it's missing")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-a").action(argparse::append);

    auto const args = parser.parse_args(1, cstr_arr{"prog"});

    CHECK(!args.get("a"));
}

TEST_CASE("Parsing an optional argument with append action throws an exception when it's missing argument")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-a").action(argparse::append);

    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "-a"}), "argument -a: expected one argument", argparse::parsing_error);
}

TEST_CASE("Parsing an optional argument with append action yields a list of arguments for one argument")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-a").action(argparse::append);

    auto const args = parser.parse_args(3, cstr_arr{"prog", "-a", "one"});

    CHECK(args.get_value<std::vector<std::string>>("a") == std::vector<std::string>{"one"});
}

TEST_CASE("Parsing an optional argument with append action yields a list of arguments for two arguments")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-a").action(argparse::append);

    auto const args = parser.parse_args(5, cstr_arr{"prog", "-a", "one", "-a", "two"});

    CHECK(args.get_value<std::vector<std::string>>("a") == std::vector<std::string>{"one", "two"});
}

TEST_CASE("Parsing an optional argument with append action yields a list of arguments for three arguments")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-a").action(argparse::append);

    auto const args = parser.parse_args(7, cstr_arr{"prog", "-a", "one", "-a", "two", "-a", "three"});

    CHECK(args.get_value<std::vector<std::string>>("a") == std::vector<std::string>{"one", "two", "three"});
}

TEST_CASE_TEMPLATE("Parsing an optional argument with append action yields its requested type", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, foo::Custom, bar::Custom)
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-a").action(argparse::append).type<T>();

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-a", "A"});

        CHECK(args.get_value<std::vector<T>>("a") == std::vector{T(65)});
    }
    else if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-a", "65"});

        CHECK(args.get_value<std::vector<T>>("a") == std::vector{T(65)});
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-a", "1.125"});

        CHECK(args.get_value<std::vector<T>>("a") == std::vector{T(1.125)});
    }
    else
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-a", "bar"});

        CHECK(args.get_value<std::vector<T>>("a") == std::vector{T("bar")});
    }
}

TEST_CASE("Parsing an optional argument with help action yields false when it's missing")
{
    auto parser = argparse::ArgumentParser().add_help(false).handle(argparse::Handle::none);
    parser.add_argument("-h").action(argparse::help);

    auto const args = parser.parse_args(1, cstr_arr{"prog"});

    CHECK(args.get_value<bool>("h") == false);
}

TEST_CASE("Parsing an optional argument with help action yields true when it's present")
{
    auto parser = argparse::ArgumentParser().add_help(false).handle(argparse::Handle::none);
    parser.add_argument("-h").action(argparse::help);

    auto const args = parser.parse_args(2, cstr_arr{"prog", "-h"});

    CHECK(args.get_value<bool>("h") == true);
}

TEST_CASE("Parsing an optional argument with version action yields false when it's missing")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-v").action(argparse::version);

    auto const args = parser.parse_args(1, cstr_arr{"prog"});

    CHECK(args.get_value<bool>("v") == false);
}

TEST_CASE("Parsing an optional argument with version action yields true when it's present")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-v").action(argparse::version);

    auto const args = parser.parse_args(2, cstr_arr{"prog", "-v"});

    CHECK(args.get_value<bool>("v") == true);
}

TEST_CASE("Optional argument can be used with its short name")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o", "--option");

    auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "val"});

    CHECK(args.get_value("option") == "val");
}

TEST_CASE("Optional argument can be used with its long name")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o", "--option");

    auto const args = parser.parse_args(3, cstr_arr{"prog", "--option", "val"});

    CHECK(args.get_value("option") == "val");
}

TEST_CASE_TEMPLATE("Parsing an optional argument yields its requested type", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, foo::Custom, bar::Custom)
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o").type<T>();

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "A"});

        CHECK(args.get_value<T>("o") == T(65));
    }
    else if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "65"});

        CHECK(args.get_value<T>("o") == T(65));
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "1.125"});

        CHECK(args.get_value<T>("o") == T(1.125));
    }
    else
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "bar"});

        CHECK(args.get_value<T>("o") == T("bar"));
    }
}

TEST_CASE("Parsing an optional argument with invalid value throws an exception")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").type<int>();

    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "not-a-number"}), "argument -o: invalid value: 'not-a-number'", argparse::parsing_error);
    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "10gibberish"}), "argument -o: invalid value: '10gibberish'", argparse::parsing_error);
}

TEST_CASE_TEMPLATE("Parsing an optional argument with default value yields the default value when it's missing", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string, foo::Custom, bar::Custom)
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

    auto const args = parser.parse_args(1, cstr_arr{"prog"});

    if constexpr (std::is_integral_v<T>)
    {
        CHECK(args.get_value<T>("o") == T(54));
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        CHECK(args.get_value<T>("o") == T(0.125));
    }
    else
    {
        CHECK(args.get_value<T>("o") == T("foo"));
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with default value yields value of the argument's type", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string, foo::Custom, bar::Custom)
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

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "A"});

        CHECK(args.get_value<T>("o") == T(65));
    }
    else if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "65"});

        CHECK(args.get_value<T>("o") == T(65));
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "1.125"});

        CHECK(args.get_value<T>("o") == T(1.125));
    }
    else
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "bar"});

        CHECK(args.get_value<T>("o") == T("bar"));
    }
}

TEST_CASE("Parsing unrecognised optional argument throws an exception for one unrecognised argument")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-a");

    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-a", "v1", "-b"}), "unrecognised arguments: -b", argparse::parsing_error);
}

TEST_CASE("Parsing unrecognised optional argument throws an exception for two unrecognised arguments")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-a");

    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-a", "v1", "-b", "-c"}), "unrecognised arguments: -b -c", argparse::parsing_error);
}

TEST_CASE("Parsing unrecognised optional argument throws an exception for three unrecognised arguments")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-a");

    CHECK_THROWS_WITH_AS(parser.parse_args(6, cstr_arr{"prog", "-a", "v1", "-b", "-c", "-d"}), "unrecognised arguments: -b -c -d", argparse::parsing_error);
}

TEST_CASE("The resulting attribute name for optional argument is based on its long name")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-f", "--foo");

    auto const args = parser.parse_args(3, cstr_arr{"prog", "-f", "val"});

    CHECK(args.get("foo"));
}

TEST_CASE("The resulting attribute name for optional argument is based on its short name")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-f");

    auto const args = parser.parse_args(3, cstr_arr{"prog", "-f", "val"});

    CHECK(args.get("f"));
}

TEST_CASE("The resulting attribute name for optional argument is based on dest parameter")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-f", "--foo").dest("bar");

    auto const args = parser.parse_args(3, cstr_arr{"prog", "-f", "val"});

    CHECK(args.get("bar"));
}

TEST_CASE("Parsing a missing optional argument with required true throws an exception")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").required(true);

    CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: -o", argparse::parsing_error);
}

TEST_CASE("Parsing a missing optional argument with required false does not throw")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").required(false);

    CHECK_NOTHROW(parser.parse_args(1, cstr_arr{"prog"}));
}

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set accepts one of the values", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string, foo::Custom, bar::Custom)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set throws an exception on incorrect value", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string, foo::Custom, bar::Custom)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing an optional argument with nargs set as a number consumes the number of arguments for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing an optional argument with nargs set as a number consumes the number of arguments for two arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing an optional argument with nargs set as a number consumes the number of arguments for three arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing an optional argument with nargs set as a number yields a list of arguments for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(1).template type<T>();

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "A"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{'A'});
    }
    else if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "42"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{42});
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "0.5"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{0.5});
    }
    else
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "foo"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{"foo"});
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with nargs set as a number yields a list of arguments for two arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(2).template type<T>();

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        auto const args = parser.parse_args(4, cstr_arr{"prog", "-o", "A", "C"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{'A', 'C'});
    }
    else if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(4, cstr_arr{"prog", "-o", "42", "54"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{42, 54});
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        auto const args = parser.parse_args(4, cstr_arr{"prog", "-o", "0.5", "1.125"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{0.5, 1.125});
    }
    else
    {
        auto const args = parser.parse_args(4, cstr_arr{"prog", "-o", "foo", "bar"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{"foo", "bar"});
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with nargs set as a number yields a list of arguments for three arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(3).template type<T>();

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "A", "C", "E"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{'A', 'C', 'E'});
    }
    else if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "42", "54", "65"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{42, 54, 65});
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "0.5", "1.125", "2.375"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{0.5, 1.125, 2.375});
    }
    else
    {
        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "bar", "baz"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{"foo", "bar", "baz"});
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with nargs set as a number throws an exception for incorrect arguments number for one argument and no provided", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(1).template type<T>();

    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "-o"}), "argument -o: expected 1 argument", argparse::parsing_error);
}

TEST_CASE_TEMPLATE("Parsing an optional argument with nargs set as a number throws an exception for incorrect arguments number for two arguments and one provided", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing an optional argument with nargs set as a number throws an exception for incorrect arguments number for three arguments and two provided", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing an optional argument with nargs set as a number throws an exception for incorrect arguments number for one argument and two provided", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing an optional argument with nargs set as a number throws an exception for incorrect arguments number for one argument and three provided", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing an optional argument with nargs set as a number throws an exception for incorrect arguments number for two arguments and three provided", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
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

TEST_CASE_TEMPLATE("Parsing an optional argument with nargs set as zero_or_one consumes single argument and yields it as a single item", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(argparse::zero_or_one).template type<T>();

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "A"});

        CHECK(args.get_value<T>("o") == T('A'));
    }
    else if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "42"});

        CHECK(args.get_value<T>("o") == T(42));
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "0.5"});

        CHECK(args.get_value<T>("o") == T(0.5));
    }
    else
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "foo"});

        CHECK(args.get_value<T>("o") == T("foo"));
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with nargs set as zero_or_one yields default value if no argument is provided", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        parser.add_argument("-o").nargs(argparse::zero_or_one).default_(T('A'));

        auto const args = parser.parse_args(1, cstr_arr{"prog"});

        CHECK(args.get_value<T>("o") == T('A'));
    }
    else if constexpr (std::is_integral_v<T>)
    {
        parser.add_argument("-o").nargs(argparse::zero_or_one).default_(T(10));

        auto const args = parser.parse_args(1, cstr_arr{"prog"});

        CHECK(args.get_value<T>("o") == T(10));
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        parser.add_argument("-o").nargs(argparse::zero_or_one).default_(T(0.0625));

        auto const args = parser.parse_args(1, cstr_arr{"prog"});

        CHECK(args.get_value<T>("o") == T(0.0625));
    }
    else
    {
        parser.add_argument("-o").nargs(argparse::zero_or_one).default_("foo"s);

        auto const args = parser.parse_args(1, cstr_arr{"prog"});

        CHECK(args.get_value<T>("o") == "foo");
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with nargs set as zero_or_one yields const value if option string is present and no argument is provided", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        parser.add_argument("-o").nargs(argparse::zero_or_one).const_(T('A'));

        auto const args = parser.parse_args(2, cstr_arr{"prog", "-o"});

        CHECK(args.get_value<T>("o") == T('A'));
    }
    else if constexpr (std::is_integral_v<T>)
    {
        parser.add_argument("-o").nargs(argparse::zero_or_one).const_(T(5));

        auto const args = parser.parse_args(2, cstr_arr{"prog", "-o"});

        CHECK(args.get_value<T>("o") == T(5));
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        parser.add_argument("-o").nargs(argparse::zero_or_one).const_(T(0.875));

        auto const args = parser.parse_args(2, cstr_arr{"prog", "-o"});

        CHECK(args.get_value<T>("o") == T(0.875));
    }
    else
    {
        parser.add_argument("-o").nargs(argparse::zero_or_one).const_("foo"s);

        auto const args = parser.parse_args(2, cstr_arr{"prog", "-o"});

        CHECK(args.get_value<T>("o") == "foo");
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with nargs set as zero_or_more yields an empty list if no arguments provided", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(argparse::zero_or_more).template type<T>();

    auto const args = parser.parse_args(2, cstr_arr{"prog", "-o"});

    CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>());
}

TEST_CASE_TEMPLATE("Parsing an optional argument with nargs set as zero_or_more consumes single argument and yields it as a list", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(argparse::zero_or_more).template type<T>();

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "A"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{'A'});
    }
    else if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "42"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{42});
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "0.5"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{0.5});
    }
    else
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "foo"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{"foo"});
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with nargs set as zero_or_more consumes all available arguments and yields them as a list", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(argparse::zero_or_more).template type<T>();

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "A", "C", "E"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{'A', 'C', 'E'});
    }
    else if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "42", "54", "65"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{42, 54, 65});
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "0.5", "1.125", "2.375"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{0.5, 1.125, 2.375});
    }
    else
    {
        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "bar", "baz"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{"foo", "bar", "baz"});
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with nargs set as one_or_more throws an exception if no arguments provided", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(argparse::one_or_more).template type<T>();

    CHECK_THROWS_WITH_AS(parser.parse_args(2, cstr_arr{"prog", "-o"}), "argument -o: expected at least one argument", argparse::parsing_error);
}

TEST_CASE_TEMPLATE("Parsing an optional argument with nargs set as one_or_more consumes single argument and yields it as a list", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(argparse::one_or_more).template type<T>();

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "A"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{'A'});
    }
    else if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "42"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{42});
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "0.5"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{0.5});
    }
    else
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "foo"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{"foo"});
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with nargs set as one_or_more consumes all available arguments and yields them as a list", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(argparse::one_or_more).template type<T>();

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "A", "C", "E"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{'A', 'C', 'E'});
    }
    else if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "42", "54", "65"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{42, 54, 65});
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "0.5", "1.125", "2.375"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{0.5, 1.125, 2.375});
    }
    else
    {
        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "bar", "baz"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{"foo", "bar", "baz"});
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set consumes the number of arguments for nargs set as a number for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set consumes the number of arguments for nargs set as a number for two arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set consumes the number of arguments for nargs set as a number for three arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set consumes the number of arguments for nargs set as zero_or_one for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set consumes the number of arguments for nargs set as zero_or_more for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set consumes the number of arguments for nargs set as zero_or_more for two arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set consumes the number of arguments for nargs set as zero_or_more for three arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set consumes the number of arguments for nargs set as one_or_more for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set consumes the number of arguments for nargs set as one_or_more for two arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set consumes the number of arguments for nargs set as one_or_more for three arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set yields the arguments for nargs set as a number for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        parser.add_argument("-o").choices({T('A'), T('C')}).nargs(1).template type<T>();

        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "A"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{'A'});
    }
    else if constexpr (std::is_integral_v<T>)
    {
        parser.add_argument("-o").choices({T(11), T(22)}).nargs(1).template type<T>();

        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "11"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{11});
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        parser.add_argument("-o").choices({T(0.125), T(0.25)}).nargs(1).template type<T>();

        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "0.125"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{0.125});
    }
    else
    {
        parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(1).template type<T>();

        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "foo"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{"foo"});
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set yields the arguments for nargs set as a number for two arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        parser.add_argument("-o").choices({T('A'), T('C')}).nargs(2).template type<T>();

        auto const args = parser.parse_args(4, cstr_arr{"prog", "-o", "A", "C"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{'A', 'C'});
    }
    else if constexpr (std::is_integral_v<T>)
    {
        parser.add_argument("-o").choices({T(11), T(22)}).nargs(2).template type<T>();

        auto const args = parser.parse_args(4, cstr_arr{"prog", "-o", "11", "22"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{11, 22});
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        parser.add_argument("-o").choices({T(0.125), T(0.25)}).nargs(2).template type<T>();

        auto const args = parser.parse_args(4, cstr_arr{"prog", "-o", "0.125", "0.25"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{0.125, 0.25});
    }
    else
    {
        parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(2).template type<T>();

        auto const args = parser.parse_args(4, cstr_arr{"prog", "-o", "foo", "bar"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{"foo", "bar"});
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set yields the arguments for nargs set as a number for three arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        parser.add_argument("-o").choices({T('A'), T('C')}).nargs(3).template type<T>();

        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "A", "C", "A"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{'A', 'C', 'A'});
    }
    else if constexpr (std::is_integral_v<T>)
    {
        parser.add_argument("-o").choices({T(11), T(22)}).nargs(3).template type<T>();

        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "11", "22", "11"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{11, 22, 11});
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        parser.add_argument("-o").choices({T(0.125), T(0.25)}).nargs(3).template type<T>();

        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "0.125", "0.25", "0.125"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{0.125, 0.25, 0.125});
    }
    else
    {
        parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(3).template type<T>();

        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "bar", "foo"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{"foo", "bar", "foo"});
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set yields the arguments for nargs set as zero_or_one for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        parser.add_argument("-o").choices({T('A'), T('C')}).nargs(argparse::zero_or_one).template type<T>();

        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "A"});

        CHECK(args.get_value<T>("o") == T('A'));
    }
    else if constexpr (std::is_integral_v<T>)
    {
        parser.add_argument("-o").choices({T(11), T(22)}).nargs(argparse::zero_or_one).template type<T>();

        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "11"});

        CHECK(args.get_value<T>("o") == T(11));
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        parser.add_argument("-o").choices({T(0.125), T(0.25)}).nargs(argparse::zero_or_one).template type<T>();

        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "0.125"});

        CHECK(args.get_value<T>("o") == T(0.125));
    }
    else
    {
        parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_one).template type<T>();

        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "foo"});

        CHECK(args.get_value<T>("o") == T("foo"));
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set yields the arguments for nargs set as zero_or_more for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "A"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{'A'});
    }
    else if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "11"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{11});
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "0.125"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{0.125});
    }
    else
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "foo"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{"foo"});
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set yields the arguments for nargs set as zero_or_more for two arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        auto const args = parser.parse_args(4, cstr_arr{"prog", "-o", "A", "A"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{'A', 'A'});
    }
    else if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(4, cstr_arr{"prog", "-o", "11", "11"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{11, 11});
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        auto const args = parser.parse_args(4, cstr_arr{"prog", "-o", "0.125", "0.125"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{0.125, 0.125});
    }
    else
    {
        auto const args = parser.parse_args(4, cstr_arr{"prog", "-o", "foo", "foo"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{"foo", "foo"});
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set yields the arguments for nargs set as zero_or_more for three arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "A", "A", "A"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{'A', 'A', 'A'});
    }
    else if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "11", "11", "11"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{11, 11, 11});
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "0.125", "0.125", "0.125"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{0.125, 0.125, 0.125});
    }
    else
    {
        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "foo", "foo"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{"foo", "foo", "foo"});
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set yields the arguments for nargs set as one_or_more for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "A"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{'A'});
    }
    else if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "11"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{11});
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "0.125"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{0.125});
    }
    else
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "foo"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{"foo"});
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set yields the arguments for nargs set as one_or_more for two arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        auto const args = parser.parse_args(4, cstr_arr{"prog", "-o", "A", "A"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{'A', 'A'});
    }
    else if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(4, cstr_arr{"prog", "-o", "11", "11"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{11, 11});
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        auto const args = parser.parse_args(4, cstr_arr{"prog", "-o", "0.125", "0.125"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{0.125, 0.125});
    }
    else
    {
        auto const args = parser.parse_args(4, cstr_arr{"prog", "-o", "foo", "foo"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{"foo", "foo"});
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set yields the arguments for nargs set as one_or_more for three arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "A", "A", "A"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{'A', 'A', 'A'});
    }
    else if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "11", "11", "11"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{11, 11, 11});
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "0.125", "0.125", "0.125"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{0.125, 0.125, 0.125});
    }
    else
    {
        auto const args = parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "foo", "foo"});

        CHECK(args.get_value<std::vector<T>>("o") == std::vector<T>{"foo", "foo", "foo"});
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set throws an exception on incorrect value for nargs set as a number for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set throws an exception on incorrect value for nargs set as a number for two arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set throws an exception on incorrect value for nargs set as a number for three arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set throws an exception on incorrect value for nargs set as zero_or_one for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set throws an exception on incorrect value for nargs set as zero_or_more for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set throws an exception on incorrect value for nargs set as zero_or_more for two arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set throws an exception on incorrect value for nargs set as zero_or_more for three arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set throws an exception on incorrect value for nargs set as one_or_more for one argument", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set throws an exception on incorrect value for nargs set as one_or_more for two arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE_TEMPLATE("Parsing an optional argument with choices set throws an exception on incorrect value for nargs set as one_or_more for three arguments", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, std::string)
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);

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

TEST_CASE("An optional argument does not consume another optional argument for simple argument")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o");
    parser.add_argument("-p");

    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "-p"}), "argument -o: expected one argument", argparse::parsing_error);
}

TEST_CASE("An optional argument does not consume another optional argument for argument with nargs set as number 1")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(1);
    parser.add_argument("-p");

    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "-p"}), "argument -o: expected 1 argument", argparse::parsing_error);
}

TEST_CASE("An optional argument does not consume another optional argument for argument with nargs set as number 2")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(2);
    parser.add_argument("-p");

    CHECK_THROWS_WITH_AS(parser.parse_args(4, cstr_arr{"prog", "-o", "foo", "-p"}), "argument -o: expected 2 arguments", argparse::parsing_error);
}

TEST_CASE("An optional argument does not consume another optional argument for argument with nargs set as number 3")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(3);
    parser.add_argument("-p");

    CHECK_THROWS_WITH_AS(parser.parse_args(5, cstr_arr{"prog", "-o", "foo", "bar", "-p"}), "argument -o: expected 3 arguments", argparse::parsing_error);
}

TEST_CASE("An optional argument does not consume another optional argument for argument with nargs set as zero_or_one")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(argparse::zero_or_one);
    parser.add_argument("-p");

    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "-p"}), "argument -p: expected one argument", argparse::parsing_error);
}

TEST_CASE("An optional argument does not consume another optional argument for argument with nargs set as zero_or_more")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(argparse::zero_or_more);
    parser.add_argument("-p");

    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "-p"}), "argument -p: expected one argument", argparse::parsing_error);
}

TEST_CASE("An optional argument does not consume another optional argument for argument with nargs set as one_or_more")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(argparse::one_or_more);
    parser.add_argument("-p");

    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "-p"}), "argument -o: expected at least one argument", argparse::parsing_error);
}

TEST_CASE("An optional argument does not consume arguments past the -- pseudo argument for simple argument")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o");

    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "--"}), "argument -o: expected one argument", argparse::parsing_error);
}

TEST_CASE("An optional argument does not consume arguments past the -- pseudo argument for argument with nargs set as number 1")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(1);

    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "--"}), "argument -o: expected 1 argument", argparse::parsing_error);
}

TEST_CASE("An optional argument does not consume arguments past the -- pseudo argument for argument with nargs set as number 2")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(2);

    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "--"}), "argument -o: expected 2 arguments", argparse::parsing_error);
}

TEST_CASE("An optional argument does not consume arguments past the -- pseudo argument for argument with nargs set as number 3")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(3);

    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "--"}), "argument -o: expected 3 arguments", argparse::parsing_error);
}

TEST_CASE("An optional argument does not consume arguments past the -- pseudo argument for argument with nargs set as zero_or_one")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(argparse::zero_or_one);

    auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "--"});

    CHECK(!args.get("o"));
}

TEST_CASE("An optional argument does not consume arguments past the -- pseudo argument for argument with nargs set as zero_or_more")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(argparse::zero_or_more);

    auto const args = parser.parse_args(3, cstr_arr{"prog", "-o", "--"});

    CHECK(args.get_value<std::vector<std::string>>("o") == std::vector<std::string>());
}

TEST_CASE("An optional argument does not consume arguments past the -- pseudo argument for argument with nargs set as one_or_more")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(argparse::one_or_more);

    CHECK_THROWS_WITH_AS(parser.parse_args(3, cstr_arr{"prog", "-o", "--"}), "argument -o: expected at least one argument", argparse::parsing_error);
}

TEST_CASE("An optional argument does not consume arguments past the -- pseudo argument for argument with nargs set as zero_or_more and followed by a positional argument")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").nargs(argparse::zero_or_more);
    parser.add_argument("pos");

    auto const args = parser.parse_args(4, cstr_arr{"prog", "-o", "--", "val"});

    CHECK(args.get_value<std::vector<std::string>>("o") == std::vector<std::string>());
    CHECK(args.get_value("pos") == "val");
}

TEST_CASE("Parsing joined short options does not throw for arguments with store true action")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-a").action(argparse::store_true);
    parser.add_argument("-b").action(argparse::store_true);

    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-ab"}));
}

TEST_CASE("Parsing joined short options does not throw for arguments with store false action")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-a").action(argparse::store_false);
    parser.add_argument("-b").action(argparse::store_false);

    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-ab"}));
}

TEST_CASE("Parsing joined short options does not throw for arguments with store const action")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-a").action(argparse::store_const).const_(10);
    parser.add_argument("-b").action(argparse::store_const).const_(20);

    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-ab"}));
}

TEST_CASE("Parsing joined short options does not throw for arguments with count action")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-a").action(argparse::count);
    parser.add_argument("-b").action(argparse::count);

    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-ab"}));
}

TEST_CASE("Parsing joined short options does not throw for argument with count action")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-a").action(argparse::count);

    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-aa"}));
}

TEST_CASE("Parsing joined short options with store true action yields true for each of them")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-a").action(argparse::store_true);
    parser.add_argument("-b").action(argparse::store_true);

    auto const args = parser.parse_args(2, cstr_arr{"prog", "-ab"});

    CHECK(args.get_value<bool>("a") == true);
    CHECK(args.get_value<bool>("b") == true);
}

TEST_CASE("Parsing joined short options with store false action yields false for each of them")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-a").action(argparse::store_false);
    parser.add_argument("-b").action(argparse::store_false);

    auto const args = parser.parse_args(2, cstr_arr{"prog", "-ab"});

    CHECK(args.get_value<bool>("a") == false);
    CHECK(args.get_value<bool>("b") == false);
}

TEST_CASE("Parsing joined short options with store const action yields const value")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-a").action(argparse::store_const).const_(10);
    parser.add_argument("-b").action(argparse::store_const).const_(20);

    auto const args = parser.parse_args(2, cstr_arr{"prog", "-ab"});

    CHECK(args.get_value<int>("a") == 10);
    CHECK(args.get_value<int>("b") == 20);
}

TEST_CASE("Parsing joined short options with count action yields their count")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-a").action(argparse::count);
    parser.add_argument("-b").action(argparse::count);

    auto const args = parser.parse_args(2, cstr_arr{"prog", "-ababa"});

    CHECK(args.get_value<int>("a") == 3);
    CHECK(args.get_value<int>("b") == 2);
}

TEST_CASE("Parsing joined short options with count action yields their count")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-a").action(argparse::count);
    parser.add_argument("-b").action(argparse::count);

    auto const args = parser.parse_args(2, cstr_arr{"prog", "-abbaa"});

    CHECK(args.get_value<int>("a") == 3);
    CHECK(args.get_value<int>("b") == 2);
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

    auto const args = parser.parse_args(2, cstr_arr{"prog", "--long=value"});

    CHECK(args.get_value("long") == "value");
}

TEST_CASE_TEMPLATE("Parsing long option with joined argument yields its value", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, foo::Custom, bar::Custom)
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("--long").type<T>();

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        auto const args = parser.parse_args(2, cstr_arr{"prog", "--long=A"});

        CHECK(args.get_value<T>("long") == T(65));
    }
    else if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(2, cstr_arr{"prog", "--long=65"});

        CHECK(args.get_value<T>("long") == T(65));
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        auto const args = parser.parse_args(2, cstr_arr{"prog", "--long=1.125"});

        CHECK(args.get_value<T>("long") == T(1.125));
    }
    else
    {
        auto const args = parser.parse_args(2, cstr_arr{"prog", "--long=bar"});

        CHECK(args.get_value<T>("long") == T("bar"));
    }
}

TEST_CASE("Parsing long options with same prefix correctly recognises them plane options")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("--same").action(argparse::store_true);
    parser.add_argument("--same-prefix").action(argparse::store_true);

    auto const args = parser.parse_args(2, cstr_arr{"prog", "--same-prefix"});

    CHECK(args.get_value<bool>("same") == false);
}

TEST_CASE("Parsing long options with same prefix correctly recognises them options with joined argument")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("--same");
    parser.add_argument("--same-prefix");

    auto const args = parser.parse_args(2, cstr_arr{"prog", "--same-prefix=value"});

    CHECK(!args.get("same"));
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

    auto const args = parser.parse_args(2, cstr_arr{"prog", "-ovalue"});

    CHECK(args.get_value("o") == "value");
}

TEST_CASE_TEMPLATE("Parsing short option with joined argument yields its value", T, char, signed char, unsigned char, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double, foo::Custom, bar::Custom)
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o").type<T>();

    if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
    {
        auto const args = parser.parse_args(2, cstr_arr{"prog", "-oA"});

        CHECK(args.get_value<T>("o") == T(65));
    }
    else if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(2, cstr_arr{"prog", "-o65"});

        CHECK(args.get_value<T>("o") == T(65));
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        auto const args = parser.parse_args(2, cstr_arr{"prog", "-o1.125"});

        CHECK(args.get_value<T>("o") == T(1.125));
    }
    else
    {
        auto const args = parser.parse_args(2, cstr_arr{"prog", "-obar"});

        CHECK(args.get_value<T>("o") == T("bar"));
    }
}

TEST_CASE("Parsing short option with joined argument with append action does not throw")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-a").action(argparse::append);

    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-aone"}));
}

TEST_CASE("Parsing short option with joined argument with append action yields its value")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-a").action(argparse::append);

    auto const args = parser.parse_args(2, cstr_arr{"prog", "-aone"});

    CHECK(args.get_value<std::vector<std::string>>("a") == std::vector<std::string>{"one"});
}

TEST_CASE("Parsing short option with joined argument with append action yields its value")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-a").action(argparse::append);

    auto const args = parser.parse_args(3, cstr_arr{"prog", "-aone", "-atwo"});

    CHECK(args.get_value<std::vector<std::string>>("a") == std::vector<std::string>{"one", "two"});
}

TEST_CASE("Parsing joined short options and short option joined with argument does not throw order ao")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-a").action(argparse::store_true);
    parser.add_argument("-o");

    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-aovalue"}));
}

TEST_CASE("Parsing joined short options and short option joined with argument does not throw order oa")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o");
    parser.add_argument("-a").action(argparse::store_true);

    CHECK_NOTHROW(parser.parse_args(2, cstr_arr{"prog", "-aovalue"}));
}

TEST_CASE("Parsing joined short options and short option joined with argument yields their values order ao")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-a").action(argparse::store_true);
    parser.add_argument("-o");

    auto const args = parser.parse_args(2, cstr_arr{"prog", "-aovalue"});

    CHECK(args.get_value<bool>("a") == true);
    CHECK(args.get_value("o") == "value");
}

TEST_CASE("Parsing joined short options and short option joined with argument yields their values order oa")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o");
    parser.add_argument("-a").action(argparse::store_true);

    auto const args = parser.parse_args(2, cstr_arr{"prog", "-aovalue"});

    CHECK(args.get_value<bool>("a") == true);
    CHECK(args.get_value("o") == "value");
}

TEST_CASE("Parsing joined short optiona and short option joined with argument does not confuse values order oav")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-o");
    parser.add_argument("-a").action(argparse::store_true);
    parser.add_argument("-v").action(argparse::store_true);

    auto const args = parser.parse_args(2, cstr_arr{"prog", "-aovalue"});

    CHECK(args.get_value<bool>("a") == true);
    CHECK(args.get_value<bool>("v") == false);
    CHECK(args.get_value("o") == "value");
}

TEST_CASE("Parsing joined short optiona and short option joined with argument does not confuse values order vao")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-v").action(argparse::store_true);
    parser.add_argument("-a").action(argparse::store_true);
    parser.add_argument("-o");

    auto const args = parser.parse_args(2, cstr_arr{"prog", "-aovalue"});

    CHECK(args.get_value<bool>("a") == true);
    CHECK(args.get_value<bool>("v") == false);
    CHECK(args.get_value("o") == "value");
}

TEST_CASE("Parsing long options does not affect short options")
{
    auto parser = argparse::ArgumentParser().add_help(false);
    parser.add_argument("-b").action(argparse::store_true);
    parser.add_argument("--bar").action(argparse::store_true);
    parser.add_argument("-a").action(argparse::store_true);
    parser.add_argument("-r").action(argparse::store_true);

    auto const args = parser.parse_args(2, cstr_arr{"prog", "--bar"});

    CHECK(args.get_value<bool>("b") == false);
    CHECK(args.get_value<bool>("a") == false);
    CHECK(args.get_value<bool>("r") == false);
    CHECK(args.get_value<bool>("bar") == true);
}

TEST_CASE("Parsing long options does not affect short options")
{
    auto parser = argparse::ArgumentParser().add_help(false);
    parser.add_argument("-b").action(argparse::store_true);
    parser.add_argument("--bar");
    parser.add_argument("-a").action(argparse::store_true);
    parser.add_argument("-r").action(argparse::store_true);

    auto const args = parser.parse_args(2, cstr_arr{"prog", "--bar=bar"});

    CHECK(args.get_value<bool>("b") == false);
    CHECK(args.get_value<bool>("a") == false);
    CHECK(args.get_value<bool>("r") == false);
    CHECK(args.get_value("bar") == "bar");
}

TEST_CASE("Parsing joined flags does not affect long options")
{
    auto parser = argparse::ArgumentParser().add_help(false);
    parser.add_argument("--bar").action(argparse::store_true);
    parser.add_argument("-b").action(argparse::store_true);
    parser.add_argument("-a").action(argparse::store_true);
    parser.add_argument("-r").action(argparse::store_true);

    auto const args = parser.parse_args(2, cstr_arr{"prog", "-bar"});

    CHECK(args.get_value<bool>("bar") == false);
    CHECK(args.get_value<bool>("b") == true);
    CHECK(args.get_value<bool>("a") == true);
    CHECK(args.get_value<bool>("r") == true);
}

TEST_CASE_TEMPLATE("Parsing an optional argument yields correct value for positive number", T, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double)
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-n").type<T>();

    if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-n", "65"});

        CHECK(args.get_value<T>("n") == T(65));
    }
    else
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-n", "1.125"});

        CHECK(args.get_value<T>("n") == T(1.125));
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument yields correct value for positive number", T, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double)
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-n").type<T>();

    if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(2, cstr_arr{"prog", "-n65"});

        CHECK(args.get_value<T>("n") == T(65));
    }
    else
    {
        auto const args = parser.parse_args(2, cstr_arr{"prog", "-n1.125"});

        CHECK(args.get_value<T>("n") == T(1.125));
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument yields correct value for positive number", T, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double)
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("--number").type<T>();

    if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "--number", "65"});

        CHECK(args.get_value<T>("number") == T(65));
    }
    else
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "--number", "1.125"});

        CHECK(args.get_value<T>("number") == T(1.125));
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument yields correct value for positive number", T, short int, unsigned short int, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double)
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("--number").type<T>();

    if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(2, cstr_arr{"prog", "--number=65"});

        CHECK(args.get_value<T>("number") == T(65));
    }
    else
    {
        auto const args = parser.parse_args(2, cstr_arr{"prog", "--number=1.125"});

        CHECK(args.get_value<T>("number") == T(1.125));
    }
}

TEST_CASE_TEMPLATE("Parsing an optional argument yields correct value for negative number", T, short int, int, long int, long long int, float, double, long double)
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-n").type<T>();

    if constexpr (std::is_integral_v<T>)
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-n", "-65"});

        CHECK(args.get_value<T>("n") == T(-65));
    }
    else
    {
        auto const args = parser.parse_args(3, cstr_arr{"prog", "-n", "-1.125"});

        CHECK(args.get_value<T>("n") == T(-1.125));
    }
}

TEST_CASE("Parsing an optional argument yields correct value for negative number")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-n").type<int>();

    auto const args = parser.parse_args(2, cstr_arr{"prog", "-n-65"});

    CHECK(args.get_value<int>("n") == -65);
}

TEST_CASE("Parsing an optional argument yields correct value for negative number")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("--number").type<int>();

    auto const args = parser.parse_args(3, cstr_arr{"prog", "--number", "-65"});

    CHECK(args.get_value<int>("number") == -65);
}

TEST_CASE("Parsing an optional argument yields correct value for negative number")
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("--number").type<int>();

    auto const args = parser.parse_args(2, cstr_arr{"prog", "--number=-65"});

    CHECK(args.get_value<int>("number") == -65);
}
