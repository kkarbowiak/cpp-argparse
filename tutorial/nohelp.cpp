#include "argparse.h"

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser().add_help(false);
    parser.parse_args(argc, argv);
}
