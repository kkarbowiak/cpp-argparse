#ifndef CPP_ARGPARSE_H__DDK
#define CPP_ARGPARSE_H__DDK

#include <string>
#include <list>
#include <vector>
#include <map>


namespace argparse
{
    using tokens = std::list<std::string>;
    using parameters = std::map<std::string, std::string>;
}

namespace argparse
{
    class ArgumentParser
    {
        public:
            void add_argument(std::string const & name)
            {
                m_arguments.emplace_back(name);
            }

            parameters parse_args(int argc, char * argv[])
            {
                return parse_args(get_tokens(argc, argv));
            }

            parameters parse_args(tokens args)
            {
                for (auto & a : m_arguments)
                {
                    args = a.parse_args(args);
                }

                if (!args.empty())
                {
                    // report error
                }

                parameters result;

                for (auto const & a : m_arguments)
                {
                    result[a.get_name()] = a.get_value();
                }

                return result;
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
            class Argument
            {
                public:
                    explicit Argument(std::string const & name)
                    : m_name(name)
                    , m_value()
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

                    std::string get_name() const
                    {
                        return m_name;
                    }

                    std::string get_value() const
                    {
                        return m_value;
                    }

                private:
                    std::string const m_name;
                    std::string m_value;
            };

        private:
            using arguments = std::vector<Argument>;

        private:
            arguments m_arguments;
    };
}

#endif /* CPP_ARGPARSE_H__DDK */
