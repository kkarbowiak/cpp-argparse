#include "argparse.h"

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.parse_args(argc, argv);
}
