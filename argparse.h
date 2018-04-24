#ifndef CPP_ARGPARSE_H__DDK
#define CPP_ARGPARSE_H__DDK

#include <string>
#include <list>
#include <vector>


namespace argparse
{
    using tokens = std::list<std::string>;
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

            tokens parse_args(tokens args)
            {
                if (args.empty())
                {
                    // report error
                }

                m_value = args.front();
                args.pop_front();

                return args;
            }

        private:
            std::string const m_name;
            std::string m_value;
    };

    class ArgumentParser
    {
        public:
            void add_argument(std::string const & name)
            {
                m_arguments.emplace_back(name);
            }

            void parse_args(int argc, char * argv[])
            {
                parse_args(get_tokens(argc, argv));
            }

            void parse_args(tokens args)
            {
                for (auto & a : m_arguments)
                {
                    args = a.parse_args(args);
                }

                if (!args.empty())
                {
                    // report error
                }
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

        private:
            using arguments = std::vector<Argument>;

        private:
            arguments m_arguments;
    };
}

#endif /* CPP_ARGPARSE_H__DDK */
