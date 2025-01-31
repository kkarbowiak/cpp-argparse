#include "argparse.h"

#include "doctest.h"

#include <string>


#ifdef _MSC_VER
#pragma warning(disable: 4456)
#endif


using namespace std::string_literals;

TEST_CASE("ArgumentParser provides help message...")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);

    SUBCASE("...with usage section")
    {
        CHECK(parser.format_help() == "usage: prog"s);
    }

    SUBCASE("...with usage section and description for description and no arguments")
    {
        auto parser = argparse::ArgumentParser().prog("prog").description("A foo that bars").add_help(false);

        CHECK(parser.format_help() == "usage: prog\n\nA foo that bars"s);
    }

    SUBCASE("...with usage section and epilog for epilog and no arguments")
    {
        auto parser = argparse::ArgumentParser().prog("prog").epilog("And that's how you'd foo a bar").add_help(false);

        CHECK(parser.format_help() == "usage: prog\n\nAnd that's how you'd foo a bar"s);
    }

    SUBCASE("...with usage section and positional arguments section for one positional argument")
    {
        parser.add_argument("p1");

        CHECK(parser.format_help() == "usage: prog p1\n\npositional arguments:\n  p1"s);
    }

    SUBCASE("...with usage section and positional arguments section for two positional arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("p2");

        CHECK(parser.format_help() == "usage: prog p1 p2\n\npositional arguments:\n  p1\n  p2"s);
    }

    SUBCASE("...with usage section and positional arguments section for three positional arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("p2");
        parser.add_argument("p3");

        CHECK(parser.format_help() == "usage: prog p1 p2 p3\n\npositional arguments:\n  p1\n  p2\n  p3"s);
    }

    SUBCASE("...with usage section and optional arguments section for one optional argument")
    {
        parser.add_argument("-o");

        CHECK(parser.format_help() == "usage: prog [-o O]\n\noptional arguments:\n  -o O"s);
    }

    SUBCASE("...with usage section and optional arguments section for two optional arguments")
    {
        parser.add_argument("-o");
        parser.add_argument("--option");

        CHECK(parser.format_help() == "usage: prog [-o O] [--option OPTION]\n\noptional arguments:\n  -o O\n  --option OPTION"s);
    }

    SUBCASE("...with usage section and optional arguments section for three optional arguments")
    {
        parser.add_argument("-o");
        parser.add_argument("--option");
        parser.add_argument("--very-long-name");

        CHECK(parser.format_help() == "usage: prog [-o O] [--option OPTION] [--very-long-name VERY_LONG_NAME]\n\noptional arguments:\n  -o O\n  --option OPTION\n  --very-long-name VERY_LONG_NAME"s);
    }

    SUBCASE("...with usage section, positional arguments section, and optional arguments section for one positional and one optional argument")
    {
        parser.add_argument("p1");
        parser.add_argument("-o");

        CHECK(parser.format_help() == "usage: prog [-o O] p1\n\npositional arguments:\n  p1\n\noptional arguments:\n  -o O"s);
    }

    SUBCASE("...with usage section, positional arguments section, and optional arguments section for three positional and three optional arguments")
    {
        parser.add_argument("p1");
        parser.add_argument("-o");
        parser.add_argument("p2");
        parser.add_argument("-a");
        parser.add_argument("p3");
        parser.add_argument("-z");

        CHECK(parser.format_help() == "usage: prog [-o O] [-a A] [-z Z] p1 p2 p3\n\npositional arguments:\n  p1\n  p2\n  p3\n\noptional arguments:\n  -o O\n  -a A\n  -z Z"s);
    }
}

TEST_CASE("Help message contains...")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);

    SUBCASE("...for positional argument...")
    {
        SUBCASE("...name")
        {
            parser.add_argument("p1");

            CHECK(parser.format_help() == "usage: prog p1\n\npositional arguments:\n  p1"s);
        }

        SUBCASE("...metavar for argument with metavar set")
        {
            parser.add_argument("p1").metavar("metap1");

            CHECK(parser.format_help() == "usage: prog metap1\n\npositional arguments:\n  metap1"s);
        }

        SUBCASE("...name and help for argument with help string")
        {
            parser.add_argument("p1").help("help1");

            CHECK(parser.format_help() == "usage: prog p1\n\npositional arguments:\n  p1                    help1"s);
        }

        SUBCASE("...choices for argument with choices set")
        {
            parser.add_argument("p1").choices({"foo"s, "bar"s});

            CHECK(parser.format_help() == "usage: prog {\"foo\",\"bar\"}\n\npositional arguments:\n  {\"foo\",\"bar\"}"s);
        }

        SUBCASE("...name for argument with nargs set as number...")
        {
            SUBCASE("...1")
            {
                parser.add_argument("p1").nargs(1);

                CHECK(parser.format_help() == "usage: prog p1\n\npositional arguments:\n  p1"s);
            }

            SUBCASE("...2")
            {
                parser.add_argument("p1").nargs(2);

                CHECK(parser.format_help() == "usage: prog p1 p1\n\npositional arguments:\n  p1"s);
            }

            SUBCASE("...3")
            {
                parser.add_argument("p1").nargs(3);

                CHECK(parser.format_help() == "usage: prog p1 p1 p1\n\npositional arguments:\n  p1"s);
            }
        }

        SUBCASE("...metavar for argument with metavar set and nargs set as number...")
        {
            SUBCASE("...1")
            {
                parser.add_argument("p1").metavar("metap1").nargs(1);

                CHECK(parser.format_help() == "usage: prog metap1\n\npositional arguments:\n  metap1"s);
            }

            SUBCASE("...2")
            {
                parser.add_argument("p1").metavar("metap1").nargs(2);

                CHECK(parser.format_help() == "usage: prog metap1 metap1\n\npositional arguments:\n  metap1"s);
            }

            SUBCASE("...3")
            {
                parser.add_argument("p1").metavar("metap1").nargs(3);

                CHECK(parser.format_help() == "usage: prog metap1 metap1 metap1\n\npositional arguments:\n  metap1"s);
            }
        }

        SUBCASE("...choices for argument with choices set and nargs set as number...")
        {
            SUBCASE("...1")
            {
                parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(1);

                CHECK(parser.format_help() == "usage: prog {\"foo\",\"bar\"}\n\npositional arguments:\n  {\"foo\",\"bar\"}"s);
            }

            SUBCASE("...2")
            {
                parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(2);

                CHECK(parser.format_help() == "usage: prog {\"foo\",\"bar\"} {\"foo\",\"bar\"}\n\npositional arguments:\n  {\"foo\",\"bar\"}"s);
            }

            SUBCASE("...3")
            {
                parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(3);

                CHECK(parser.format_help() == "usage: prog {\"foo\",\"bar\"} {\"foo\",\"bar\"} {\"foo\",\"bar\"}\n\npositional arguments:\n  {\"foo\",\"bar\"}"s);
            }
        }

        SUBCASE("...name for argument with nargs set as zero_or_one")
        {
            parser.add_argument("p1").nargs(argparse::zero_or_one);

            CHECK(parser.format_help() == "usage: prog [p1]\n\npositional arguments:\n  p1"s);
        }

        SUBCASE("...metavar for argument with metavar set and nargs set as zero_or_one")
        {
            parser.add_argument("p1").metavar("metap1").nargs(argparse::zero_or_one);

            CHECK(parser.format_help() == "usage: prog [metap1]\n\npositional arguments:\n  metap1"s);
        }

        SUBCASE("...choices for argument with choices set and nargs set as zero_or_one")
        {
            parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_one);

            CHECK(parser.format_help() == "usage: prog [{\"foo\",\"bar\"}]\n\npositional arguments:\n  {\"foo\",\"bar\"}"s);
        }

        SUBCASE("...name for argument with nargs set as zero_or_more")
        {
            parser.add_argument("p1").nargs(argparse::zero_or_more);

            CHECK(parser.format_help() == "usage: prog [p1 [p1 ...]]\n\npositional arguments:\n  p1"s);
        }

        SUBCASE("...metavar for argument with metavar set and nargs set as zero_or_more")
        {
            parser.add_argument("p1").metavar("metap1").nargs(argparse::zero_or_more);

            CHECK(parser.format_help() == "usage: prog [metap1 [metap1 ...]]\n\npositional arguments:\n  metap1"s);
        }

        SUBCASE("...choices for argument with choices set and nargs set as zero_or_more")
        {
            parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_more);

            CHECK(parser.format_help() == "usage: prog [{\"foo\",\"bar\"} [{\"foo\",\"bar\"} ...]]\n\npositional arguments:\n  {\"foo\",\"bar\"}"s);
        }

        SUBCASE("...name for argument with nargs set as one_or_more")
        {
            parser.add_argument("p1").nargs(argparse::one_or_more);

            CHECK(parser.format_help() == "usage: prog p1 [p1 ...]\n\npositional arguments:\n  p1"s);
        }

        SUBCASE("...metavar for argument with metavar set and nargs set as one_or_more")
        {
            parser.add_argument("p1").metavar("metap1").nargs(argparse::one_or_more);

            CHECK(parser.format_help() == "usage: prog metap1 [metap1 ...]\n\npositional arguments:\n  metap1"s);
        }

        SUBCASE("...choices for argument with choices set and nargs set as one_or_more")
        {
            parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(argparse::one_or_more);

            CHECK(parser.format_help() == "usage: prog {\"foo\",\"bar\"} [{\"foo\",\"bar\"} ...]\n\npositional arguments:\n  {\"foo\",\"bar\"}"s);
        }
    }

    SUBCASE("...for optional argument...")
    {
        SUBCASE("...name for argument with store true action")
        {
            parser.add_argument("-o").action(argparse::store_true);

            CHECK(parser.format_help() == "usage: prog [-o]\n\noptional arguments:\n  -o"s);
        }

        SUBCASE("...name and long name for argument with store true action")
        {
            parser.add_argument("-o", "--option").action(argparse::store_true);

            CHECK(parser.format_help() == "usage: prog [-o]\n\noptional arguments:\n  -o, --option"s);
        }

        SUBCASE("...name and help for argument with store true action and help string")
        {
            parser.add_argument("-o").action(argparse::store_true).help("help1");

            CHECK(parser.format_help() == "usage: prog [-o]\n\noptional arguments:\n  -o                    help1"s);
        }

        SUBCASE("...name for argument with store false action")
        {
            parser.add_argument("-o").action(argparse::store_false);

            CHECK(parser.format_help() == "usage: prog [-o]\n\noptional arguments:\n  -o"s);
        }

        SUBCASE("...name and long name for argument with store false action")
        {
            parser.add_argument("-o", "--option").action(argparse::store_false);

            CHECK(parser.format_help() == "usage: prog [-o]\n\noptional arguments:\n  -o, --option"s);
        }

        SUBCASE("...name and long name for argument with store false action")
        {
            parser.add_argument("-o", "--option").action(argparse::store_false);

            CHECK(parser.format_help() == "usage: prog [-o]\n\noptional arguments:\n  -o, --option"s);
        }

        SUBCASE("...name and help for argument with store false action and help string")
        {
            parser.add_argument("-o").action(argparse::store_false).help("help1");

            CHECK(parser.format_help() == "usage: prog [-o]\n\noptional arguments:\n  -o                    help1"s);
        }

        SUBCASE("...name for argument with store const action")
        {
            parser.add_argument("-o").action(argparse::store_const);

            CHECK(parser.format_help() == "usage: prog [-o]\n\noptional arguments:\n  -o"s);
        }

        SUBCASE("...name and long name for argument with store const action")
        {
            parser.add_argument("-o", "--option").action(argparse::store_const);

            CHECK(parser.format_help() == "usage: prog [-o]\n\noptional arguments:\n  -o, --option"s);
        }

        SUBCASE("...name and help for argument with store const action and help string")
        {
            parser.add_argument("-o").action(argparse::store_const).help("help1");

            CHECK(parser.format_help() == "usage: prog [-o]\n\noptional arguments:\n  -o                    help1"s);
        }

        SUBCASE("...name for argument with help action")
        {
            parser.add_argument("-h").action(argparse::help);

            CHECK(parser.format_help() == "usage: prog [-h]\n\noptional arguments:\n  -h"s);
        }

        SUBCASE("...name and long name for argument with help action")
        {
            parser.add_argument("-h", "--help").action(argparse::help);

            CHECK(parser.format_help() == "usage: prog [-h]\n\noptional arguments:\n  -h, --help"s);
        }

        SUBCASE("...name and help for argument with help action and help string")
        {
            parser.add_argument("-h").action(argparse::help).help("help1");

            CHECK(parser.format_help() == "usage: prog [-h]\n\noptional arguments:\n  -h                    help1"s);
        }

        SUBCASE("...name and help for automatically added help argument")
        {
            auto parser = argparse::ArgumentParser().prog("prog");

            CHECK(parser.format_help() == "usage: prog [-h]\n\noptional arguments:\n  -h, --help            show this help message and exit"s);
        }

        SUBCASE("...name and automatically added help for argument with version action")
        {
            parser.add_argument("-v").action(argparse::version);

            CHECK(parser.format_help() == "usage: prog [-v]\n\noptional arguments:\n  -v                    show program's version number and exit"s);
        }

        SUBCASE("...name, long name, and automatically added help for argument with version action")
        {
            parser.add_argument("-v", "--version").action(argparse::version);

            CHECK(parser.format_help() == "usage: prog [-v]\n\noptional arguments:\n  -v, --version         show program's version number and exit"s);
        }

        SUBCASE("...name and help for argument with version action and help string")
        {
            parser.add_argument("-v").action(argparse::version).help("version1");

            CHECK(parser.format_help() == "usage: prog [-v]\n\noptional arguments:\n  -v                    version1"s);
        }

        SUBCASE("...name and automatic metavar")
        {
            parser.add_argument("-o");

            CHECK(parser.format_help() == "usage: prog [-o O]\n\noptional arguments:\n  -o O"s);
        }

        SUBCASE("...name and metavar for argument with metavar")
        {
            parser.add_argument("-o").metavar("METAVARO");

            CHECK(parser.format_help() == "usage: prog [-o METAVARO]\n\noptional arguments:\n  -o METAVARO"s);
        }

        SUBCASE("...name, automatic metavar, and help for argument with help string")
        {
            parser.add_argument("-o").help("help1");

            CHECK(parser.format_help() == "usage: prog [-o O]\n\noptional arguments:\n  -o O                  help1"s);
        }

        SUBCASE("...no brackets for argument with required true")
        {
            parser.add_argument("-o").required(true);

            CHECK(parser.format_help() == "usage: prog -o O\n\noptional arguments:\n  -o O"s);
        }

        SUBCASE("...brackets for argument with required false")
        {
            parser.add_argument("-o").required(false);

            CHECK(parser.format_help() == "usage: prog [-o O]\n\noptional arguments:\n  -o O"s);
        }

        SUBCASE("...name and choices for argument with choices set")
        {
            parser.add_argument("-o").choices({"foo"s, "bar"s});

            CHECK(parser.format_help() == "usage: prog [-o {\"foo\",\"bar\"}]\n\noptional arguments:\n  -o {\"foo\",\"bar\"}"s);
        }

        SUBCASE("...name and automatic metavar repeated N times for argument with nargs set as number...")
        {
            SUBCASE("...1")
            {
                parser.add_argument("-o").nargs(1);

                CHECK(parser.format_help() == "usage: prog [-o O]\n\noptional arguments:\n  -o O"s);
            }

            SUBCASE("...2")
            {
                parser.add_argument("-o").nargs(2);

                CHECK(parser.format_help() == "usage: prog [-o O O]\n\noptional arguments:\n  -o O O"s);
            }

            SUBCASE("...3")
            {
                parser.add_argument("-o").nargs(3);

                CHECK(parser.format_help() == "usage: prog [-o O O O]\n\noptional arguments:\n  -o O O O"s);
            }
        }

        SUBCASE("...name and metavar repeated N times for argument with metavar and nargs set as number...")
        {
            SUBCASE("...1")
            {
                parser.add_argument("-o").metavar("METAVARO").nargs(1);

                CHECK(parser.format_help() == "usage: prog [-o METAVARO]\n\noptional arguments:\n  -o METAVARO"s);
            }

            SUBCASE("...2")
            {
                parser.add_argument("-o").metavar("METAVARO").nargs(2);

                CHECK(parser.format_help() == "usage: prog [-o METAVARO METAVARO]\n\noptional arguments:\n  -o METAVARO METAVARO"s);
            }

            SUBCASE("...3")
            {
                parser.add_argument("-o").metavar("METAVARO").nargs(3);

                CHECK(parser.format_help() == "usage: prog [-o METAVARO METAVARO METAVARO]\n\noptional arguments:\n  -o METAVARO METAVARO METAVARO"s);
            }
        }

        SUBCASE("...name and choices repeated N times for argument with choices set and nargs set as number...")
        {
            SUBCASE("...1")
            {
                parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(1);

                CHECK(parser.format_help() == "usage: prog [-o {\"foo\",\"bar\"}]\n\noptional arguments:\n  -o {\"foo\",\"bar\"}"s);
            }

            SUBCASE("...2")
            {
                parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(2);

                CHECK(parser.format_help() == "usage: prog [-o {\"foo\",\"bar\"} {\"foo\",\"bar\"}]\n\noptional arguments:\n  -o {\"foo\",\"bar\"} {\"foo\",\"bar\"}"s);
            }

            SUBCASE("...3")
            {
                parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(3);

                CHECK(parser.format_help() == "usage: prog [-o {\"foo\",\"bar\"} {\"foo\",\"bar\"} {\"foo\",\"bar\"}]\n\noptional arguments:\n  -o {\"foo\",\"bar\"} {\"foo\",\"bar\"} {\"foo\",\"bar\"}"s);
            }
        }

        SUBCASE("...name followed by automatic metavar in brackets for argument with nargs set as zero_or_one")
        {
            parser.add_argument("-o").nargs(argparse::zero_or_one);

            CHECK(parser.format_help() == "usage: prog [-o [O]]\n\noptional arguments:\n  -o [O]"s);
        }

        SUBCASE("...name followed by metavar in brackets for argument with metavar set and nargs set as zero_or_one")
        {
            parser.add_argument("-o").metavar("METAVARO").nargs(argparse::zero_or_one);

            CHECK(parser.format_help() == "usage: prog [-o [METAVARO]]\n\noptional arguments:\n  -o [METAVARO]"s);
        }

        SUBCASE("...name followed by choices in brackets for argument with choices set and nargs set as zero_or_one")
        {
            parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_one);

            CHECK(parser.format_help() == "usage: prog [-o [{\"foo\",\"bar\"}]]\n\noptional arguments:\n  -o [{\"foo\",\"bar\"}]"s);
        }

        SUBCASE("...name followed by automatic metavar in brackets followed by automatic metavar and ellipsis in nested brackets for argument with nargs set as zero_or_more")
        {
            parser.add_argument("-o").nargs(argparse::zero_or_more);

            CHECK(parser.format_help() == "usage: prog [-o [O [O ...]]]\n\noptional arguments:\n  -o [O [O ...]]"s);
        }

        SUBCASE("...name followed by metavar in brackets followed by metavar and ellipsis in nested brackets for argument with metavar set and nargs set as zero_or_more")
        {
            parser.add_argument("-o").metavar("METAVARO").nargs(argparse::zero_or_more);

            CHECK(parser.format_help() == "usage: prog [-o [METAVARO [METAVARO ...]]]\n\noptional arguments:\n  -o [METAVARO [METAVARO ...]]"s);
        }

        SUBCASE("...name followed by choices in brackets followed by choices and ellipsis in nested brackets for argument with choices set and nargs set as zero_or_more")
        {
            parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_more);

            CHECK(parser.format_help() == "usage: prog [-o [{\"foo\",\"bar\"} [{\"foo\",\"bar\"} ...]]]\n\noptional arguments:\n  -o [{\"foo\",\"bar\"} [{\"foo\",\"bar\"} ...]]"s);
        }

        SUBCASE("...name followed by automatic metavar followed by automatic metavar and ellipsis in brackets for argument with nargs set as one_or_more")
        {
            parser.add_argument("-o").nargs(argparse::one_or_more);

            CHECK(parser.format_help() == "usage: prog [-o O [O ...]]\n\noptional arguments:\n  -o O [O ...]"s);
        }

        SUBCASE("...name followed by metavar followed by metavar and ellipsis in brackets for argument with metavar set and nargs set as one_or_more")
        {
            parser.add_argument("-o").metavar("METAVARO").nargs(argparse::one_or_more);

            CHECK(parser.format_help() == "usage: prog [-o METAVARO [METAVARO ...]]\n\noptional arguments:\n  -o METAVARO [METAVARO ...]"s);
        }

        SUBCASE("...name followed by choices followed by choices and ellipsis in brackets for argument with choices set and nargs set as one_or_more")
        {
            parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(argparse::one_or_more);

            CHECK(parser.format_help() == "usage: prog [-o {\"foo\",\"bar\"} [{\"foo\",\"bar\"} ...]]\n\noptional arguments:\n  -o {\"foo\",\"bar\"} [{\"foo\",\"bar\"} ...]"s);
        }
    }
}

TEST_CASE("Help string starts on 25th column...")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);

    SUBCASE("...of the same line for...")
    {
        SUBCASE("...positional arguments with length less than 21 characters")
        {
            parser.add_argument("pos").help("help");

            CHECK(parser.format_help() == "usage: prog pos\n\npositional arguments:\n  pos                   help");
        }

        SUBCASE("...optional arguments with length less than 21 characters")
        {
            parser.add_argument("-o").help("help");

            CHECK(parser.format_help() == "usage: prog [-o O]\n\noptional arguments:\n  -o O                  help");
        }
    }

    SUBCASE("...of the next line for...")
    {
        SUBCASE("...positional arguments with length of 21 characters or more")
        {
            parser.add_argument("abcdefghijklmnopqrstu").help("help");

            CHECK(parser.format_help() == "usage: prog abcdefghijklmnopqrstu\n\npositional arguments:\n  abcdefghijklmnopqrstu\n                        help");
        }

        SUBCASE("...optional arguments with length of 21 characters or more")
        {
            parser.add_argument("--abcdefghijklmnopq").metavar("A").help("help");

            CHECK(parser.format_help() == "usage: prog [--abcdefghijklmnopq A]\n\noptional arguments:\n  --abcdefghijklmnopq A\n                        help");
        }
    }
}
