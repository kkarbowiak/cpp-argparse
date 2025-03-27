#include "argparse.h"

#include "cstring_array.h"

#include "doctest.h"

#include <string>


using namespace std::string_literals;

TEST_CASE("ArgumentParser provides help message with usage section")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);

    CHECK(parser.format_help() == "usage: prog"s);
}

TEST_CASE("ArgumentParser provides help message with usage section and description for description and no arguments")
{
    auto parser = argparse::ArgumentParser().prog("prog").description("A foo that bars").add_help(false);

    CHECK(parser.format_help() == "usage: prog\n"
                                  "\n"
                                  "A foo that bars"s);
}

TEST_CASE("ArgumentParser provides help message with usage section and epilog for epilog and no arguments")
{
    auto parser = argparse::ArgumentParser().prog("prog").epilog("And that's how you'd foo a bar").add_help(false);

    CHECK(parser.format_help() == "usage: prog\n"
                                  "\n"
                                  "And that's how you'd foo a bar"s);
}

TEST_CASE("ArgumentParser provides help message with usage section and positional arguments section for one positional argument")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1");

    CHECK(parser.format_help() == "usage: prog p1\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  p1"s);
}

TEST_CASE("ArgumentParser provides help message with usage section and positional arguments section for two positional arguments")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1");
    parser.add_argument("p2");

    CHECK(parser.format_help() == "usage: prog p1 p2\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  p1\n"
                                  "  p2"s);
}

TEST_CASE("ArgumentParser provides help message with usage section and positional arguments section for three positional arguments")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1");
    parser.add_argument("p2");
    parser.add_argument("p3");

    CHECK(parser.format_help() == "usage: prog p1 p2 p3\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  p1\n"
                                  "  p2\n"
                                  "  p3"s);
}

TEST_CASE("ArgumentParser provides help message with usage section and optional arguments section for one optional argument")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o");

    CHECK(parser.format_help() == "usage: prog [-o O]\n"
                                  "\n"
                                  "optional arguments:\n"
                                  "  -o O"s);
}

TEST_CASE("ArgumentParser provides help message with usage section and optional arguments section for two optional arguments")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o");
    parser.add_argument("--option");

    CHECK(parser.format_help() == "usage: prog [-o O] [--option OPTION]\n"
                                  "\n"
                                  "optional arguments:\n"
                                  "  -o O\n"
                                  "  --option OPTION"s);
}

TEST_CASE("ArgumentParser provides help message with usage section and optional arguments section for three optional arguments")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o");
    parser.add_argument("--option");
    parser.add_argument("--very-long-name");

    CHECK(parser.format_help() == "usage: prog [-o O] [--option OPTION] [--very-long-name VERY_LONG_NAME]\n"
                                  "\n"
                                  "optional arguments:\n"
                                  "  -o O\n"
                                  "  --option OPTION\n"
                                  "  --very-long-name VERY_LONG_NAME"s);
}

TEST_CASE("ArgumentParser provides help message with usage section, positional arguments section, and optional arguments section for one positional and one optional argument")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1");
    parser.add_argument("-o");

    CHECK(parser.format_help() == "usage: prog [-o O] p1\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  p1\n"
                                  "\n"
                                  "optional arguments:\n"
                                  "  -o O"s);
}

TEST_CASE("ArgumentParser provides help message with usage section, positional arguments section, and optional arguments section for three positional and three optional arguments")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1");
    parser.add_argument("-o");
    parser.add_argument("p2");
    parser.add_argument("-a");
    parser.add_argument("p3");
    parser.add_argument("-z");

    CHECK(parser.format_help() == "usage: prog [-o O] [-a A] [-z Z] p1 p2 p3\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  p1\n"
                                  "  p2\n"
                                  "  p3\n"
                                  "\n"
                                  "optional arguments:\n"
                                  "  -o O\n"
                                  "  -a A\n"
                                  "  -z Z"s);
}

TEST_CASE("Help message contains for positional argument name")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1");

    CHECK(parser.format_help() == "usage: prog p1\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  p1"s);
}

TEST_CASE("Help message contains for positional argument metavar for argument with metavar set")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").metavar("metap1");

    CHECK(parser.format_help() == "usage: prog metap1\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  metap1"s);
}

TEST_CASE("Help message contains for positional argument name and help for argument with help string")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").help("help1");

    CHECK(parser.format_help() == "usage: prog p1\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  p1                    help1"s);
}

TEST_CASE("Help message contains for positional argument choices for argument with choices set")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").choices({"foo"s, "bar"s});

    CHECK(parser.format_help() == "usage: prog {\"foo\",\"bar\"}\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  {\"foo\",\"bar\"}"s);
}

TEST_CASE("Help message contains for positional argument name for argument with nargs set as number 1")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").nargs(1);

    CHECK(parser.format_help() == "usage: prog p1\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  p1"s);
}

TEST_CASE("Help message contains for positional argument name for argument with nargs set as number 2")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").nargs(2);

    CHECK(parser.format_help() == "usage: prog p1 p1\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  p1"s);
}

TEST_CASE("Help message contains for positional argument name for argument with nargs set as number 3")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").nargs(3);

    CHECK(parser.format_help() == "usage: prog p1 p1 p1\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  p1"s);
}

TEST_CASE("Help message contains for positional argument metavar for argument with metavar set and nargs set as number 1")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").metavar("metap1").nargs(1);

    CHECK(parser.format_help() == "usage: prog metap1\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  metap1"s);
}

TEST_CASE("Help message contains for positional argument metavar for argument with metavar set and nargs set as number 2")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").metavar("metap1").nargs(2);

    CHECK(parser.format_help() == "usage: prog metap1 metap1\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  metap1"s);
}

TEST_CASE("Help message contains for positional argument metavar for argument with metavar set and nargs set as number 3")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").metavar("metap1").nargs(3);

    CHECK(parser.format_help() == "usage: prog metap1 metap1 metap1\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  metap1"s);
}

TEST_CASE("Help message contains for positional argument choices for argument with choices set and nargs set as number 1")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(1);

    CHECK(parser.format_help() == "usage: prog {\"foo\",\"bar\"}\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  {\"foo\",\"bar\"}"s);
}

TEST_CASE("Help message contains for positional argument choices for argument with choices set and nargs set as number 2")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(2);

    CHECK(parser.format_help() == "usage: prog {\"foo\",\"bar\"} {\"foo\",\"bar\"}\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  {\"foo\",\"bar\"}"s);
}

TEST_CASE("Help message contains for positional argument choices for argument with choices set and nargs set as number 3")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(3);

    CHECK(parser.format_help() == "usage: prog {\"foo\",\"bar\"} {\"foo\",\"bar\"} {\"foo\",\"bar\"}\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  {\"foo\",\"bar\"}"s);
}

TEST_CASE("Help message contains for positional argument name for argument with nargs set as zero_or_one")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").nargs(argparse::zero_or_one);

    CHECK(parser.format_help() == "usage: prog [p1]\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  p1"s);
}

TEST_CASE("Help message contains for positional argument metavar for argument with metavar set and nargs set as zero_or_one")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").metavar("metap1").nargs(argparse::zero_or_one);

    CHECK(parser.format_help() == "usage: prog [metap1]\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  metap1"s);
}

TEST_CASE("Help message contains for positional argument choices for argument with choices set and nargs set as zero_or_one")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_one);

    CHECK(parser.format_help() == "usage: prog [{\"foo\",\"bar\"}]\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  {\"foo\",\"bar\"}"s);
}

TEST_CASE("Help message contains for positional argument name for argument with nargs set as zero_or_more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").nargs(argparse::zero_or_more);

    CHECK(parser.format_help() == "usage: prog [p1 [p1 ...]]\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  p1"s);
}

TEST_CASE("Help message contains for positional argument metavar for argument with metavar set and nargs set as zero_or_more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").metavar("metap1").nargs(argparse::zero_or_more);

    CHECK(parser.format_help() == "usage: prog [metap1 [metap1 ...]]\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  metap1"s);
}

TEST_CASE("Help message contains for positional argument choices for argument with choices set and nargs set as zero_or_more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_more);

    CHECK(parser.format_help() == "usage: prog [{\"foo\",\"bar\"} [{\"foo\",\"bar\"} ...]]\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  {\"foo\",\"bar\"}"s);
}

TEST_CASE("Help message contains for positional argument name for argument with nargs set as one_or_more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").nargs(argparse::one_or_more);

    CHECK(parser.format_help() == "usage: prog p1 [p1 ...]\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  p1"s);
}

TEST_CASE("Help message contains for positional argument metavar for argument with metavar set and nargs set as one_or_more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").metavar("metap1").nargs(argparse::one_or_more);

    CHECK(parser.format_help() == "usage: prog metap1 [metap1 ...]\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  metap1"s);
}

TEST_CASE("Help message contains for positional argument choices for argument with choices set and nargs set as one_or_more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(argparse::one_or_more);

    CHECK(parser.format_help() == "usage: prog {\"foo\",\"bar\"} [{\"foo\",\"bar\"} ...]\n"
                                  "\n"
                                  "positional arguments:\n"
                                  "  {\"foo\",\"bar\"}"s);
}

TEST_CASE("Help message contains for optional argument name for argument with store true action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").action(argparse::store_true);

    CHECK(parser.format_help() == "usage: prog [-o]\n"
                                  "\n"
                                  "optional arguments:\n"
                                  "  -o"s);
}

TEST_CASE("Help message contains for optional argument name and long name for argument with store true action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o", "--option").action(argparse::store_true);

    CHECK(parser.format_help() == "usage: prog [-o]\n"
                                  "\n"
                                  "optional arguments:\n"
                                  "  -o, --option"s);
}

TEST_CASE("Help message contains for optional argument name and help for argument with store true action and help string")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").action(argparse::store_true).help("help1");

    CHECK(parser.format_help() == "usage: prog [-o]\n"
                                  "\n"
                                  "optional arguments:\n"
                                  "  -o                    help1"s);
}

TEST_CASE("Help message contains for optional argument name for argument with store false action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").action(argparse::store_false);

    CHECK(parser.format_help() == "usage: prog [-o]\n"
                                  "\n"
                                  "optional arguments:\n"
                                  "  -o"s);
}

TEST_CASE("Help message contains for optional argument name and long name for argument with store false action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o", "--option").action(argparse::store_false);

    CHECK(parser.format_help() == "usage: prog [-o]\n"
                                  "\n"
                                  "optional arguments:\n"
                                  "  -o, --option"s);
}

TEST_CASE("Help message contains for optional argument name and long name for argument with store false action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o", "--option").action(argparse::store_false);

    CHECK(parser.format_help() == "usage: prog [-o]\n"
                                  "\n"
                                  "optional arguments:\n"
                                  "  -o, --option"s);
}

TEST_CASE("Help message contains for optional argument name and help for argument with store false action and help string")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").action(argparse::store_false).help("help1");

    CHECK(parser.format_help() == "usage: prog [-o]\n\noptional arguments:\n  -o                    help1"s);
}

TEST_CASE("Help message contains for optional argument name for argument with store const action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").action(argparse::store_const);

    CHECK(parser.format_help() == "usage: prog [-o]\n\noptional arguments:\n  -o"s);
}

TEST_CASE("Help message contains for optional argument name and long name for argument with store const action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o", "--option").action(argparse::store_const);

    CHECK(parser.format_help() == "usage: prog [-o]\n\noptional arguments:\n  -o, --option"s);
}

TEST_CASE("Help message contains for optional argument name and help for argument with store const action and help string")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").action(argparse::store_const).help("help1");

    CHECK(parser.format_help() == "usage: prog [-o]\n\noptional arguments:\n  -o                    help1"s);
}

TEST_CASE("Help message contains for optional argument name for argument with count action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-c").action(argparse::count);

    CHECK(parser.format_help() == "usage: prog [-c]\n\noptional arguments:\n  -c"s);
}

TEST_CASE("Help message contains for optional argument name and long name for argument with count action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-c", "--count").action(argparse::count);

    CHECK(parser.format_help() == "usage: prog [-c]\n\noptional arguments:\n  -c, --count"s);
}

TEST_CASE("Help message contains for optional argument name and help for argument with count action and help string")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-c").action(argparse::count).help("help1");

    CHECK(parser.format_help() == "usage: prog [-c]\n\noptional arguments:\n  -c                    help1"s);
}

TEST_CASE("Help message contains for optional argument name and automatic metavar for argument with append action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-a").action(argparse::append);

    CHECK(parser.format_help() == "usage: prog [-a A]\n\noptional arguments:\n  -a A"s);
}

TEST_CASE("Help message contains for optional argument name, long name, and automatic metavar for argument with append action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-a", "--append").action(argparse::append);

    CHECK(parser.format_help() == "usage: prog [-a APPEND]\n\noptional arguments:\n  -a APPEND, --append APPEND"s);
}

TEST_CASE("Help message contains for optional argument name, long name, automatic metavar, and help string for argument with append action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-a", "--append").action(argparse::append).help("help1");

    CHECK(parser.format_help() == "usage: prog [-a APPEND]\n\noptional arguments:\n  -a APPEND, --append APPEND\n                        help1"s);
}

TEST_CASE("Help message contains for optional argument name for argument with help action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-h").action(argparse::help);

    CHECK(parser.format_help() == "usage: prog [-h]\n\noptional arguments:\n  -h"s);
}

TEST_CASE("Help message contains for optional argument name and long name for argument with help action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-h", "--help").action(argparse::help);

    CHECK(parser.format_help() == "usage: prog [-h]\n\noptional arguments:\n  -h, --help"s);
}

TEST_CASE("Help message contains for optional argument name and help for argument with help action and help string")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-h").action(argparse::help).help("help1");

    CHECK(parser.format_help() == "usage: prog [-h]\n\noptional arguments:\n  -h                    help1"s);
}

TEST_CASE("Help message contains for optional argument name and help for automatically added help argument")
{
    auto parser = argparse::ArgumentParser().prog("prog");

    CHECK(parser.format_help() == "usage: prog [-h]\n\noptional arguments:\n  -h, --help            show this help message and exit"s);
}

TEST_CASE("Help message contains for optional argument name and automatically added help for argument with version action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-v").action(argparse::version);

    CHECK(parser.format_help() == "usage: prog [-v]\n\noptional arguments:\n  -v                    show program's version number and exit"s);
}

TEST_CASE("Help message contains for optional argument name, long name, and automatically added help for argument with version action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-v", "--version").action(argparse::version);

    CHECK(parser.format_help() == "usage: prog [-v]\n\noptional arguments:\n  -v, --version         show program's version number and exit"s);
}

TEST_CASE("Help message contains for optional argument name and help for argument with version action and help string")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-v").action(argparse::version).help("version1");

    CHECK(parser.format_help() == "usage: prog [-v]\n\noptional arguments:\n  -v                    version1"s);
}

TEST_CASE("Help message contains for optional argument name and automatic metavar")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o");

    CHECK(parser.format_help() == "usage: prog [-o O]\n\noptional arguments:\n  -o O"s);
}

TEST_CASE("Help message contains for optional argument name and metavar for argument with metavar")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").metavar("METAVARO");

    CHECK(parser.format_help() == "usage: prog [-o METAVARO]\n\noptional arguments:\n  -o METAVARO"s);
}

TEST_CASE("Help message contains for optional argument name, automatic metavar, and help for argument with help string")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").help("help1");

    CHECK(parser.format_help() == "usage: prog [-o O]\n\noptional arguments:\n  -o O                  help1"s);
}

TEST_CASE("Help message contains for optional argument no brackets for argument with required true")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").required(true);

    CHECK(parser.format_help() == "usage: prog -o O\n\noptional arguments:\n  -o O"s);
}

TEST_CASE("Help message contains for optional argument brackets for argument with required false")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").required(false);

    CHECK(parser.format_help() == "usage: prog [-o O]\n\noptional arguments:\n  -o O"s);
}

TEST_CASE("Help message contains for optional argument name and choices for argument with choices set")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").choices({"foo"s, "bar"s});

    CHECK(parser.format_help() == "usage: prog [-o {\"foo\",\"bar\"}]\n\noptional arguments:\n  -o {\"foo\",\"bar\"}"s);
}

TEST_CASE("Help message contains for optional argument name and automatic metavar repeated N times for argument with nargs set as number 1")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").nargs(1);

    CHECK(parser.format_help() == "usage: prog [-o O]\n\noptional arguments:\n  -o O"s);
}

TEST_CASE("Help message contains for optional argument name and automatic metavar repeated N times for argument with nargs set as number 2")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").nargs(2);

    CHECK(parser.format_help() == "usage: prog [-o O O]\n\noptional arguments:\n  -o O O"s);
}

TEST_CASE("Help message contains for optional argument name and automatic metavar repeated N times for argument with nargs set as number 3")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").nargs(3);

    CHECK(parser.format_help() == "usage: prog [-o O O O]\n\noptional arguments:\n  -o O O O"s);
}

TEST_CASE("Help message contains for optional argument name and metavar repeated N times for argument with metavar and nargs set as number 1")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").metavar("METAVARO").nargs(1);

    CHECK(parser.format_help() == "usage: prog [-o METAVARO]\n\noptional arguments:\n  -o METAVARO"s);
}

TEST_CASE("Help message contains for optional argument name and metavar repeated N times for argument with metavar and nargs set as number 2")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").metavar("METAVARO").nargs(2);

    CHECK(parser.format_help() == "usage: prog [-o METAVARO METAVARO]\n\noptional arguments:\n  -o METAVARO METAVARO"s);
}

TEST_CASE("Help message contains for optional argument name and metavar repeated N times for argument with metavar and nargs set as number 3")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").metavar("METAVARO").nargs(3);

    CHECK(parser.format_help() == "usage: prog [-o METAVARO METAVARO METAVARO]\n\noptional arguments:\n  -o METAVARO METAVARO METAVARO"s);
}

TEST_CASE("Help message contains for optional argument name and choices repeated N times for argument with choices set and nargs set as number 1")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(1);

    CHECK(parser.format_help() == "usage: prog [-o {\"foo\",\"bar\"}]\n\noptional arguments:\n  -o {\"foo\",\"bar\"}"s);
}

TEST_CASE("Help message contains for optional argument name and choices repeated N times for argument with choices set and nargs set as number 2")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(2);

    CHECK(parser.format_help() == "usage: prog [-o {\"foo\",\"bar\"} {\"foo\",\"bar\"}]\n\noptional arguments:\n  -o {\"foo\",\"bar\"} {\"foo\",\"bar\"}"s);
}

TEST_CASE("Help message contains for optional argument name and choices repeated N times for argument with choices set and nargs set as number 3")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(3);

    CHECK(parser.format_help() == "usage: prog [-o {\"foo\",\"bar\"} {\"foo\",\"bar\"} {\"foo\",\"bar\"}]\n\noptional arguments:\n  -o {\"foo\",\"bar\"} {\"foo\",\"bar\"} {\"foo\",\"bar\"}"s);
}

TEST_CASE("Help message contains for optional argument name followed by automatic metavar in brackets for argument with nargs set as zero_or_one")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").nargs(argparse::zero_or_one);

    CHECK(parser.format_help() == "usage: prog [-o [O]]\n\noptional arguments:\n  -o [O]"s);
}

TEST_CASE("Help message contains for optional argument name followed by metavar in brackets for argument with metavar set and nargs set as zero_or_one")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").metavar("METAVARO").nargs(argparse::zero_or_one);

    CHECK(parser.format_help() == "usage: prog [-o [METAVARO]]\n\noptional arguments:\n  -o [METAVARO]"s);
}

TEST_CASE("Help message contains for optional argument name followed by choices in brackets for argument with choices set and nargs set as zero_or_one")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_one);

    CHECK(parser.format_help() == "usage: prog [-o [{\"foo\",\"bar\"}]]\n\noptional arguments:\n  -o [{\"foo\",\"bar\"}]"s);
}

TEST_CASE("Help message contains for optional argument name followed by automatic metavar in brackets followed by automatic metavar and ellipsis in nested brackets for argument with nargs set as zero_or_more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").nargs(argparse::zero_or_more);

    CHECK(parser.format_help() == "usage: prog [-o [O [O ...]]]\n\noptional arguments:\n  -o [O [O ...]]"s);
}

TEST_CASE("Help message contains for optional argument name followed by metavar in brackets followed by metavar and ellipsis in nested brackets for argument with metavar set and nargs set as zero_or_more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").metavar("METAVARO").nargs(argparse::zero_or_more);

    CHECK(parser.format_help() == "usage: prog [-o [METAVARO [METAVARO ...]]]\n\noptional arguments:\n  -o [METAVARO [METAVARO ...]]"s);
}

TEST_CASE("Help message contains for optional argument name followed by choices in brackets followed by choices and ellipsis in nested brackets for argument with choices set and nargs set as zero_or_more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_more);

    CHECK(parser.format_help() == "usage: prog [-o [{\"foo\",\"bar\"} [{\"foo\",\"bar\"} ...]]]\n\noptional arguments:\n  -o [{\"foo\",\"bar\"} [{\"foo\",\"bar\"} ...]]"s);
}

TEST_CASE("Help message contains for optional argument name followed by automatic metavar followed by automatic metavar and ellipsis in brackets for argument with nargs set as one_or_more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").nargs(argparse::one_or_more);

    CHECK(parser.format_help() == "usage: prog [-o O [O ...]]\n\noptional arguments:\n  -o O [O ...]"s);
}

TEST_CASE("Help message contains for optional argument name followed by metavar followed by metavar and ellipsis in brackets for argument with metavar set and nargs set as one_or_more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").metavar("METAVARO").nargs(argparse::one_or_more);

    CHECK(parser.format_help() == "usage: prog [-o METAVARO [METAVARO ...]]\n\noptional arguments:\n  -o METAVARO [METAVARO ...]"s);
}

TEST_CASE("Help message contains for optional argument name followed by choices followed by choices and ellipsis in brackets for argument with choices set and nargs set as one_or_more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(argparse::one_or_more);

    CHECK(parser.format_help() == "usage: prog [-o {\"foo\",\"bar\"} [{\"foo\",\"bar\"} ...]]\n\noptional arguments:\n  -o {\"foo\",\"bar\"} [{\"foo\",\"bar\"} ...]"s);
}

TEST_CASE("Help string starts on 25th column of the same line for positional arguments with length less than 21 characters")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("pos").help("help");

    CHECK(parser.format_help() == "usage: prog pos\n\npositional arguments:\n  pos                   help");
}

TEST_CASE("Help string starts on 25th column of the same line for optional arguments with length less than 21 characters")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").help("help");

    CHECK(parser.format_help() == "usage: prog [-o O]\n\noptional arguments:\n  -o O                  help");
}

TEST_CASE("Help string starts on 25th column of the next line for positional arguments with length of 21 characters or more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("abcdefghijklmnopqrstu").help("help");

    CHECK(parser.format_help() == "usage: prog abcdefghijklmnopqrstu\n\npositional arguments:\n  abcdefghijklmnopqrstu\n                        help");
}

TEST_CASE("Help string starts on 25th column of the next line for optional arguments with length of 21 characters or more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("--abcdefghijklmnopq").metavar("A").help("help");

    CHECK(parser.format_help() == "usage: prog [--abcdefghijklmnopq A]\n\noptional arguments:\n  --abcdefghijklmnopq A\n                        help");
}

TEST_CASE("ArgumentParser replaces '{prog}' with program name in description text")
{
    auto parser = argparse::ArgumentParser().prog("program").description("A {prog} that bars").add_help(false);

    CHECK(parser.format_help() == "usage: program\n\nA program that bars"s);
}

TEST_CASE("ArgumentParser replaces '{prog}' with program name in description text")
{
    auto parser = argparse::ArgumentParser().prog("program").description("A {prog} that {prog}s").add_help(false);

    CHECK(parser.format_help() == "usage: program\n\nA program that programs"s);
}

TEST_CASE("ArgumentParser replaces '{prog}' with program name in epilog text")
{
    auto parser = argparse::ArgumentParser().prog("program").epilog("And that's how you'd foo a bar using {prog}").add_help(false);

    CHECK(parser.format_help() == "usage: program\n\nAnd that's how you'd foo a bar using program"s);
}

TEST_CASE("ArgumentParser replaces '{prog}' with program name in epilog text")
{
    auto parser = argparse::ArgumentParser().prog("program").epilog("And that's how you'd {prog} a bar using {prog}").add_help(false);

    CHECK(parser.format_help() == "usage: program\n\nAnd that's how you'd program a bar using program"s);
}

TEST_CASE("ArgumentParser replaces '{prog}' with program name in positional argument help message")
{
    auto parser = argparse::ArgumentParser().prog("program").add_help(false);
    parser.add_argument("p1").help("p1 of the {prog} itself");

    CHECK(parser.format_help() == "usage: program p1\n\npositional arguments:\n  p1                    p1 of the program itself"s);
}

TEST_CASE("ArgumentParser replaces '{prog}' with program name in optional argument help message")
{
    auto parser = argparse::ArgumentParser().prog("program").add_help(false);
    parser.add_argument("-o").help("option of the {prog} itself");

    CHECK(parser.format_help() == "usage: program [-o O]\n\noptional arguments:\n  -o O                  option of the program itself");
}

TEST_CASE("ArgumentParser replaces '{prog}' with program name taken from first command-line parameter")
{
    auto parser = argparse::ArgumentParser().add_help(false).description("This is {prog}.").epilog("This was {prog}.");
    parser.add_argument("pos").help("this is {prog}'s positional argument");
    parser.add_argument("-o").help("this is {prog}'s optional argument");

    parser.parse_args(2, cstr_arr{"program", "p"});

    CHECK(parser.format_help() == "usage: program [-o O] pos\n\nThis is program.\n\npositional arguments:\n  pos                   this is program's positional argument\n\noptional arguments:\n  -o O                  this is program's optional argument\n\nThis was program."s);
}
