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
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    try
    {
        auto parsed = parser.parse_args(argc, argv);
        if (parsed.get_value<bool>("help"))
        {
            std::cout << parser.format_help() << '\n';
            return 0;
        }
    }
    catch (argparse::parsing_error const & e)
    {
        std::cout << e.what() << '\n';
        return 1;
    }
}
