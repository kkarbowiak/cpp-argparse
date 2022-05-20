/*
    Copyright 2018 - 2022 Krzysztof Karbowiak

    cpp-argparse v1.0.0

    See https://github.com/kkarbowiak/cpp-argparse

    Distributed under MIT license
*/

#ifndef CPP_ARGPARSE_H__DDK
#define CPP_ARGPARSE_H__DDK

#include <string>
#include <string_view>
#include <list>
#include <vector>
#include <map>
#include <optional>
#include <any>
#include <variant>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <type_traits>
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
    inline auto from_string(std::string const & s, T & t) -> bool
    {
        std::istringstream iss(s);
        if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
        {
            int i;
            iss >> i;
            t = static_cast<T>(i);
        }
        else
        {
            iss >> t;
        }

        return !iss.fail() && iss.eof();
    }

    template<typename T>
    inline auto to_string(T const & t) -> std::string
    {
        std::ostringstream ostr;
        if constexpr (std::is_same_v<char, T> || std::is_same_v<signed char, T> || std::is_same_v<unsigned char, T>)
        {
            ostr << static_cast<int>(t);
        }
        else
        {
            ostr << t;
        }
        return ostr.str();
    }

    template<typename T>
    inline auto are_equal(T const & lhs, T const & rhs) -> bool
    {
        return lhs == rhs;
    }

    class ArgumentParser
    {
        private:
            class Value
            {
                public:
                    template<typename T>
                    Value(T const & t)
                      : m_value(t)
                    {
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
                    std::any const m_value;
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

                    auto insert(std::string const & name, std::any const & value) -> void
                    {
                        (void) m_parameters.emplace(name, value);
                    }

                private:
                    std::map<std::string, Value> m_parameters;
            };

        private:
            using tokens = std::list<std::string>;
            using optstring = std::optional<std::string>;

            class HelpRequested {};

        public:
            template<typename ...Args>
            decltype(auto) add_argument(Args &&... names)
            {
                return ArgumentBuilder(m_arguments, std::vector<std::string>{names...});
            }

            auto parse_args(int argc, char const * const argv[]) -> Parameters
            {
                if (!m_prog)
                {
                    m_prog = extract_filename(argv[0]);
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

            auto add_mutually_exclusive_group()
            {
                return MutuallyExclusiveGroup(m_arguments);
            }

            auto prog(std::string const & prog) -> ArgumentParser &&
            {
                m_prog = prog;

                return std::move(*this);
            }

            auto usage(std::string const & usage) -> ArgumentParser &&
            {
                m_usage = usage;

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
                auto const formatter = Formatter(m_arguments, m_prog, m_usage, m_description, m_epilog);
                return formatter.format_usage();
            }

            auto format_help() const -> std::string
            {
                auto const formatter = Formatter(m_arguments, m_prog, m_usage, m_description, m_epilog);
                return formatter.format_help();
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
            auto extract_filename(std::string const & path) -> std::string
            {
                if (auto path_separator = path.find_last_of("/\\"); path_separator != path.npos)
                {
                    return path.substr(path_separator + 1);
                }

                return path;
            }

            auto parse_args(tokens args) -> Parameters
            {
                args = parse_optional_arguments(args);
                args = parse_positional_arguments(args);

                ensure_no_unrecognised_arguments(args);
                ensure_no_arguments_excluded();
                ensure_no_arguments_missing();

                return get_parameters();
            }

            auto get_tokens(int argc, char const * const argv[]) const -> tokens
            {
                return tokens(&argv[1], &argv[argc]);
            }

            template<typename Container>
            static auto join(Container const & strings, std::string const & separator) -> std::string
            {
                std::string result;

                for (auto it = strings.begin(); it != strings.end(); ++it)
                {
                    if (it != strings.begin())
                    {
                        result += separator;
                    }
                    result += *it;
                }

                return result;
            }

            auto parse_optional_arguments(tokens args) -> tokens
            {
                for (auto & arg : m_arguments)
                {
                    if (!arg->is_positional())
                    {
                        args = arg->parse_args(args);
                    }
                }

                return args;
            }

            auto parse_positional_arguments(tokens args) -> tokens
            {
                for (auto & arg : m_arguments)
                {
                    if (arg->is_positional())
                    {
                        args = arg->parse_args(args);
                    }
                }

                return args;
            }

            auto ensure_no_unrecognised_arguments(tokens const & args) const -> void
            {
                if (!args.empty())
                {
                    throw parsing_error("unrecognised arguments: " + join(args, " "));
                }
            }

            auto ensure_no_arguments_excluded() const -> void
            {
                std::map<MutuallyExclusiveGroup const *, Argument const *> exclusive_args;
                for (auto const & arg : m_arguments)
                {
                    if (arg->is_present() && arg->get_options().mutually_exclusive_group != nullptr)
                    {
                        if (auto const result = exclusive_args.emplace(arg->get_options().mutually_exclusive_group, arg.get()); !result.second)
                        {
                            throw parsing_error("argument " + join(arg->get_names(), "/") + ": not allowed with argument " + join(result.first->second->get_names(), "/"));
                        }
                    }
                }
            }

            auto ensure_no_arguments_missing() const -> void
            {
                optstring error_message;

                for (auto const & arg : m_arguments)
                {
                    if (arg->is_required() && !arg->has_value())
                    {
                        if (!error_message)
                        {
                            error_message = "the following arguments are required: " + join(arg->get_names(), "/");
                        }
                        else
                        {
                            *error_message += " " + join(arg->get_names(), "/");
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

                for (auto const & arg : m_arguments)
                {
                    result.insert(arg->get_dest_name(), arg->get_value());
                }

                return result;
            }

        private:
            class MutuallyExclusiveGroup;

            class TypeHandler
            {
                public:
                    virtual ~TypeHandler() = default;

                    virtual auto from_string(std::string const & string, std::any & value) const -> bool = 0;
                    virtual auto to_string(std::any const & value) const -> std::string = 0;
                    virtual auto compare(std::any const & lhs, std::any const & rhs) const -> bool = 0;
                    virtual auto transform(std::vector<std::any> const & values) const -> std::any = 0;
                    virtual auto size(std::any const & value) const -> std::size_t = 0;
            };

            template<typename T>
            class TypeHandlerT : public TypeHandler
            {
                public:
                    auto from_string(std::string const & string, std::any & value) const -> bool override
                    {
                        if constexpr (std::is_same_v<std::string, T>)
                        {
                            value = string;
                            return true;
                        }
                        else
                        {
                            using argparse::from_string;
                            T val;
                            if (from_string(string, val))
                            {
                                value = val;
                                return true;
                            }
                            else
                            {
                                return false;
                            }
                        }
                    }

                    auto to_string(std::any const & value) const -> std::string override
                    {
                        if constexpr (std::is_same_v<std::string, T>)
                        {
                            return "\"" + std::any_cast<std::string>(value) + "\"";
                        }
                        else
                        {
                            using argparse::to_string;
                            return to_string(std::any_cast<T>(value));
                        }
                    }

                    auto compare(std::any const & lhs, std::any const & rhs) const -> bool override
                    {
                        return are_equal(std::any_cast<T>(lhs), std::any_cast<T>(rhs));
                    }

                    auto transform(std::vector<std::any> const & values) const -> std::any override
                    {
                        std::vector<T> result;
                        for (auto const & value : values)
                        {
                            result.push_back(std::any_cast<T>(value));
                        }
                        return std::any(result);
                    }

                    auto size(std::any const & value) const -> std::size_t override
                    {
                        return std::any_cast<std::vector<T>>(value).size();
                    }
            };

            struct Options
            {
                std::vector<std::string> names;
                std::string help;
                std::string metavar;
                std::string dest;
                Action action = store;
                std::any const_;
                std::any default_;
                bool required;
                std::vector<std::any> choices;
                std::optional<std::variant<std::size_t, char>> nargs;
                MutuallyExclusiveGroup const * mutually_exclusive_group = nullptr;
                std::unique_ptr<TypeHandler> type_handler = std::make_unique<TypeHandlerT<std::string>>();

                auto join_choices(std::string const & separator) const -> std::string
                {
                    std::string result;
                    for (auto i = choices.begin(); i != choices.end(); ++i)
                    {
                        if (i != choices.begin())
                        {
                            result += separator;
                        }
                        result += type_handler->to_string(*i);
                    }
                    return result;
                }
            };

            class Argument
            {
                public:
                    Argument(Options options)
                      : m_options(std::move(options))
                    {
                    }
                    virtual ~Argument() = default;

                    virtual auto parse_args(tokens args) -> tokens = 0;
                    virtual auto get_dest_name() const -> std::string = 0;
                    virtual auto get_metavar_name() const -> std::string = 0;
                    virtual auto has_value() const -> bool = 0;
                    virtual auto get_value() const -> std::any = 0;
                    virtual auto is_required() const -> bool = 0;
                    virtual auto is_positional() const -> bool = 0;
                    virtual auto is_present() const -> bool = 0;

                    auto get_name() const -> std::string
                    {
                        return m_options.names.front();
                    }

                    auto get_names() const -> std::vector<std::string> const &
                    {
                        return m_options.names;
                    }

                    auto get_options() const -> Options const &
                    {
                        return m_options;
                    }

                    auto has_nargs() const -> bool
                    {
                        return m_options.nargs.has_value();
                    }

                    auto has_nargs_number() const -> bool
                    {
                        return std::holds_alternative<std::size_t>(*m_options.nargs);
                    }

                    auto get_nargs_number() const -> std::size_t
                    {
                        return std::get<std::size_t>(*m_options.nargs);
                    }

                    auto get_nargs_symbol() const -> char
                    {
                        return std::get<char>(*m_options.nargs);
                    }

                protected:
                    auto check_choices(std::any const & value) const -> void
                    {
                        if (!std::any_of(
                            m_options.choices.begin(),
                            m_options.choices.end(),
                            [&](auto const & rhs) { return m_options.type_handler->compare(value, rhs); }))
                        {
                            std::string message = "argument " + join(m_options.names, "/") + ": invalid choice: ";
                            message += m_options.type_handler->to_string(value);
                            message += " (choose from ";
                            message += m_options.join_choices(", ");
                            message += ")";
                            throw parsing_error(message);
                        }
                    }

                protected:
                    Options const m_options;
            };

            class PositionalArgument final : public Argument
            {
                public:
                    explicit PositionalArgument(Options options)
                      : Argument(std::move(options))
                      , m_value()
                    {
                    }

                    auto parse_args(tokens args) -> tokens override
                    {
                        if (has_nargs())
                        {
                            if (has_nargs_number())
                            {
                                std::vector<std::any> values(std::min(get_nargs_number(), args.size()));
                                consume_args(args, values);

                                m_value = m_options.type_handler->transform(values);
                            }
                            else
                            {
                                switch (get_nargs_symbol())
                                {
                                    case '?':
                                    {
                                        if (!args.empty())
                                        {
                                            consume_arg(args, m_value);
                                        }
                                        else
                                        {
                                            m_value = m_options.default_;
                                        }
                                        break;
                                    }
                                    case '*':
                                    {
                                        std::vector<std::any> values(args.size());
                                        consume_args(args, values);
                                        m_value = m_options.type_handler->transform(values);
                                        break;
                                    }
                                    case '+':
                                    {
                                        std::vector<std::any> values(args.size());
                                        consume_args(args, values);
                                        if (!values.empty())
                                        {
                                            m_value = m_options.type_handler->transform(values);
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (!args.empty())
                            {
                                consume_arg(args, m_value);
                            }
                        }

                        return args;
                    }

                    auto get_dest_name() const -> std::string override
                    {
                        return m_options.dest.empty()
                            ? m_options.names.front()
                            : m_options.dest;
                    }

                    auto get_metavar_name() const -> std::string override
                    {
                        return m_options.metavar.empty()
                            ? m_options.names.front()
                            : m_options.metavar;
                    }

                    auto has_value() const -> bool override
                    {
                        return has_nargs() && has_nargs_number()
                            ? m_options.type_handler->size(m_value) == get_nargs_number()
                            : m_value.has_value();
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

                    auto is_present() const -> bool override
                    {
                        return false;
                    }

                private:
                    auto consume_arg(tokens & args, std::any & value) -> void
                    {
                        if (!m_options.type_handler->from_string(args.front(), value))
                        {
                            throw parsing_error("argument " + get_dest_name() + ": invalid value: '" + args.front() + "'");
                        }
                        if (!m_options.choices.empty())
                        {
                            check_choices(value);
                        }
                        args.pop_front();
                    }

                    auto consume_args(tokens & args, std::vector<std::any> & values) -> void
                    {
                        for (auto & value : values)
                        {
                            consume_arg(args, value);
                        }
                    }

                private:
                    std::any m_value;
            };

            class OptionalArgument final : public Argument
            {
                public:
                    explicit OptionalArgument(Options options)
                      : Argument(std::move(options))
                      , m_value()
                      , m_present(false)
                    {
                    }

                    auto parse_args(tokens args) -> tokens override
                    {
                        if (auto it = find_arg(args); it != args.end())
                        {
                            it = args.erase(it);
                            switch (m_options.action)
                            {
                                case store:
                                    if (has_nargs())
                                    {
                                        if (has_nargs_number())
                                        {
                                            auto const nargs_number = get_nargs_number();
                                            auto const args_number = count_args(it, args.end());
                                            if (args_number < nargs_number)
                                            {
                                                throw parsing_error("argument " + join(get_names(), "/") + ": expected " + std::to_string(nargs_number) + " argument" + (nargs_number > 1 ? "s" : ""));
                                            }
                                            std::vector<std::any> values(args_number);
                                            consume_args(args, it, values);
                                            m_value = m_options.type_handler->transform(values);
                                        }
                                        else
                                        {
                                            switch (get_nargs_symbol())
                                            {
                                                case '?':
                                                {
                                                    if (it == args.end() || it->front() == '-')
                                                    {
                                                        m_value = m_options.const_;
                                                    }
                                                    else
                                                    {
                                                        consume_arg(args, it, m_value);
                                                    }
                                                    break;
                                                }
                                                case '*':
                                                {
                                                    std::vector<std::any> values(count_args(it, args.end()));
                                                    consume_args(args, it, values);
                                                    m_value = m_options.type_handler->transform(values);
                                                    break;
                                                }
                                                case '+':
                                                {
                                                    std::vector<std::any> values(count_args(it, args.end()));
                                                    consume_args(args, it, values);
                                                    if (values.empty())
                                                    {
                                                        throw parsing_error("argument " + join(get_names(), "/") + ": expected at least one argument");
                                                    }
                                                    m_value = m_options.type_handler->transform(values);
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if (it == args.end() || it->front() == '-')
                                        {
                                            throw parsing_error("argument " + join(get_names(), "/") + ": expected one argument");
                                        }
                                        consume_arg(args, it, m_value);
                                    }
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
                            m_present = true;
                        }
                        else
                        {
                            if (m_options.action == store_true || m_options.action == help)
                            {
                                m_value = false;
                            }
                            else if (m_options.action == store_false)
                            {
                                m_value = true;
                            }
                            else
                            {
                                m_value = m_options.default_;
                            }
                        }

                        return args;
                    }

                    auto get_dest_name() const -> std::string override
                    {
                        if (!m_options.dest.empty())
                        {
                            return m_options.dest;
                        }

                        auto dest = get_name_for_dest();

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

                    auto is_present() const -> bool override
                    {
                        return m_present;
                    }

                private:
                    auto find_arg(tokens const & args) const -> tokens::const_iterator
                    {
                        return std::find_first_of(args.begin(), args.end(), m_options.names.begin(), m_options.names.end());
                    }

                    auto count_args(tokens::const_iterator it, tokens::const_iterator end) const -> std::size_t
                    {
                        std::size_t result = 0;
                        while (it != end && it->front() != '-')
                        {
                            ++result;
                            ++it;
                        }
                        return result;
                    }

                    auto consume_arg(tokens & args, tokens::const_iterator & arg_it, std::any & value) -> void
                    {
                        if (!m_options.type_handler->from_string(*arg_it, value))
                        {
                            throw parsing_error("argument " + join(get_names(), "/") + ": invalid value: '" + *arg_it + "'");
                        }
                        if (!m_options.choices.empty())
                        {
                            check_choices(value);
                        }
                        arg_it = args.erase(arg_it);
                    }

                    auto consume_args(tokens & args, tokens::const_iterator & arg_it, std::vector<std::any> & values) -> void
                    {
                        for (auto & value : values)
                        {
                            consume_arg(args, arg_it, value);
                        }
                    }

                    auto get_name_for_dest() const -> std::string
                    {
                        for (auto const & name : m_options.names)
                        {
                            if (name[0] == '-' && name[1] == '-')
                            {
                                return name.substr(2);
                            }
                        }

                        return m_options.names.front().substr(1);
                    }

                private:
                    std::any m_value;
                    bool m_present;
            };

        private:
            using argument_uptr = std::unique_ptr<Argument>;
            using argument_uptrs = std::vector<argument_uptr>;

        private:
            class Formatter
            {
                public:
                    Formatter(argument_uptrs const & arguments, optstring const & prog, optstring const & usage, optstring const & description, optstring const & epilog)
                      : m_arguments(arguments)
                      , m_prog(prog)
                      , m_usage(usage)
                      , m_description(description)
                      , m_epilog(epilog)
                    {
                    }

                    auto format_usage() const -> std::string
                    {
                        if (m_usage)
                        {
                            return "usage: " + *m_usage;
                        }

                        std::string message = "usage: " + *m_prog;
                        std::string optionals;
                        std::string positionals;

                        for (auto it = m_arguments.cbegin(); it != m_arguments.cend(); ++it)
                        {
                            auto const & arg = **it;

                            if (arg.is_positional())
                            {
                                if (arg.has_nargs())
                                {
                                    positionals += format_nargs(arg);
                                }
                                else
                                {
                                    positionals += " ";
                                    positionals += format_arg(arg);
                                }
                            }
                            else
                            {
                                if (arg.is_required())
                                {
                                    optionals += " ";
                                }
                                else if (arg.get_options().mutually_exclusive_group != nullptr && it != m_arguments.cbegin() && arg.get_options().mutually_exclusive_group == (*std::prev(it))->get_options().mutually_exclusive_group)
                                {
                                    optionals += " | ";
                                }
                                else
                                {
                                    optionals += " [";
                                }

                                if (arg.has_nargs())
                                {
                                    optionals += arg.get_name();
                                    optionals += format_nargs(arg);
                                }
                                else
                                {
                                    optionals += arg.get_name();
                                    if (arg.get_options().action == store)
                                    {
                                        optionals += " ";
                                        optionals += format_arg(arg);
                                    }
                                }

                                if (arg.is_required())
                                {
                                    // skip
                                }
                                else if (arg.get_options().mutually_exclusive_group != nullptr && std::next(it) != m_arguments.cend() && arg.get_options().mutually_exclusive_group == (*std::next(it))->get_options().mutually_exclusive_group)
                                {
                                    // skip
                                }
                                else
                                {
                                    optionals += "]";
                                }
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
                                auto arg_line = "  " + format_arg(*arg);

                                if (!arg->get_options().help.empty())
                                {
                                    arg_line += help_string_separation(arg_line.size());
                                    arg_line += arg->get_options().help;
                                }

                                positionals += '\n' + arg_line;
                            }
                            else
                            {
                                std::string arg_line = "  ";

                                for (auto name_it = arg->get_names().begin(); name_it != arg->get_names().end(); ++name_it)
                                {
                                    if (name_it != arg->get_names().begin())
                                    {
                                        arg_line += ", ";
                                    }

                                    arg_line += *name_it;
                                    if (arg->get_options().action == store)
                                    {
                                        if (arg->has_nargs())
                                        {
                                            arg_line += format_nargs(*arg);
                                        }
                                        else
                                        {
                                            arg_line += " ";
                                            arg_line += format_arg(*arg);
                                        }
                                    }
                                }

                                if (!arg->get_options().help.empty())
                                {
                                    arg_line += help_string_separation(arg_line.size());
                                    arg_line += arg->get_options().help;
                                }

                                optionals += '\n' + arg_line;
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

                private:
                    auto format_arg(Argument const & argument) const -> std::string
                    {
                        return argument.get_options().choices.empty()
                            ? argument.get_metavar_name()
                            : "{" + argument.get_options().join_choices(",") + "}";
                    }

                    auto format_nargs(Argument const & argument) const -> std::string
                    {
                        std::string result;
                        auto const formatted_arg = format_arg(argument);

                        if (argument.has_nargs_number())
                        {
                            for (auto n = 0u; n < argument.get_nargs_number(); n++)
                            {
                                result += " " + formatted_arg;
                            }
                        }
                        else
                        {
                            switch (argument.get_nargs_symbol())
                            {
                                case '?':
                                    result += " [" + formatted_arg + "]";
                                    break;
                                case '*':
                                    result += " [" + formatted_arg + " [" + formatted_arg + " ...]]";
                                    break;
                                case '+':
                                    result += " " + formatted_arg + " [" + formatted_arg + " ...]";
                                    break;
                            }
                        }

                        return result;
                    }

                    auto help_string_separation(std::size_t arg_line_length) const -> std::string_view
                    {
                        constexpr std::string_view fill = "\n                        ";
                        return arg_line_length < 23
                            ? fill.substr(arg_line_length + 1)
                            : fill;
                    }

                private:
                    argument_uptrs const & m_arguments;
                    optstring const & m_prog;
                    optstring const & m_usage;
                    optstring const & m_description;
                    optstring const & m_epilog;
            };

            class MutuallyExclusiveGroup
            {
                public:
                    MutuallyExclusiveGroup(argument_uptrs & arguments)
                      : m_arguments(arguments)
                    {
                    }

                    template<typename ...Args>
                    decltype(auto) add_argument(Args &&... names)
                    {
                        return ArgumentBuilder(m_arguments, std::vector<std::string>{names...}, this);
                    }

                private:
                    argument_uptrs & m_arguments;
            };

            class ArgumentBuilder
            {
                public:
                    ArgumentBuilder(argument_uptrs & arguments, std::vector<std::string> names, MutuallyExclusiveGroup * group = nullptr)
                      : m_arguments(arguments)
                      , m_options()
                    {
                        m_options.names = std::move(names);
                        m_options.mutually_exclusive_group = group;
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
                        if constexpr (!std::is_same_v<std::string, T>)
                        {
                            m_options.type_handler = std::make_unique<TypeHandlerT<T>>();
                        }
                        return *this;
                    }

                    auto default_(std::any const & default_) -> ArgumentBuilder &
                    {
                        m_options.default_ = default_;
                        return *this;
                    }

                    auto required(bool required) -> ArgumentBuilder &
                    {
                        if (is_positional())
                        {
                            throw std::runtime_error("'required' is an invalid argument for positionals");
                        }
                        m_options.required = required;
                        return *this;
                    }

                    auto choices(std::vector<std::any> const & choices) -> ArgumentBuilder &
                    {
                        m_options.choices = choices;
                        return *this;
                    }

                    auto nargs(std::size_t nargs) -> ArgumentBuilder &
                    {
                        m_options.nargs = nargs;
                        return *this;
                    }

                    auto nargs(int nargs) -> ArgumentBuilder &
                    {
                        m_options.nargs = static_cast<std::size_t>(nargs);
                        return *this;
                    }

                    auto nargs(char nargs) -> ArgumentBuilder &
                    {
                        m_options.nargs = nargs;
                        return *this;
                    }

                private:
                    auto is_positional() const -> bool
                    {
                        return m_options.names.front().front() != '-';
                    }

                private:
                    argument_uptrs & m_arguments;
                    Options m_options;
            };

        private:
            argument_uptrs m_arguments;
            optstring m_prog;
            optstring m_usage;
            optstring m_description;
            optstring m_epilog;
            Handle m_handle;
    };
}

#endif /* CPP_ARGPARSE_H__DDK */
