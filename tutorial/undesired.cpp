#include "argparse.h"
#include <iostream>

class Logger
{
    public:
        Logger()
        {
            std::cout << "Log started\n";
        }

        ~Logger()
        {
            std::cout << "Log ended\n";
        }
};

int main(int argc, char * argv[])
{
    Logger logger;
    auto parser = argparse::ArgumentParser();
    parser.parse_args(argc, argv);
}
