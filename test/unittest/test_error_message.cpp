#include "argparse.h"

#include "cstring_array.h"

#include "doctest.h"


TEST_CASE("Missing required arguments message lists all argument's names")
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.add_argument("-o").required(true);

    CHECK_THROWS_WITH_AS(parser.parse_args(1, cstr_arr{"prog"}), "the following arguments are required: -o", argparse::parsing_error);
}
