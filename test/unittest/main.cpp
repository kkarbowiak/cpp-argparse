#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include "argparse.h"


TEST_CASE("Checking existence of ArgumentParser class")
{
    argparse::ArgumentParser();
}
