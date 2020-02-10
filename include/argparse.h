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
#include <array>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <charconv>
#include <iostream>
#include <sstream>
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

    template<typename T>
    inline auto from_string(std::string const & s, T & t) -> void
    {
        (void) std::from_chars(s.data(), s.data() + s.size(), t);
    }

    inline auto from_string(std::string const & s, float & f) -> void
    {
        f = std::stof(s);
    }

    inline auto from_string(std::string const & s, double & d) -> void
    {
        d = std::stod(s);
    }

    inline auto from_string(std::string const & s, long double & ld) -> void
    {
        ld = std::stold(s);
    }

    template<typename T>
    inline auto to_string(T const& t) -> std::string
    {
        std::array<char, 256> chars;
        auto [ptr, ec] = std::to_chars(chars.data(), chars.data() + chars.size(), t);
        return std::string(chars.data(), ptr);
    }

    inline auto to_string(float f) -> std::string
    {
        std::ostringstream ostr;
        ostr << f;
        return ostr.str();
    }

    inline auto to_string(double d) -> std::string
    {
        std::ostringstream ostr;
        ostr << d;
        return ostr.str();
    }

    inline auto to_string(long double ld) -> std::string
    {
        std::ostringstream ostr;
        ostr << ld;
        return ostr.str();
    }

    class ArgumentParser
    {
        private:
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
            using tokens = std::list<std::string>;
            using optstring = std::optional<std::string>;

            class HelpRequested {};

        public:
            decltype(auto) add_argument(std::string const & name1, std::string const & name2 = "")
            {
                return ArgumentBuilder(m_arguments, name1, name2);
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
                        optionals += arg->is_required()
                            ? " "
                            : " [";
                        optionals += arg->get_name();
                        if (arg->get_options().action == store)
                        {
                            optionals += " " + arg->get_metavar_name();
                        }
                        optionals += arg->is_required()
                            ? ""
                            : "]";
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

                        if (!arg->get_options().help.empty())
                        {
                            positionals += " " + arg->get_options().help;
                        }
                    }
                    else
                    {
                        optionals += "\n  " + arg->get_name();
                        if (arg->get_options().action == store)
                        {
                            optionals += " " + arg->get_metavar_name();
                        }

                        if (!arg->get_options().help.empty())
                        {
                            optionals += " " + arg->get_options().help;
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
              : m_arguments()
              , m_prog()
              , m_description()
              , m_epilog()
              , m_handle(Handle::errors_and_help)
            {
                add_argument("-h", "--help").action(help).help("show this help message and exit");
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
                            error_message = "the following arguments are required: " + a->get_name();
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
            struct Options
            {
                std::string name1;
                std::string name2;
                std::string help;
                std::string metavar;
                std::string dest;
                Action action = store;
                std::any const_;
                std::any default_;
                bool required;
                std::vector<std::any> choices;
                std::function<void (std::string const &, std::any &)> from_string =
                    [](std::string const & s, std::any & a)
                    {
                        a = s;
                    };
                std::function<std::string(std::any const&)> to_string =
                    [](std::any const& a)
                    {
                        return "\"" + std::any_cast<std::string>(a) + "\"";
                    };
                std::function<bool (std::any const &, std::any const &)> comparator =
                    [](std::any const & lhs, std::any const & rhs)
                    {
                        return std::any_cast<std::string>(lhs) == std::any_cast<std::string>(rhs);
                    };
            };

            class Argument
            {
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

                    virtual auto get_options() -> Options const & = 0;
            };

            class PositionalArgument : public Argument
            {
                public:
                    explicit PositionalArgument(Options options)
                      : m_value()
                      , m_options(std::move(options))
                    {
                    }

                    auto parse_args(tokens args) -> tokens override
                    {
                        if (!args.empty())
                        {
                            m_options.from_string(args.front(), m_value);
                            if (!m_options.choices.empty())
                            {
                                if (!std::any_of(
                                    m_options.choices.begin(),
                                    m_options.choices.end(),
                                    [&](auto const& rhs){ return m_options.comparator(m_value, rhs); }))
                                {
                                    std::string message = "argument " + get_name() + ": invalid choice: ";
                                    message += m_options.to_string(m_value);
                                    message += " (choose from ";
                                    for (auto i = m_options.choices.begin(); i != m_options.choices.end(); ++i)
                                    {
                                        if (i != m_options.choices.begin())
                                        {
                                            message += ", ";
                                        }
                                        message += m_options.to_string(*i);
                                    }
                                    message += ")";
                                    throw parsing_error(message);
                                }
                            }
                            args.pop_front();
                        }

                        return args;
                    }

                    auto get_name() const -> std::string override
                    {
                        return m_options.name1;
                    }

                    auto get_dest_name() const -> std::string override
                    {
                        return m_options.dest.empty()
                            ? m_options.name1
                            : m_options.dest;
                    }

                    auto get_metavar_name() const -> std::string override
                    {
                        return m_options.metavar.empty()
                            ? m_options.name1
                            : m_options.metavar;
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

                    auto get_options() -> Options const & override
                    {
                        return m_options;
                    }

                private:
                    std::any m_value;
                    Options const m_options;
            };

            class OptionalArgument : public Argument
            {
                public:
                    explicit OptionalArgument(Options options)
                      : m_value()
                      , m_options(std::move(options))
                    {
                        if (m_options.action == store_true || m_options.action == help)
                        {
                            m_value = false;
                        }
                        else if (m_options.action == store_false)
                        {
                            m_value = true;
                        }
                        else if (m_options.default_.has_value())
                        {
                            m_value = m_options.default_;
                        }
                    }

                    auto parse_args(tokens args) -> tokens override
                    {
                        for (auto i = args.begin(); i != args.end(); ++i)
                        {
                            if (*i == m_options.name1 || *i == m_options.name2)
                            {
                                i = args.erase(i);
                                switch (m_options.action)
                                {
                                    case store:
                                        if (i == args.end())
                                        {
                                            throw parsing_error("argument " + get_name() + ": expected one argument");
                                        }
                                        m_options.from_string(*i, m_value);
                                        if (!m_options.choices.empty())
                                        {
                                            if (!std::any_of(
                                                m_options.choices.begin(),
                                                m_options.choices.end(),
                                                [&](auto const & rhs){ return m_options.comparator(m_value, rhs); }))
                                            {
                                                std::string message = "argument " + get_name() + ": invalid choice: ";
                                                message += m_options.to_string(m_value);
                                                message += " (choose from ";
                                                for (auto i = m_options.choices.begin(); i != m_options.choices.end(); ++i)
                                                {
                                                    if (i != m_options.choices.begin())
                                                    {
                                                        message += ", ";
                                                    }
                                                    message += m_options.to_string(*i);
                                                }
                                                message += ")";
                                                throw parsing_error(message);
                                            }
                                        }
                                        (void) args.erase(i);
                                        break;
                                    case store_true:
                                        m_value = true;
                                        break;
                                    case store_false:
                                        m_value = false;
                                        break;
                                    case store_const:
                                        m_value = m_options.const_;
                                        break;
                                    case help:
                                        m_value = true;
                                        throw HelpRequested();
                                }
                                break;
                            }
                        }

                        return args;
                    }

                    auto get_name() const -> std::string override
                    {
                        return m_options.name1;
                    }

                    auto get_dest_name() const -> std::string override
                    {
                        if (!m_options.dest.empty())
                        {
                            return m_options.dest;
                        }

                        std::string dest;

                        if (m_options.name1[0] == '-' && m_options.name1[1] == '-')
                        {
                            dest = m_options.name1.substr(2);
                        }
                        else if (m_options.name2[0] == '-' && m_options.name2[1] == '-')
                        {
                            dest = m_options.name2.substr(2);
                        }
                        else
                        {
                            dest = m_options.name1.substr(1);
                        }

                        std::replace(dest.begin(), dest.end(), '-', '_');

                        return dest;
                    }

                    auto get_metavar_name() const -> std::string override
                    {
                        if (!m_options.metavar.empty())
                        {
                            return m_options.metavar;
                        }

                        auto metavar = get_dest_name();

                        std::transform(metavar.begin(), metavar.end(), metavar.begin(), [](char ch) { return static_cast<char>(::toupper(ch)); });

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
                        return m_options.required;
                    }

                    auto is_positional() const -> bool override
                    {
                        return false;
                    }

                    auto get_options() -> Options const & override
                    {
                        return m_options;
                    }

                private:
                    std::any m_value;
                    Options const m_options;
            };

        private:
            using argument_uptr = std::unique_ptr<Argument>;
            using argument_uptrs = std::vector<argument_uptr>;

        private:
            class ArgumentBuilder
            {
                public:
                    ArgumentBuilder(argument_uptrs & arguments, std::string const & name1, std::string const & name2)
                      : m_arguments(arguments)
                      , m_options()
                    {
                        m_options.name1 = name1;
                        m_options.name2 = name2;
                    }

                    ~ArgumentBuilder()
                    {
                        if (is_positional())
                        {
                            m_arguments.push_back(std::make_unique<PositionalArgument>(std::move(m_options)));
                        }
                        else
                        {
                            m_arguments.push_back(std::make_unique<OptionalArgument>(std::move(m_options)));
                        }
                    }

                    auto help(std::string const & help) -> ArgumentBuilder &
                    {
                        m_options.help = help;
                        return *this;
                    }

                    auto metavar(std::string const & metavar) -> ArgumentBuilder &
                    {
                        m_options.metavar = metavar;
                        return *this;
                    }

                    auto dest(std::string const & dest) -> ArgumentBuilder &
                    {
                        m_options.dest = dest;
                        return *this;
                    }

                    auto action(Action action) -> ArgumentBuilder &
                    {
                        m_options.action = action;
                        return *this;
                    }

                    auto const_(std::any const & const_) -> ArgumentBuilder &
                    {
                        m_options.const_ = const_;
                        return *this;
                    }

                    template<typename T>
                    auto type() -> ArgumentBuilder &
                    {
                        m_options.from_string =
                            [](std::string const & s, std::any & a)
                            {
                                T val;
                                from_string(s, val);
                                a = val;
                            };
                        m_options.to_string =
                            [](std::any const& a)
                            {
                                return to_string(std::any_cast<T>(a));
                            };
                        m_options.comparator =
                            [](std::any const& l, std::any const& r)
                            {
                                return std::any_cast<T>(l) == std::any_cast<T>(r);
                            };
                        return *this;
                    }

                    auto default_(std::any const & default_) -> ArgumentBuilder &
                    {
                        m_options.default_ = default_;
                        return *this;
                    }

                    auto required(bool required) -> ArgumentBuilder&
                    {
                        if (is_positional())
                        {
                            throw std::runtime_error("'required' is an invalid argument for positionals");
                        }
                        m_options.required = required;
                        return *this;
                    }

                    auto choices(std::vector<std::any> const & choices) -> ArgumentBuilder&
                    {
                        m_options.choices = choices;
                        return *this;
                    }

                private:
                    auto is_positional() const -> bool
                    {
                        return m_options.name1.front() != '-';
                    }

                private:
                    argument_uptrs & m_arguments;
                    Options m_options;
            };

        private:
            argument_uptrs m_arguments;
            optstring m_prog;
            optstring m_description;
            optstring m_epilog;
            Handle m_handle;
    };
}

#endif /* CPP_ARGPARSE_H__DDK */
