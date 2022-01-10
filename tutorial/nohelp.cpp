#include "argparse.h"

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser().add_help(false);
    parser.parse_args(argc, argv);
}
