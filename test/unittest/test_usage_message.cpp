#include "argparse.h"

#include "doctest.h"

#include <string>


using namespace std::string_literals;

TEST_CASE("ArgumentParser provides usage message...")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);

    SUBCASE("...with prog name")
    {
        CHECK(parser.format_usage() == "usage: prog"s);
    }

    SUBCASE("...with prog name and list of positional arguments for one positional argument")
    {
        parser.add_argument("p1");

        CHECK(parser.format_usage() == "usage: prog p1"s);
    }

    SUBCASE("...with prog name and list of positional arguments for two positional arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("p2");

        CHECK(parser.format_usage() == "usage: prog p1 p2"s);
    }

    SUBCASE("...with prog name and list of positional arguments for three positional arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("p2");
        parser.add_argument("p3");

        CHECK(parser.format_usage() == "usage: prog p1 p2 p3"s);
    }

    SUBCASE("...with prog name and list of optional arguments for one optional argument")
    {
        parser.add_argument("-o");

        CHECK(parser.format_usage() == "usage: prog [-o O]"s);
    }

    SUBCASE("...with prog name and list of optional arguments for two optional arguments")
    {
        parser.add_argument("-o");
        parser.add_argument("--option");

        CHECK(parser.format_usage() == "usage: prog [-o O] [--option OPTION]"s);
    }

    SUBCASE("...with prog name and list of optional arguments for three optional arguments")
    {
        parser.add_argument("-o");
        parser.add_argument("--option");
        parser.add_argument("--very-long-name");

        CHECK(parser.format_usage() == "usage: prog [-o O] [--option OPTION] [--very-long-name VERY_LONG_NAME]"s);
    }

    SUBCASE("...for one optional argument with short and long name")
    {
        parser.add_argument("-f", "--foo");

        CHECK(parser.format_usage() == "usage: prog [-f FOO]"s);
    }

    SUBCASE("... for one optional argument with store true action")
    {
        parser.add_argument("-o").action(argparse::store_true);

        CHECK(parser.format_usage() == "usage: prog [-o]"s);
    }

    SUBCASE("...for two optional arguments with store true action")
    {
        parser.add_argument("-o").action(argparse::store_true);
        parser.add_argument("--option").action(argparse::store_true);

        CHECK(parser.format_usage() == "usage: prog [-o] [--option]"s);
    }

    SUBCASE("...for one optional argument with store false action")
    {
        parser.add_argument("-o").action(argparse::store_false);

        CHECK(parser.format_usage() == "usage: prog [-o]"s);
    }

    SUBCASE("...for two optional arguments with store false action")
    {
        parser.add_argument("-o").action(argparse::store_false);
        parser.add_argument("--option").action(argparse::store_false);

        CHECK(parser.format_usage() == "usage: prog [-o] [--option]"s);
    }

    SUBCASE("...for one optional argument with store const action")
    {
        parser.add_argument("-o").action(argparse::store_const);

        CHECK(parser.format_usage() == "usage: prog [-o]"s);
    }

    SUBCASE("...for one optional argument with help action")
    {
        parser.add_argument("-h").action(argparse::help);

        CHECK(parser.format_usage() == "usage: prog [-h]"s);
    }

    SUBCASE("...for one positional and one optional argument")
    {
        parser.add_argument("p1");
        parser.add_argument("-o");

        CHECK(parser.format_usage() == "usage: prog [-o O] p1"s);
    }

    SUBCASE("...for three positional and three optional arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("-o");
        parser.add_argument("p2");
        parser.add_argument("-a");
        parser.add_argument("p3");
        parser.add_argument("-z");

        CHECK(parser.format_usage() == "usage: prog [-o O] [-a A] [-z Z] p1 p2 p3"s);
    }
}

TEST_CASE("Usage message contains...")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);

    SUBCASE("...for positional argument...")
    {
        SUBCASE("...its name")
        {
            parser.add_argument("p1");

            CHECK(parser.format_usage() == "usage: prog p1"s);
        }

        SUBCASE("...metavar for argument with metavar set")
        {
            parser.add_argument("p1").metavar("metap1");

            CHECK(parser.format_usage() == "usage: prog metap1"s);
        }

        SUBCASE("...choices for argument with choices set")
        {
            parser.add_argument("p1").choices({"foo"s, "bar"s});

            CHECK(parser.format_usage() == "usage: prog {\"foo\",\"bar\"}"s);
        }

        SUBCASE("...its name repeated N times for argument with nargs set as number...")
        {
            SUBCASE("...1")
            {
                parser.add_argument("p1").nargs(1);

                CHECK(parser.format_usage() == "usage: prog p1"s);
            }

            SUBCASE("...2")
            {
                parser.add_argument("p1").nargs(2);

                CHECK(parser.format_usage() == "usage: prog p1 p1"s);
            }

            SUBCASE("...3")
            {
                parser.add_argument("p1").nargs(3);

                CHECK(parser.format_usage() == "usage: prog p1 p1 p1"s);
            }
        }

        SUBCASE("...choices repeated N times for argument with choices set and nargs set as number...")
        {
            SUBCASE("...1")
            {
                parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(1);

                CHECK(parser.format_usage() == "usage: prog {\"foo\",\"bar\"}"s);
            }

            SUBCASE("...2")
            {
                parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(2);

                CHECK(parser.format_usage() == "usage: prog {\"foo\",\"bar\"} {\"foo\",\"bar\"}"s);
            }

            SUBCASE("...3")
            {
                parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(3);

                CHECK(parser.format_usage() == "usage: prog {\"foo\",\"bar\"} {\"foo\",\"bar\"} {\"foo\",\"bar\"}"s);
            }
        }

        SUBCASE("...its name in brackets for argument with nargs set as zero_or_one")
        {
            parser.add_argument("p1").nargs(argparse::zero_or_one);

            CHECK(parser.format_usage() == "usage: prog [p1]"s);
        }

        SUBCASE("...choices in brackets for argument with choices set and nargs set as zero_or_one")
        {
            parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_one);

            CHECK(parser.format_usage() == "usage: prog [{\"foo\",\"bar\"}]"s);
        }

        SUBCASE("...its name in brackets followed by its name and ellipsis in brackets for argument with nargs set as zero_or_more")
        {
            parser.add_argument("p1").nargs(argparse::zero_or_more);

            CHECK(parser.format_usage() == "usage: prog [p1 [p1 ...]]"s);
        }

        SUBCASE("...choices in brackets followed by choices and ellipsis in brackets for argument with choices set and nargs set as zero_or_more")
        {
            parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_more);

            CHECK(parser.format_usage() == "usage: prog [{\"foo\",\"bar\"} [{\"foo\",\"bar\"} ...]]"s);
        }

        SUBCASE("...its name followed by its name and ellipsis in brackets for argument with nargs set as one_or_more")
        {
            parser.add_argument("p1").nargs(argparse::one_or_more);

            CHECK(parser.format_usage() == "usage: prog p1 [p1 ...]"s);
        }

        SUBCASE("...choices followed by choices and ellipsis in brackets for argument with choices set and nargs set as one_or_more")
        {
            parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(argparse::one_or_more);

            CHECK(parser.format_usage() == "usage: prog {\"foo\",\"bar\"} [{\"foo\",\"bar\"} ...]"s);
        }
    }

    SUBCASE("...for optional argument...")
    {
        SUBCASE("...name for argument with store true action")
        {
            parser.add_argument("-o").action(argparse::store_true);

            CHECK(parser.format_usage() == "usage: prog [-o]"s);
        }

        SUBCASE("...name for argument with store false action")
        {
            parser.add_argument("-o").action(argparse::store_false);

            CHECK(parser.format_usage() == "usage: prog [-o]"s);
        }

        SUBCASE("...name for argument with store const action")
        {
            parser.add_argument("-o").action(argparse::store_const);

            CHECK(parser.format_usage() == "usage: prog [-o]"s);
        }

        SUBCASE("...name for argument with help action")
        {
            parser.add_argument("-h").action(argparse::store_false);

            CHECK(parser.format_usage() == "usage: prog [-h]"s);
        }

        SUBCASE("...name and automatic metavar")
        {
            parser.add_argument("-o");

            CHECK(parser.format_usage() == "usage: prog [-o O]"s);
        }

        SUBCASE("...name and metavar for argument with metavar set")
        {
            parser.add_argument("-o").metavar("METAVARO");

            CHECK(parser.format_usage() == "usage: prog [-o METAVARO]"s);
        }

        SUBCASE("...no brackets for argument with required true...")
        {
            SUBCASE("...and automatic metavar")
            {
                parser.add_argument("-o").required(true);

                CHECK(parser.format_usage() == "usage: prog -o O"s);
            }

            SUBCASE("...and metavar")
            {
                parser.add_argument("-o").metavar("METAVARO").required(true);

                CHECK(parser.format_usage() == "usage: prog -o METAVARO"s);
            }

            SUBCASE("...and choices")
            {
                parser.add_argument("-o").choices({"foo"s, "bar"s}).required(true);

                CHECK(parser.format_usage() == "usage: prog -o {\"foo\",\"bar\"}"s);
            }

            SUBCASE("...and nargs")
            {
                parser.add_argument("-o").nargs(1).required(true);

                CHECK(parser.format_usage() == "usage: prog -o O"s);
            }
        }

        SUBCASE("...brackets for argument with required false")
        {
            parser.add_argument("-o").required(false);

            CHECK(parser.format_usage() == "usage: prog [-o O]"s);
        }

        SUBCASE("...name and choices for argument with choices set")
        {
            parser.add_argument("-o").choices({"foo"s, "bar"s});

            CHECK(parser.format_usage() == "usage: prog [-o {\"foo\",\"bar\"}]"s);
        }

        SUBCASE("...its name and automatic metavar repeated N times for argument with nargs set as number...")
        {
            SUBCASE("...1")
            {
                parser.add_argument("-o").nargs(1);

                CHECK(parser.format_usage() == "usage: prog [-o O]"s);
            }

            SUBCASE("...2")
            {
                parser.add_argument("-o").nargs(2);

                CHECK(parser.format_usage() == "usage: prog [-o O O]"s);
            }

            SUBCASE("...2")
            {
                parser.add_argument("-o").nargs(3);

                CHECK(parser.format_usage() == "usage: prog [-o O O O]"s);
            }
        }

        SUBCASE("...its name and metavar repeated N times for argument with metavar set and nargs set as number...")
        {
            SUBCASE("...1")
            {
                parser.add_argument("-o").metavar("METAVARO").nargs(1);

                CHECK(parser.format_usage() == "usage: prog [-o METAVARO]"s);
            }

            SUBCASE("...2")
            {
                parser.add_argument("-o").metavar("METAVARO").nargs(2);

                CHECK(parser.format_usage() == "usage: prog [-o METAVARO METAVARO]"s);
            }

            SUBCASE("...3")
            {
                parser.add_argument("-o").metavar("METAVARO").nargs(3);

                CHECK(parser.format_usage() == "usage: prog [-o METAVARO METAVARO METAVARO]"s);
            }
        }

        SUBCASE("...its name and choices repeated N times for argument with choices set and nargs set as number...")
        {
            SUBCASE("...1")
            {
                parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(1);

                CHECK(parser.format_usage() == "usage: prog [-o {\"foo\",\"bar\"}]"s);
            }

            SUBCASE("...2")
            {
                parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(2);

                CHECK(parser.format_usage() == "usage: prog [-o {\"foo\",\"bar\"} {\"foo\",\"bar\"}]"s);
            }

            SUBCASE("...3")
            {
                parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(3);

                CHECK(parser.format_usage() == "usage: prog [-o {\"foo\",\"bar\"} {\"foo\",\"bar\"} {\"foo\",\"bar\"}]"s);
            }
        }

        SUBCASE("...is name and automatic metavar in brackets for argument with nargs set as zero_or_one")
        {
            parser.add_argument("-o").nargs(argparse::zero_or_one);

            CHECK(parser.format_usage() == "usage: prog [-o [O]]"s);
        }

        SUBCASE("...is name and metavar in brackets for argument with metavar set and nargs set as zero_or_one")
        {
            parser.add_argument("-o").metavar("METAVARO").nargs(argparse::zero_or_one);

            CHECK(parser.format_usage() == "usage: prog [-o [METAVARO]]"s);
        }

        SUBCASE("...is name and choices in brackets for argument with choices set and nargs set as zero_or_one")
        {
            parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_one);

            CHECK(parser.format_usage() == "usage: prog [-o [{\"foo\",\"bar\"}]]"s);
        }

        SUBCASE("...its name followed by automatic metavar in brackets and automatic metavar and ellipsis in nested brackets for arguments with nargs set as zero_or_more")
        {
            parser.add_argument("-o").nargs(argparse::zero_or_more);

            CHECK(parser.format_usage() == "usage: prog [-o [O [O ...]]]"s);
        }

        SUBCASE("...its name followed by metavar in brackets and metavar and ellipsis in nested brackets for arguments with metavar set and nargs set as zero_or_more")
        {
            parser.add_argument("-o").metavar("METAVARO").nargs(argparse::zero_or_more);

            CHECK(parser.format_usage() == "usage: prog [-o [METAVARO [METAVARO ...]]]"s);
        }

        SUBCASE("...its name followed by choices in brackets and choices and ellipsis in nested brackets for arguments with choices set and nargs set as zero_or_more")
        {
            parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_more);

            CHECK(parser.format_usage() == "usage: prog [-o [{\"foo\",\"bar\"} [{\"foo\",\"bar\"} ...]]]"s);
        }

        SUBCASE("...its name followed by automatic metavar and automatic metavar and ellipsis in brackets for arguments with nargs set as one_or_more")
        {
            parser.add_argument("-o").nargs(argparse::one_or_more);

            CHECK(parser.format_usage() == "usage: prog [-o O [O ...]]"s);
        }

        SUBCASE("...its name followed by metavar and metavar and ellipsis in brackets for arguments with metavar set and nargs set as one_or_more")
        {
            parser.add_argument("-o").metavar("METAVARO").nargs(argparse::one_or_more);

            CHECK(parser.format_usage() == "usage: prog [-o METAVARO [METAVARO ...]]"s);
        }

        SUBCASE("...its name followed by choices and choices and ellipsis in brackets for arguments with choices set and nargs set as one_or_more")
        {
            parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(argparse::one_or_more);

            CHECK(parser.format_usage() == "usage: prog [-o {\"foo\",\"bar\"} [{\"foo\",\"bar\"} ...]]"s);
        }
    }

    SUBCASE("...for optional argument in mutually exclusive group its name in brackets")
    {
        auto group = parser.add_mutually_exclusive_group();
        group.add_argument("-o");

        CHECK(parser.format_usage() == "usage: prog [-o O]"s);
    }

    SUBCASE("...for two optional arguments in same mutually exclusive group their names in brackets separated by a pipe")
    {
        auto group = parser.add_mutually_exclusive_group();
        group.add_argument("-o");
        group.add_argument("-a");

        CHECK(parser.format_usage() == "usage: prog [-o O | -a A]"s);
    }

    SUBCASE("...for three optional arguments in same mutually exclusive group their names in brackets separated by a pipe")
    {
        auto group = parser.add_mutually_exclusive_group();
        group.add_argument("-o");
        group.add_argument("-a");
        group.add_argument("-z");

        CHECK(parser.format_usage() == "usage: prog [-o O | -a A | -z Z]"s);
    }

    SUBCASE("...for two optional arguments in separate mutually exclusive groups their names in brackets")
    {
        auto group1 = parser.add_mutually_exclusive_group();
        group1.add_argument("-o");
        auto group2 = parser.add_mutually_exclusive_group();
        group2.add_argument("-a");

        CHECK(parser.format_usage() == "usage: prog [-o O] [-a A]"s);
    }

    SUBCASE("...for four optional arguments in separate mutually exclusive groups their names in brackets separated by a pipe")
    {
        auto group1 = parser.add_mutually_exclusive_group();
        group1.add_argument("-o");
        group1.add_argument("-p");
        auto group2 = parser.add_mutually_exclusive_group();
        group2.add_argument("-a");
        group2.add_argument("-b");

        CHECK(parser.format_usage() == "usage: prog [-o O | -p P] [-a A | -b B]"s);
    }
}
