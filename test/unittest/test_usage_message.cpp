#include "argparse.h"

#include "doctest.h"

#include <string>


using namespace std::string_literals;

TEST_CASE("ArgumentParser provides usage message with prog name")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);

    CHECK(parser.format_usage() == "usage: prog"s);
}

TEST_CASE("ArgumentParser provides usage message with prog name and list of positional arguments for one positional argument")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1");

    CHECK(parser.format_usage() == "usage: prog p1"s);
}

TEST_CASE("ArgumentParser provides usage message with prog name and list of positional arguments for two positional arguments")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1");
    parser.add_argument("p2");

    CHECK(parser.format_usage() == "usage: prog p1 p2"s);
}

TEST_CASE("ArgumentParser provides usage message with prog name and list of positional arguments for three positional arguments")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1");
    parser.add_argument("p2");
    parser.add_argument("p3");

    CHECK(parser.format_usage() == "usage: prog p1 p2 p3"s);
}

TEST_CASE("ArgumentParser provides usage message with prog name and list of optional arguments for one optional argument")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o");

    CHECK(parser.format_usage() == "usage: prog [-o O]"s);
}

TEST_CASE("ArgumentParser provides usage message with prog name and list of optional arguments for two optional arguments")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o");
    parser.add_argument("--option");

    CHECK(parser.format_usage() == "usage: prog [-o O] [--option OPTION]"s);
}

TEST_CASE("ArgumentParser provides usage message with prog name and list of optional arguments for three optional arguments")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o");
    parser.add_argument("--option");
    parser.add_argument("--very-long-name");

    CHECK(parser.format_usage() == "usage: prog [-o O] [--option OPTION] [--very-long-name VERY_LONG_NAME]"s);
}

TEST_CASE("ArgumentParser provides usage message for one optional argument with short and long name")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-f", "--foo");

    CHECK(parser.format_usage() == "usage: prog [-f FOO]"s);
}

TEST_CASE("ArgumentParser provides usage message for one optional argument with store true action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").action(argparse::store_true);

    CHECK(parser.format_usage() == "usage: prog [-o]"s);
}

TEST_CASE("ArgumentParser provides usage message for two optional arguments with store true action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").action(argparse::store_true);
    parser.add_argument("--option").action(argparse::store_true);

    CHECK(parser.format_usage() == "usage: prog [-o] [--option]"s);
}

TEST_CASE("ArgumentParser provides usage message for one optional argument with store false action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").action(argparse::store_false);

    CHECK(parser.format_usage() == "usage: prog [-o]"s);
}

TEST_CASE("ArgumentParser provides usage message for two optional arguments with store false action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").action(argparse::store_false);
    parser.add_argument("--option").action(argparse::store_false);

    CHECK(parser.format_usage() == "usage: prog [-o] [--option]"s);
}

TEST_CASE("ArgumentParser provides usage message for one optional argument with store const action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").action(argparse::store_const);

    CHECK(parser.format_usage() == "usage: prog [-o]"s);
}

TEST_CASE("ArgumentParser provides usage message for two optional arguments with store const action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").action(argparse::store_const);
    parser.add_argument("--option").action(argparse::store_const);

    CHECK(parser.format_usage() == "usage: prog [-o] [--option]"s);
}

TEST_CASE("ArgumentParser provides usage message for one optional argument with help action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-h").action(argparse::help);

    CHECK(parser.format_usage() == "usage: prog [-h]"s);
}

TEST_CASE("ArgumentParser provides usage message for one optional argument with version action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-v").action(argparse::version);

    CHECK(parser.format_usage() == "usage: prog [-v]"s);
}

TEST_CASE("ArgumentParser provides usage message for one positional and one optional argument")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1");
    parser.add_argument("-o");

    CHECK(parser.format_usage() == "usage: prog [-o O] p1"s);
}

TEST_CASE("ArgumentParser provides usage message for three positional and three optional arguments")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1");
    parser.add_argument("-o");
    parser.add_argument("p2");
    parser.add_argument("-a");
    parser.add_argument("p3");
    parser.add_argument("-z");

    CHECK(parser.format_usage() == "usage: prog [-o O] [-a A] [-z Z] p1 p2 p3"s);
}

TEST_CASE("Usage message contains for positional argument its name")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1");

    CHECK(parser.format_usage() == "usage: prog p1"s);
}

TEST_CASE("Usage message contains for positional argument metavar for argument with metavar set")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").metavar("metap1");

    CHECK(parser.format_usage() == "usage: prog metap1"s);
}

TEST_CASE("Usage message contains for positional argument choices for argument with choices set")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").choices({"foo"s, "bar"s});

    CHECK(parser.format_usage() == "usage: prog {\"foo\",\"bar\"}"s);
}

TEST_CASE("Usage message contains for positional argument its name repeated N times for argument with nargs set as number 1")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").nargs(1);

    CHECK(parser.format_usage() == "usage: prog p1"s);
}

TEST_CASE("Usage message contains for positional argument its name repeated N times for argument with nargs set as number 2")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").nargs(2);

    CHECK(parser.format_usage() == "usage: prog p1 p1"s);
}

TEST_CASE("Usage message contains for positional argument its name repeated N times for argument with nargs set as number 3")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").nargs(3);

    CHECK(parser.format_usage() == "usage: prog p1 p1 p1"s);
}

TEST_CASE("Usage message contains for positional argument choices repeated N times for argument with choices set and nargs set as number 1")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(1);

    CHECK(parser.format_usage() == "usage: prog {\"foo\",\"bar\"}"s);
}

TEST_CASE("Usage message contains for positional argument choices repeated N times for argument with choices set and nargs set as number 2")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(2);

    CHECK(parser.format_usage() == "usage: prog {\"foo\",\"bar\"} {\"foo\",\"bar\"}"s);
}

TEST_CASE("Usage message contains for positional argument choices repeated N times for argument with choices set and nargs set as number 3")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(3);

    CHECK(parser.format_usage() == "usage: prog {\"foo\",\"bar\"} {\"foo\",\"bar\"} {\"foo\",\"bar\"}"s);
}

TEST_CASE("Usage message contains for positional argument its name in brackets for argument with nargs set as zero_or_one")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").nargs(argparse::zero_or_one);

    CHECK(parser.format_usage() == "usage: prog [p1]"s);
}

TEST_CASE("Usage message contains for positional argument choices in brackets for argument with choices set and nargs set as zero_or_one")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_one);

    CHECK(parser.format_usage() == "usage: prog [{\"foo\",\"bar\"}]"s);
}

TEST_CASE("Usage message contains for positional argument its name in brackets followed by its name and ellipsis in brackets for argument with nargs set as zero_or_more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").nargs(argparse::zero_or_more);

    CHECK(parser.format_usage() == "usage: prog [p1 [p1 ...]]"s);
}

TEST_CASE("Usage message contains for positional argument choices in brackets followed by choices and ellipsis in brackets for argument with choices set and nargs set as zero_or_more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_more);

    CHECK(parser.format_usage() == "usage: prog [{\"foo\",\"bar\"} [{\"foo\",\"bar\"} ...]]"s);
}

TEST_CASE("Usage message contains for positional argument its name followed by its name and ellipsis in brackets for argument with nargs set as one_or_more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").nargs(argparse::one_or_more);

    CHECK(parser.format_usage() == "usage: prog p1 [p1 ...]"s);
}

TEST_CASE("Usage message contains for positional argument choices followed by choices and ellipsis in brackets for argument with choices set and nargs set as one_or_more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("p1").choices({"foo"s, "bar"s}).nargs(argparse::one_or_more);

    CHECK(parser.format_usage() == "usage: prog {\"foo\",\"bar\"} [{\"foo\",\"bar\"} ...]"s);
}

TEST_CASE("Usage message contains for optional argument name for argument with store true action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").action(argparse::store_true);

    CHECK(parser.format_usage() == "usage: prog [-o]"s);
}

TEST_CASE("Usage message contains for optional argument name for argument with store false action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").action(argparse::store_false);

    CHECK(parser.format_usage() == "usage: prog [-o]"s);
}

TEST_CASE("Usage message contains for optional argument name for argument with store const action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").action(argparse::store_const);

    CHECK(parser.format_usage() == "usage: prog [-o]"s);
}

TEST_CASE("Usage message contains for optional argument name for argument with help action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-h").action(argparse::store_false);

    CHECK(parser.format_usage() == "usage: prog [-h]"s);
}

TEST_CASE("Usage message contains for optional argument name for argument with version action")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-v").action(argparse::version);

    CHECK(parser.format_usage() == "usage: prog [-v]"s);
}

TEST_CASE("Usage message contains for optional argument name and automatic metavar")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o");

    CHECK(parser.format_usage() == "usage: prog [-o O]"s);
}

TEST_CASE("Usage message contains for optional argument name and metavar for argument with metavar set")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").metavar("METAVARO");

    CHECK(parser.format_usage() == "usage: prog [-o METAVARO]"s);
}

TEST_CASE("Usage message contains for optional argument no brackets for argument with required true and automatic metavar")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").required(true);

    CHECK(parser.format_usage() == "usage: prog -o O"s);
}

TEST_CASE("Usage message contains for optional argument no brackets for argument with required true and metavar")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").metavar("METAVARO").required(true);

    CHECK(parser.format_usage() == "usage: prog -o METAVARO"s);
}

TEST_CASE("Usage message contains for optional argument no brackets for argument with required true and choices")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").choices({"foo"s, "bar"s}).required(true);

    CHECK(parser.format_usage() == "usage: prog -o {\"foo\",\"bar\"}"s);
}

TEST_CASE("Usage message contains for optional argument no brackets for argument with required true and nargs")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").nargs(1).required(true);

    CHECK(parser.format_usage() == "usage: prog -o O"s);
}

TEST_CASE("Usage message contains for optional argument brackets for argument with required false")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").required(false);

    CHECK(parser.format_usage() == "usage: prog [-o O]"s);
}

TEST_CASE("Usage message contains for optional argument name and choices for argument with choices set")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").choices({"foo"s, "bar"s});

    CHECK(parser.format_usage() == "usage: prog [-o {\"foo\",\"bar\"}]"s);
}

TEST_CASE("Usage message contains for optional argument its name and automatic metavar repeated N times for argument with nargs set as number 1")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").nargs(1);

    CHECK(parser.format_usage() == "usage: prog [-o O]"s);
}

TEST_CASE("Usage message contains for optional argument its name and automatic metavar repeated N times for argument with nargs set as number 2")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").nargs(2);

    CHECK(parser.format_usage() == "usage: prog [-o O O]"s);
}

TEST_CASE("Usage message contains for optional argument its name and automatic metavar repeated N times for argument with nargs set as number 2")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").nargs(3);

    CHECK(parser.format_usage() == "usage: prog [-o O O O]"s);
}

TEST_CASE("Usage message contains for optional argument its name and metavar repeated N times for argument with metavar set and nargs set as number 1")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").metavar("METAVARO").nargs(1);

    CHECK(parser.format_usage() == "usage: prog [-o METAVARO]"s);
}

TEST_CASE("Usage message contains for optional argument its name and metavar repeated N times for argument with metavar set and nargs set as number 2")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").metavar("METAVARO").nargs(2);

    CHECK(parser.format_usage() == "usage: prog [-o METAVARO METAVARO]"s);
}

TEST_CASE("Usage message contains for optional argument its name and metavar repeated N times for argument with metavar set and nargs set as number 3")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").metavar("METAVARO").nargs(3);

    CHECK(parser.format_usage() == "usage: prog [-o METAVARO METAVARO METAVARO]"s);
}

TEST_CASE("Usage message contains for optional argument its name and choices repeated N times for argument with choices set and nargs set as number 1")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(1);

    CHECK(parser.format_usage() == "usage: prog [-o {\"foo\",\"bar\"}]"s);
}

TEST_CASE("Usage message contains for optional argument its name and choices repeated N times for argument with choices set and nargs set as number 2")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(2);

    CHECK(parser.format_usage() == "usage: prog [-o {\"foo\",\"bar\"} {\"foo\",\"bar\"}]"s);
}

TEST_CASE("Usage message contains for optional argument its name and choices repeated N times for argument with choices set and nargs set as number 3")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(3);

    CHECK(parser.format_usage() == "usage: prog [-o {\"foo\",\"bar\"} {\"foo\",\"bar\"} {\"foo\",\"bar\"}]"s);
}

TEST_CASE("Usage message contains for optional argument is name and automatic metavar in brackets for argument with nargs set as zero_or_one")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").nargs(argparse::zero_or_one);

    CHECK(parser.format_usage() == "usage: prog [-o [O]]"s);
}

TEST_CASE("Usage message contains for optional argument is name and metavar in brackets for argument with metavar set and nargs set as zero_or_one")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").metavar("METAVARO").nargs(argparse::zero_or_one);

    CHECK(parser.format_usage() == "usage: prog [-o [METAVARO]]"s);
}

TEST_CASE("Usage message contains for optional argument is name and choices in brackets for argument with choices set and nargs set as zero_or_one")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_one);

    CHECK(parser.format_usage() == "usage: prog [-o [{\"foo\",\"bar\"}]]"s);
}

TEST_CASE("Usage message contains for optional argument its name followed by automatic metavar in brackets and automatic metavar and ellipsis in nested brackets for arguments with nargs set as zero_or_more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").nargs(argparse::zero_or_more);

    CHECK(parser.format_usage() == "usage: prog [-o [O [O ...]]]"s);
}

TEST_CASE("Usage message contains for optional argument its name followed by metavar in brackets and metavar and ellipsis in nested brackets for arguments with metavar set and nargs set as zero_or_more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").metavar("METAVARO").nargs(argparse::zero_or_more);

    CHECK(parser.format_usage() == "usage: prog [-o [METAVARO [METAVARO ...]]]"s);
}

TEST_CASE("Usage message contains for optional argument its name followed by choices in brackets and choices and ellipsis in nested brackets for arguments with choices set and nargs set as zero_or_more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(argparse::zero_or_more);

    CHECK(parser.format_usage() == "usage: prog [-o [{\"foo\",\"bar\"} [{\"foo\",\"bar\"} ...]]]"s);
}

TEST_CASE("Usage message contains for optional argument its name followed by automatic metavar and automatic metavar and ellipsis in brackets for arguments with nargs set as one_or_more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").nargs(argparse::one_or_more);

    CHECK(parser.format_usage() == "usage: prog [-o O [O ...]]"s);
}

TEST_CASE("Usage message contains for optional argument its name followed by metavar and metavar and ellipsis in brackets for arguments with metavar set and nargs set as one_or_more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").metavar("METAVARO").nargs(argparse::one_or_more);

    CHECK(parser.format_usage() == "usage: prog [-o METAVARO [METAVARO ...]]"s);
}

TEST_CASE("Usage message contains for optional argument its name followed by choices and choices and ellipsis in brackets for arguments with choices set and nargs set as one_or_more")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    parser.add_argument("-o").choices({"foo"s, "bar"s}).nargs(argparse::one_or_more);

    CHECK(parser.format_usage() == "usage: prog [-o {\"foo\",\"bar\"} [{\"foo\",\"bar\"} ...]]"s);
}

TEST_CASE("Usage message contains for optional argument in mutually exclusive group its name in brackets")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    auto group = parser.add_mutually_exclusive_group();
    group.add_argument("-o");

    CHECK(parser.format_usage() == "usage: prog [-o O]"s);
}

TEST_CASE("Usage message contains for two optional arguments in same mutually exclusive group their names in brackets separated by a pipe")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    auto group = parser.add_mutually_exclusive_group();
    group.add_argument("-o");
    group.add_argument("-a");

    CHECK(parser.format_usage() == "usage: prog [-o O | -a A]"s);
}

TEST_CASE("Usage message contains for three optional arguments in same mutually exclusive group their names in brackets separated by a pipe")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    auto group = parser.add_mutually_exclusive_group();
    group.add_argument("-o");
    group.add_argument("-a");
    group.add_argument("-z");

    CHECK(parser.format_usage() == "usage: prog [-o O | -a A | -z Z]"s);
}

TEST_CASE("Usage message contains for two optional arguments in separate mutually exclusive groups their names in brackets")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    auto group1 = parser.add_mutually_exclusive_group();
    group1.add_argument("-o");
    auto group2 = parser.add_mutually_exclusive_group();
    group2.add_argument("-a");

    CHECK(parser.format_usage() == "usage: prog [-o O] [-a A]"s);
}

TEST_CASE("Usage message contains for four optional arguments in separate mutually exclusive groups their names in brackets separated by a pipe")
{
    auto parser = argparse::ArgumentParser().prog("prog").add_help(false);
    auto group1 = parser.add_mutually_exclusive_group();
    group1.add_argument("-o");
    group1.add_argument("-p");
    auto group2 = parser.add_mutually_exclusive_group();
    group2.add_argument("-a");
    group2.add_argument("-b");

    CHECK(parser.format_usage() == "usage: prog [-o O | -p P] [-a A | -b B]"s);
}
