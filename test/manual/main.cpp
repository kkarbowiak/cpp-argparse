#include "argparse.h"


int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("positional");
    parser.add_argument("--optional");
    parser.add_argument("-v", "--version").action(argparse::version).version("1.0.0");
    parser.parse_args(argc, argv);
}
