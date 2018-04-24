#ifndef CPP_ARGPARSE_H__DDK
#define CPP_ARGPARSE_H__DDK

#include <string>
#include <vector>


namespace argparse
{
    using tokens = std::vector<std::string>;
}

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

            void parse_args(tokens const & /* args */)
            {
            }

        private:
            tokens get_tokens(int argc, char * argv[])
            {
                tokens result;

                for (int i = 1; i < argc; ++i)
                {
                    result.push_back(argv[i]);
                }

                return result;
            }
    };
}

#endif /* CPP_ARGPARSE_H__DDK */
