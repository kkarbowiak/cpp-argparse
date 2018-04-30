#ifndef CPP_ARGPARSE_H__DDK
#define CPP_ARGPARSE_H__DDK

#include <string>
#include <list>
#include <vector>
#include <map>
#include <optional>
#include <stdexcept>


namespace argparse
{
    using tokens = std::list<std::string>;
    using parameters = std::map<std::string, std::string>;
    using optstring = std::optional<std::string>;
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

                ensure_no_arguments_missing();

                return get_parameters();
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

            auto ensure_no_arguments_missing() const -> void
            {
                optstring error_message;

                for (auto const & a : m_arguments)
                {
                    if (!a.get_value())
                    {
                        if (!error_message)
                        {
                            error_message = "missing arguments: " + a.get_name();
                        }
                        else
                        {
                            *error_message += " " + a.get_name();
                        }
                    }
                }

                if (error_message)
                {
                    throw std::runtime_error(*error_message);
                }
            }

            auto get_parameters() const -> parameters
            {
                parameters result;

                for (auto const & a : m_arguments)
                {
                    result[a.get_name()] = *a.get_value();
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
                        if (!args.empty())
                        {
                            m_value = args.front();
                            args.pop_front();
                        }

                        return args;
                    }

                    auto get_name() const -> std::string
                    {
                        return m_name;
                    }

                    auto get_value() const -> optstring
                    {
                        return m_value;
                    }

                private:
                    std::string const m_name;
                    optstring m_value;
            };

        private:
            using arguments = std::vector<Argument>;

        private:
            arguments m_arguments;
    };
}

#endif /* CPP_ARGPARSE_H__DDK */
