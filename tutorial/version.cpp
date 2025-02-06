#include "argparse.h"

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("--version").action(argparse::version).version("1.0.0-rc1");
    parser.parse_args(argc, argv);
}
