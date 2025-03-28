#include "argparse.hpp"

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser();
    parser.parse_args(argc, argv);
}
