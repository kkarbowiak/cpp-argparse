#ifndef CPP_ARGPARSE_H__DDK
#define CPP_ARGPARSE_H__DDK

#include <string>
#include <list>
#include <vector>
#include <map>
#include <stdexcept>


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
            auto add_argument(std::string const & name) -> void
            {
                m_arguments.emplace_back(name);
            }

            auto parse_args(int argc, char * argv[]) -> parameters
            {
                return parse_args(get_tokens(argc, argv));
            }

            auto parse_args(tokens args) -> parameters
            {
                for (auto & a : m_arguments)
                {
                    args = a.parse_args(args);
                }

                if (!args.empty())
                {
                    throw std::runtime_error("unrecognised arguments: " + get_string(args));
                }

                parameters result;

                for (auto const & a : m_arguments)
                {
                    result[a.get_name()] = a.get_value();
                }

                return result;
            }

        private:
            auto get_tokens(int argc, char * argv[]) const -> tokens
            {
                tokens result;

                for (int i = 1; i < argc; ++i)
                {
                    result.push_back(argv[i]);
                }

                return result;
            }

            auto get_string(tokens args) const -> std::string
            {
                std::string result;

                for (auto i = args.begin(); i != args.end(); ++i)
                {
                    if (i != args.begin())
                    {
                        result += " ";
                    }
                    result += *i;
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

                    auto parse_args(tokens args) -> tokens
                    {
                        if (args.empty())
                        {
                            // report error
                        }

                        m_value = args.front();
                        args.pop_front();

                        return args;
                    }

                    auto get_name() const -> std::string
                    {
                        return m_name;
                    }

                    auto get_value() const -> std::string
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
