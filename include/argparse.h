/*
    Copyright 2018 Krzysztof Karbowiak
*/

#ifndef CPP_ARGPARSE_H__DDK
#define CPP_ARGPARSE_H__DDK

#include <string>
#include <list>
#include <vector>
#include <map>
#include <optional>
#include <memory>
#include <stdexcept>
#include <algorithm>


namespace argparse
{
    using tokens = std::list<std::string>;
    using optstring = std::optional<std::string>;
    using parameters = std::map<std::string, optstring>;
}

namespace argparse
{
    class ArgumentParser
    {
        public:
            auto add_argument(std::string const & name) -> void
            {
                if (name.front() != '-')
                {
                    m_arguments.emplace_back(std::make_unique<PositionalArgument>(name));
                }
                else
                {
                    m_arguments.emplace_back(std::make_unique<OptionalArgument>(name));
                }
            }

            auto parse_args(int argc, char const * argv[]) -> parameters
            {
                m_prog = argv[0];

                return parse_args(get_tokens(argc, argv));
            }

            auto parse_args(tokens args) -> parameters
            {
                for (auto & a : m_arguments)
                {
                    args = a->parse_args(args);
                }

                if (!args.empty())
                {
                    throw std::runtime_error("unrecognised arguments: " + get_string(args));
                }

                ensure_no_arguments_missing();

                return get_parameters();
            }

            auto format_usage() const -> std::string
            {
                std::string usage = "usage: " + m_prog;

                for (auto const & arg : m_arguments)
                {
                    if (arg->is_required())
                    {
                        usage += " " + arg->get_name();
                    }
                }

                for (auto const & arg : m_arguments)
                {
                    if (!arg->is_required())
                    {
                        usage += " [" + arg->get_name() + " " + arg->get_metavar_name() +"]";
                    }
                }

                return usage;
            }

        private:
            auto get_tokens(int argc, char const * argv[]) const -> tokens
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
                    if (a->is_required() && !a->get_value())
                    {
                        if (!error_message)
                        {
                            error_message = "missing arguments: " + a->get_name();
                        }
                        else
                        {
                            *error_message += " " + a->get_name();
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
                    result[a->get_dest_name()] = a->get_value();
                }

                return result;
            }

        private:
            class Argument
            {
                public:
                    virtual ~Argument() = default;

                    virtual auto parse_args(tokens args) -> tokens = 0;
                    virtual auto get_name() const -> std::string = 0;
                    virtual auto get_dest_name() const -> std::string = 0;
                    virtual auto get_metavar_name() const -> std::string = 0;
                    virtual auto get_value() const -> optstring = 0;
                    virtual auto is_required() const -> bool = 0;
            };

            class PositionalArgument : public Argument
            {
                public:
                    explicit PositionalArgument(std::string const & name)
                      : m_name(name)
                      , m_value()
                    {
                    }

                    auto parse_args(tokens args) -> tokens override
                    {
                        if (!args.empty())
                        {
                            m_value = args.front();
                            args.pop_front();
                        }

                        return args;
                    }

                    auto get_name() const -> std::string override
                    {
                        return m_name;
                    }

                    auto get_dest_name() const -> std::string override
                    {
                        return m_name;
                    }

                    auto get_metavar_name() const -> std::string override
                    {
                        return m_name;
                    }

                    auto get_value() const -> optstring override
                    {
                        return m_value;
                    }

                    auto is_required() const -> bool override
                    {
                        return true;
                    }

                private:
                    std::string const m_name;
                    optstring m_value;
            };

            class OptionalArgument : public Argument
            {
                public:
                    explicit OptionalArgument(std::string const & name)
                      : m_name(name)
                      , m_value()
                    {
                    }

                    auto parse_args(tokens args) -> tokens override
                    {
                        for (auto i = args.begin(); i != args.end(); ++i)
                        {
                            if (*i == m_name)
                            {
                                i = args.erase(i);
                                if (i == args.end())
                                {
                                    throw std::runtime_error("argument " + get_name() + ": expected one argument");
                                }
                                m_value = *i;
                                (void) args.erase(i);
                                break;
                            }
                        }

                        return args;
                    }

                    auto get_name() const -> std::string override
                    {
                        return m_name;
                    }

                    auto get_dest_name() const -> std::string override
                    {
                        auto dest = m_name.substr(1);
                        if (dest.front() == '-')
                        {
                            dest = dest.substr(1);
                        }
                        std::replace(dest.begin(), dest.end(), '-', '_');

                        return dest;
                    }

                    auto get_metavar_name() const -> std::string override
                    {
                        auto metavar = get_dest_name();

                        for (auto & ch : metavar)
                        {
                            ch = ::toupper(ch);
                        }

                        return metavar;
                    }

                    auto get_value() const -> optstring override
                    {
                        return m_value;
                    }

                    auto is_required() const -> bool override
                    {
                        return false;
                    }

                private:
                    std::string const m_name;
                    optstring m_value;
            };

        private:
            using argument_uptr = std::unique_ptr<Argument>;
            using argument_uptrs = std::vector<argument_uptr>;

        private:
            argument_uptrs m_arguments;
            std::string m_prog;
    };
}

#endif /* CPP_ARGPARSE_H__DDK */
