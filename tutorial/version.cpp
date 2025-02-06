#include "argparse.h"

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("--version").action(argparse::version).version("1.0.0-rc1");
    parser.parse_args(argc, argv);
}
