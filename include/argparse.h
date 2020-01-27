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
#include <any>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <cstdlib>


namespace argparse
{
    enum Action
    {
        store,
        store_true,
        store_false,
        store_const,
        help
    };

    enum class Handle
    {
        errors_and_help,
        errors,
        help,
        none
    };

    class parsing_error
      : public std::runtime_error
    {
        public:
            explicit parsing_error(std::string const & message)
              : std::runtime_error(message)
            {
            }
    };

    class ArgumentParser
    {
        public:
            using tokens = std::list<std::string>;
            using optstring = std::optional<std::string>;

        public:
            class Value
            {
                public:
                    Value() = default;

                    template<typename T>
                    Value(T const & t)
                      : m_value(t)
                    {
                    }

                    template<typename T>
                    auto operator=(T const & t) -> Value &
                    {
                        m_value = t;
                        return *this;
                    }

                    explicit operator bool() const
                    {
                        return m_value.has_value();
                    }

                    auto get() const -> std::string
                    {
                        return get<std::string>();
                    }

                    template<typename T>
                    auto get() const -> T
                    {
                        return std::any_cast<T>(m_value);
                    }

                private:
                    std::any m_value;
            };

            class Parameters
            {
                public:
                    auto get(std::string const & name) const -> Value
                    {
                        return m_parameters.at(name);
                    }

                    auto get_value(std::string const & name) const -> std::string
                    {
                        return get(name).get();
                    }

                    template<typename T>
                    auto get_value(std::string const & name) const -> T
                    {
                        return get(name).get<T>();
                    }

                    auto operator[](std::string const & name) -> Value &
                    {
                        return m_parameters[name];
                    }

                private:
                    std::map<std::string, Value> m_parameters;
            };

        private:
            class HelpRequested {};

        public:
            decltype(auto) add_argument(std::string const & name1, std::string const & name2 = "")
            {
                if (name1.front() != '-')
                {
                    return *m_arguments.emplace_back(std::make_unique<PositionalArgument>(name1, name2));
                }
                else
                {
                    return *m_arguments.emplace_back(std::make_unique<OptionalArgument>(name1, name2));
                }
            }

            auto parse_args(int argc, char const * const argv[]) -> Parameters
            {
                if (!m_prog)
                {
                    m_prog = argv[0];
                }

                try
                {
                    return parse_args(get_tokens(argc, argv));
                }
                catch (HelpRequested const &)
                {
                    if (m_handle == Handle::errors_and_help || m_handle == Handle::help)
                    {
                        std::cout << format_help() << std::endl;
                        std::exit(EXIT_SUCCESS);
                    }

                    return get_parameters();
                }
                catch (parsing_error const & e)
                {
                    if (m_handle == Handle::errors_and_help || m_handle == Handle::errors)
                    {
                        std::cout << e.what() << '\n';
                        std::cout << format_help() << std::endl;
                        std::exit(EXIT_FAILURE);
                    }

                    throw;
                }
            }

            auto prog(std::string const & prog) -> ArgumentParser &&
            {
                m_prog = prog;

                return std::move(*this);
            }

            auto description(std::string const & description) -> ArgumentParser &&
            {
                m_description = description;

                return std::move(*this);
            }

            auto epilog(std::string const & epilog) -> ArgumentParser &&
            {
                m_epilog = epilog;

                return std::move(*this);
            }

            auto add_help(bool add) -> ArgumentParser &&
            {
                if (!add)
                {
                    (void) m_arguments.erase(m_arguments.begin());
                }

                return std::move(*this);
            }

            auto handle(Handle handle) -> ArgumentParser &&
            {
                m_handle = handle;

                return std::move(*this);
            }

            auto format_usage() const -> std::string
            {
                std::string message = "usage: " + *m_prog;
                std::string optionals;
                std::string positionals;

                for (auto const & arg : m_arguments)
                {
                    if (arg->is_positional())
                    {
                        positionals += " " + arg->get_metavar_name();
                    }
                    else
                    {
                        optionals += " [" + arg->get_name();
                        if (arg->get_options().m_action != store_true &&
                            arg->get_options().m_action != store_false &&
                            arg->get_options().m_action != store_const &&
                            arg->get_options().m_action != help)
                        {
                            optionals += " " + arg->get_metavar_name();
                        }
                        optionals += "]";
                    }
                }

                return message + optionals + positionals;
            }

            auto format_help() const -> std::string
            {
                std::string message = format_usage();
                std::string positionals;
                std::string optionals;

                for (auto const & arg : m_arguments)
                {
                    if (arg->is_positional())
                    {
                        positionals += "\n  " + arg->get_metavar_name();

                        if (!arg->get_options().m_help.empty())
                        {
                            positionals += " " + arg->get_options().m_help;
                        }
                    }
                    else
                    {
                        optionals += "\n  " + arg->get_name();
                        if (arg->get_options().m_action != store_true &&
                            arg->get_options().m_action != store_false &&
                            arg->get_options().m_action != help)
                        {
                            optionals += " " + arg->get_metavar_name();
                        }

                        if (!arg->get_options().m_help.empty())
                        {
                            optionals += " " + arg->get_options().m_help;
                        }
                    }
                }

                if (m_description)
                {
                    message += "\n\n" + *m_description;
                }

                if (!positionals.empty())
                {
                    message += "\n\npositional arguments:" + positionals;
                }

                if (!optionals.empty())
                {
                    message += "\n\noptional arguments:" + optionals;
                }

                if (m_epilog)
                {
                    message += "\n\n" + *m_epilog;
                }

                return message;
            }

            ArgumentParser()
              : m_handle(Handle::errors_and_help)
            {
                add_argument("-h", "--help").action(argparse::help).help("show this help message and exit");
            }

        private:
            auto parse_args(tokens args) -> Parameters
            {
                for (auto & a : m_arguments)
                {
                    if (!a->is_positional())
                    {
                        args = a->parse_args(args);
                    }
                }

                for (auto & a : m_arguments)
                {
                    if (a->is_positional())
                    {
                        args = a->parse_args(args);
                    }
                }

                if (!args.empty())
                {
                    throw parsing_error("unrecognised arguments: " + get_string(args));
                }

                ensure_no_arguments_missing();

                return get_parameters();
            }

            auto get_tokens(int argc, char const * const argv[]) const -> tokens
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
                    if (a->is_required() && !a->has_value())
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
                    throw parsing_error(*error_message);
                }
            }

            auto get_parameters() const -> Parameters
            {
                Parameters result;

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
                    struct Options
                    {
                        std::string m_help;
                        std::string m_metavar;
                        std::string m_dest;
                        Action m_action = store;
                    };

                public:
                    virtual ~Argument() = default;

                    virtual auto parse_args(tokens args) -> tokens = 0;
                    virtual auto get_name() const -> std::string = 0;
                    virtual auto get_dest_name() const -> std::string = 0;
                    virtual auto get_metavar_name() const -> std::string = 0;
                    virtual auto has_value() const -> bool = 0;
                    virtual auto get_value() const -> std::any = 0;
                    virtual auto is_required() const -> bool = 0;
                    virtual auto is_positional() const -> bool = 0;

                    virtual auto help(std::string const & help) -> Argument & = 0;
                    virtual auto metavar(std::string const & metavar) -> Argument & = 0;
                    virtual auto dest(std::string const & dest) -> Argument & = 0;
                    virtual auto action(Action action) -> Argument & = 0;

                    virtual auto get_options() -> Options & = 0;
            };

            class PositionalArgument : public Argument
            {
                public:
                    PositionalArgument(std::string const & name1, std::string const & name2)
                      : m_name1(name1)
                      , m_name2(name2)
                      , m_value()
                      , m_options()
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
                        return m_name1;
                    }

                    auto get_dest_name() const -> std::string override
                    {
                        return m_options.m_dest.empty()
                            ? m_name1
                            : m_options.m_dest;
                    }

                    auto get_metavar_name() const -> std::string override
                    {
                        return m_options.m_metavar.empty()
                            ? m_name1
                            : m_options.m_metavar;
                    }

                    auto has_value() const -> bool override
                    {
                        return m_value.has_value();
                    }

                    auto get_value() const -> std::any override
                    {
                        return m_value;
                    }

                    auto is_required() const -> bool override
                    {
                        return true;
                    }

                    auto is_positional() const -> bool override
                    {
                        return true;
                    }

                    auto help(std::string const & help) -> Argument & override
                    {
                        m_options.m_help = help;
                        return *this;
                    }

                    auto metavar(std::string const & metavar) -> Argument & override
                    {
                        m_options.m_metavar = metavar;
                        return *this;
                    }

                    auto dest(std::string const & dest) -> Argument & override
                    {
                        m_options.m_dest = dest;
                        return *this;
                    }

                    auto action(Action /* action */) -> Argument & override
                    {
                        return *this;
                    }

                    auto get_options() -> Options & override
                    {
                        return m_options;
                    }

                private:
                    std::string const m_name1;
                    std::string const m_name2;
                    std::any m_value;
                    Options m_options;
            };

            class OptionalArgument : public Argument
            {
                public:
                    explicit OptionalArgument(std::string const & name1, std::string const & name2)
                      : m_name1(name1)
                      , m_name2(name2)
                      , m_value()
                      , m_options()
                    {
                    }

                    auto parse_args(tokens args) -> tokens override
                    {
                        for (auto i = args.begin(); i != args.end(); ++i)
                        {
                            if (*i == m_name1 || *i == m_name2)
                            {
                                i = args.erase(i);
                                if (m_options.m_action == store_true)
                                {
                                    m_value = true;
                                }
                                else if (m_options.m_action == store_false)
                                {
                                    m_value = false;
                                }
                                else if (m_options.m_action == argparse::help)
                                {
                                    m_value = true;
                                    throw HelpRequested();
                                }
                                else
                                {
                                    if (i == args.end())
                                    {
                                        throw parsing_error("argument " + get_name() + ": expected one argument");
                                    }
                                    m_value = *i;
                                    (void) args.erase(i);
                                }
                                break;
                            }
                        }

                        return args;
                    }

                    auto get_name() const -> std::string override
                    {
                        return m_name1;
                    }

                    auto get_dest_name() const -> std::string override
                    {
                        if (!m_options.m_dest.empty())
                        {
                            return m_options.m_dest;
                        }

                        std::string dest;

                        if (m_name1[0] == '-' && m_name1[1] == '-')
                        {
                            dest = m_name1.substr(2);
                        }
                        else if (m_name2[0] == '-' && m_name2[1] == '-')
                        {
                            dest = m_name2.substr(2);
                        }
                        else
                        {
                            dest = m_name1.substr(1);
                        }

                        std::replace(dest.begin(), dest.end(), '-', '_');

                        return dest;
                    }

                    auto get_metavar_name() const -> std::string override
                    {
                        if (!m_options.m_metavar.empty())
                        {
                            return m_options.m_metavar;
                        }

                        auto metavar = get_dest_name();

                        for (auto & ch : metavar)
                        {
                            ch = ::toupper(ch);
                        }

                        return metavar;
                    }

                    auto has_value() const -> bool override
                    {
                        return m_value.has_value();
                    }

                    auto get_value() const -> std::any override
                    {
                        return m_value;
                    }

                    auto is_required() const -> bool override
                    {
                        return false;
                    }

                    auto is_positional() const -> bool override
                    {
                        return false;
                    }

                    auto help(std::string const & help) -> Argument & override
                    {
                        m_options.m_help = help;
                        return *this;
                    }

                    auto metavar(std::string const & metavar) -> Argument & override
                    {
                        m_options.m_metavar = metavar;
                        return *this;
                    }

                    auto dest(std::string const & dest) -> Argument & override
                    {
                        m_options.m_dest = dest;
                        return *this;
                    }

                    auto action(Action action) -> Argument & override
                    {
                        m_options.m_action = action;
                        if (action == store_true || action == argparse::help)
                        {
                            m_value = false;
                        }
                        else if (action == store_false)
                        {
                            m_value = true;
                        }
                        else
                        {
                            // ignore
                        }

                        return *this;
                    }

                    auto get_options() -> Options & override
                    {
                        return m_options;
                    }

                private:
                    std::string const m_name1;
                    std::string const m_name2;
                    std::any m_value;
                    Options m_options;
            };

        private:
            using argument_uptr = std::unique_ptr<Argument>;
            using argument_uptrs = std::vector<argument_uptr>;

        private:
            argument_uptrs m_arguments;
            optstring m_prog;
            optstring m_description;
            optstring m_epilog;
            Handle m_handle;
    };
}

#endif /* CPP_ARGPARSE_H__DDK */
