#ifndef CPP_ARGPARSE_H__DDK
#define CPP_ARGPARSE_H__DDK

#include <string>


namespace argparse
{
    class Argument
    {
        public:
            explicit Argument(std::string const & name)
              : m_name(name)
            {
            }

        private:
            std::string const m_name;
    };

    class ArgumentParser
    {
        public:
            void add_argument()
            {
            }

            void parse_args(int /* argc */, char * /* argv */[])
            {
            }
    };
}

#endif /* CPP_ARGPARSE_H__DDK */
