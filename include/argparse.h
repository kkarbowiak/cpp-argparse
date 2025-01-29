/*
    Copyright 2018 - 2025 Krzysztof Karbowiak

    cpp-argparse v2.3.0

    See https://github.com/kkarbowiak/cpp-argparse

    Distributed under MIT license
*/

#ifndef CPP_ARGPARSE_H__DDK
#define CPP_ARGPARSE_H__DDK

#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <optional>
#include <any>
#include <variant>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <ranges>
#include <type_traits>
#include <iostream>
#include <sstream>
#include <format>
#include <cstdlib>


namespace argparse
{
    enum Action
    {
        store,
        store_true,
        store_false,
        store_const,
        help,
        version
    };

    enum Nargs
    {
        zero_or_one,
        zero_or_more,
        one_or_more
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
            using runtime_error::runtime_error;
    };

    inline auto from_string(std::string const & s, auto & t) -> bool
    {
        auto iss = std::istringstream(s);
        iss >> t;

        return !iss.fail() && (iss.eof() || iss.peek() == std::istringstream::traits_type::eof());
    }

    inline auto to_string(auto const & t) -> std::string
    {
        auto ostr = std::ostringstream();
        ostr << t;

        return ostr.str();
    }

    inline auto are_equal(auto const & lhs, auto const & rhs) -> bool
    {
        return lhs == rhs;
    }

    class ArgumentParser
    {
        private:
            class Value
            {
                public:
                    explicit Value(auto const & t)
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
                        (void) m_parameters.try_emplace(name, value);
                    }

                private:
                    std::map<std::string, Value> m_parameters;
            };

        private:
            using tokens = std::vector<std::string>;
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
            static auto extract_filename(std::string const & path) -> std::string
            {
                if (auto path_separator = path.find_last_of("/\\"); path_separator != path.npos)
                {
                    return path.substr(path_separator + 1);
                }

                return path;
            }

            auto parse_args(tokens args) -> Parameters
            {
                args = parse_optional_arguments(std::move(args));
                args = remove_pseudo_arguments(std::move(args));
                args = parse_positional_arguments(std::move(args));

                ensure_no_unrecognised_arguments(args);
                ensure_no_arguments_excluded();
                ensure_no_arguments_missing();

                return get_parameters();
            }

            static auto get_tokens(int argc, char const * const argv[]) -> tokens
            {
                return tokens(&argv[1], &argv[argc]);
            }

            static auto join(std::vector<std::string> const & strings, std::string const & separator) -> std::string
            {
                auto result = std::string();

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
                for (auto const & arg : m_arguments
                                      | std::views::filter([](auto && arg){ return !arg->is_positional(); }))
                {
                    args = arg->parse_args(std::move(args));
                }

                return args;
            }

            auto parse_positional_arguments(tokens args) -> tokens
            {
                for (auto const & arg : m_arguments
                                      | std::views::filter([](auto && arg){ return arg->is_positional(); }))
                {
                    args = arg->parse_args(std::move(args));
                }

                return args;
            }

            static auto remove_pseudo_arguments(tokens args) -> tokens
            {
                std::erase(args, "--");

                return args;
            }

            auto ensure_no_unrecognised_arguments(tokens const & args) const -> void
            {
                if (!args.empty())
                {
                    throw parsing_error(std::format("unrecognised arguments: {}", join(args, " ")));
                }
            }

            auto ensure_no_arguments_excluded() const -> void
            {
                for (auto it1 = m_arguments.begin(); it1 != m_arguments.end(); ++it1)
                {
                    if (auto const & arg1 = **it1; arg1.is_present())
                    {
                        for (auto it2 = std::next(it1); it2 != m_arguments.end(); ++it2)
                        {
                            if (auto const & arg2 = **it2; arg2.is_present() && arg2.is_mutually_exclusive_with(arg1))
                            {
                                throw parsing_error(std::format("argument {}: not allowed with argument {}", join(arg2.get_names(), "/"), join(arg1.get_names(), "/")));
                            }
                        }
                    }
                }
            }

            auto ensure_no_arguments_missing() const -> void
            {
                auto error_message = optstring();

                for (auto const & arg : m_arguments
                                      | std::views::filter([](auto const & arg){ return arg->is_required() && !arg->has_value(); }))
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

                if (error_message)
                {
                    throw parsing_error(*error_message);
                }
            }

            auto get_parameters() const -> Parameters
            {
                auto result = Parameters();

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
                            auto val = T();
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
                        auto result = std::vector<T>();
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
                std::optional<std::variant<std::size_t, Nargs>> nargs;
                MutuallyExclusiveGroup const * mutually_exclusive_group = nullptr;
                std::unique_ptr<TypeHandler> type_handler = std::make_unique<TypeHandlerT<std::string>>();
            };

            class Argument
            {
                public:
                    explicit Argument(Options options)
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

                    auto get_nargs_option() const -> Nargs
                    {
                        return std::get<Nargs>(*m_options.nargs);
                    }

                    auto is_mutually_exclusive() const -> bool
                    {
                        return m_options.mutually_exclusive_group != nullptr;
                    }

                    auto is_mutually_exclusive_with(Argument const & other) const -> bool
                    {
                        return (m_options.mutually_exclusive_group != nullptr) && (m_options.mutually_exclusive_group == other.m_options.mutually_exclusive_group);
                    }

                    auto has_store_action() const -> bool
                    {
                        return m_options.action == store;
                    }

                    auto get_help_message() const -> std::string const &
                    {
                        return m_options.help;
                    }

                    auto has_choices() const -> bool
                    {
                        return !m_options.choices.empty();
                    }

                    auto get_joined_choices(std::string const & separator) const -> std::string
                    {
                        auto result = std::string();
                        for (auto i = m_options.choices.begin(); i != m_options.choices.end(); ++i)
                        {
                            if (i != m_options.choices.begin())
                            {
                                result += separator;
                            }
                            result += m_options.type_handler->to_string(*i);
                        }
                        return result;
                    }

                protected:
                    auto check_choices(std::any const & value) const -> void
                    {
                        if (!std::ranges::any_of(
                            m_options.choices,
                            [&](auto const & rhs) { return m_options.type_handler->compare(value, rhs); }))
                        {
                            auto const message = std::format(
                                "argument {}: invalid choice: {} (choose from {})",
                                join(m_options.names, "/"),
                                m_options.type_handler->to_string(value),
                                get_joined_choices(", "));
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
                                parse_arguments_number(args, std::min(get_nargs_number(), args.size()));
                            }
                            else
                            {
                                parse_arguments_option(args);
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
                    auto parse_arguments_number(tokens & args, std::size_t number) -> void
                    {
                        auto values = std::vector<std::any>(number);
                        consume_args(args, values);

                        m_value = m_options.type_handler->transform(values);
                    }

                    auto parse_arguments_option(tokens & args) -> void
                    {
                        switch (get_nargs_option())
                        {
                            case zero_or_one:
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
                            case zero_or_more:
                            {
                                parse_arguments_number(args, args.size());
                                break;
                            }
                            case one_or_more:
                            {
                                auto values = std::vector<std::any>(args.size());
                                consume_args(args, values);
                                if (!values.empty())
                                {
                                    m_value = m_options.type_handler->transform(values);
                                }
                                break;
                            }
                        }
                    }

                    auto consume_arg(tokens & args, std::any & value) const -> void
                    {
                        if (!m_options.type_handler->from_string(args.front(), value))
                        {
                            throw parsing_error(std::format("argument {}: invalid value: '{}'", get_dest_name(), args.front()));
                        }
                        if (!m_options.choices.empty())
                        {
                            check_choices(value);
                        }
                        args.erase(args.begin());
                    }

                    auto consume_args(tokens & args, std::vector<std::any> & values) const -> void
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
                        auto const pseudo_it = find_pseudo_arg(args);
                        if (auto [it, name] = find_arg(args.begin(), pseudo_it); it != pseudo_it)
                        {
                            it = consume_name(args, it, name);

                            switch (m_options.action)
                            {
                                case store:
                                    if (has_nargs())
                                    {
                                        if (has_nargs_number())
                                        {
                                            parse_arguments_number(args, it);
                                        }
                                        else
                                        {
                                            parse_arguments_option(args, it);
                                        }
                                    }
                                    else
                                    {
                                        if (it == args.end() || it->starts_with("-"))
                                        {
                                            throw parsing_error(std::format("argument {}: expected one argument", join(get_names(), "/")));
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
                                case version:
                                    m_value = true;
                                    break;
                            }
                            m_present = true;
                        }
                        else
                        {
                            switch (m_options.action)
                            {
                                case store_true:
                                case help:
                                case version:
                                    m_value = false;
                                    break;
                                case store_false:
                                    m_value = true;
                                    break;
                                default:
                                    m_value = m_options.default_;
                                    break;
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

                        std::ranges::replace(dest, '-', '_');

                        return dest;
                    }

                    auto get_metavar_name() const -> std::string override
                    {
                        if (!m_options.metavar.empty())
                        {
                            return m_options.metavar;
                        }

                        auto metavar = get_dest_name();

                        std::ranges::for_each(metavar, [](char & ch) { ch = static_cast<char>(::toupper(ch)); });

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
                    auto parse_arguments_number(tokens & args, tokens::iterator it) -> void
                    {
                        auto const nargs_number = get_nargs_number();
                        auto const args_number = count_args(it, args.end());
                        if (args_number < nargs_number)
                        {
                            throw parsing_error(std::format("argument {}: expected {} argument{}", join(get_names(), "/"), std::to_string(nargs_number), nargs_number > 1 ? "s" : ""));
                        }
                        parse_arguments_number(args, it, nargs_number);
                    }

                    auto parse_arguments_option(tokens & args, tokens::iterator it) -> void
                    {
                        switch (get_nargs_option())
                        {
                            case zero_or_one:
                            {
                                if (it == args.end() || it->starts_with("-"))
                                {
                                    m_value = m_options.const_;
                                }
                                else
                                {
                                    consume_arg(args, it, m_value);
                                }
                                break;
                            }
                            case zero_or_more:
                            {
                                auto const args_number = count_args(it, args.end());
                                parse_arguments_number(args, it, args_number);
                                break;
                            }
                            case one_or_more:
                            {
                                auto const args_number = count_args(it, args.end());
                                if (args_number == 0)
                                {
                                    throw parsing_error(std::format("argument {}: expected at least one argument", join(get_names(), "/")));
                                }
                                parse_arguments_number(args, it, args_number);
                                break;
                            }
                        }
                    }

                    auto parse_arguments_number(tokens & args, tokens::iterator it, std::size_t args_number) -> void
                    {
                        auto values = std::vector<std::any>(args_number);
                        consume_args(args, it, values);
                        m_value = m_options.type_handler->transform(values);
                    }

                    auto find_pseudo_arg(tokens & args) const -> tokens::iterator
                    {
                        return std::ranges::find(args, "--");
                    }

                    auto find_arg(tokens::iterator begin, tokens::iterator end) const -> std::pair<tokens::iterator, std::string>
                    {
                        for (auto it = begin; it != end; ++it)
                        {
                            for (auto const & name : m_options.names)
                            {
                                if (name[1] != '-')
                                {
                                    if (it->starts_with("-") && !it->starts_with("--") && it->find(name[1]) != std::string::npos)
                                    {
                                        return {it, name};
                                    }
                                }
                                else
                                {
                                    auto const [first_it, second_it] = std::ranges::mismatch(name, *it);
                                    if (first_it == name.end() && (second_it == it->end() || *second_it == '='))
                                    {
                                        return {it, name};
                                    }
                                }
                            }
                        }

                        return {end, ""};
                    }

                    auto consume_name(tokens & args, tokens::iterator it, std::string const & name) const -> tokens::iterator
                    {
                        if (auto const & arg = *it; arg.starts_with("--"))
                        {
                            if (auto const pos = arg.find('='); pos != std::string::npos)
                            {
                                auto const value = arg.substr(pos + 1);
                                *it = value;
                            }
                            else
                            {
                                it = args.erase(it);
                            }
                        }
                        else
                        {
                            if (it->size() != 2)
                            {
                                auto const pos = it->find(name[1]);
                                it->erase(pos, 1);
                                if (m_options.action == store)
                                {
                                    if (pos == 1)
                                    {
                                        it->erase(0, 1);
                                    }
                                    else
                                    {
                                        auto const prefix = it->substr(0, pos);
                                        auto const value = it->substr(pos);
                                        *it = prefix;
                                        it = args.insert(it, value);
                                    }
                                }
                            }
                            else
                            {
                                it = args.erase(it);
                            }
                        }

                        return it;
                    }

                    auto count_args(tokens::const_iterator it, tokens::const_iterator end) const -> std::size_t
                    {
                        auto result = std::size_t(0);
                        while (it != end && !it->starts_with('-'))
                        {
                            ++result;
                            ++it;
                        }
                        return result;
                    }

                    auto consume_arg(tokens & args, tokens::iterator & arg_it, std::any & value) const -> void
                    {
                        if (!m_options.type_handler->from_string(*arg_it, value))
                        {
                            throw parsing_error(std::format("argument {}: invalid value: '{}'", join(get_names(), "/"), *arg_it));
                        }
                        if (!m_options.choices.empty())
                        {
                            check_choices(value);
                        }
                        arg_it = args.erase(arg_it);
                    }

                    auto consume_args(tokens & args, tokens::iterator & arg_it, std::vector<std::any> & values) const -> void
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
                            if (name.starts_with("--"))
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
                            return std::format("usage: {}", *m_usage);
                        }

                        return std::format("usage: {}{}{}", *m_prog, format_usage_optionals(), format_usage_positionals());
                    }

                    auto format_help() const -> std::string
                    {
                        auto message = format_usage();
                        auto positionals = format_help_positionals();
                        auto optionals = format_help_optionals();

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
                    auto format_usage_positionals() const -> std::string
                    {
                        auto positionals = std::string();

                        for (auto const & arg : m_arguments
                                              | std::views::filter([](auto const & arg){ return arg->is_positional(); }))
                        {
                            if (arg->has_nargs())
                            {
                                positionals += format_nargs(*arg);
                            }
                            else
                            {
                                positionals += " ";
                                positionals += format_arg(*arg);
                            }
                        }

                        return positionals;
                    }

                    auto format_usage_optionals() const -> std::string
                    {
                        auto optionals = std::string();

                        for (auto it = m_arguments.cbegin(); it != m_arguments.cend(); ++it)
                        {
                            auto const & arg = *it;

                            if (!arg->is_positional())
                            {
                                if (arg->is_required())
                                {
                                    optionals += " ";
                                }
                                else if (arg->is_mutually_exclusive() && it != m_arguments.cbegin() && arg->is_mutually_exclusive_with(**std::prev(it)))
                                {
                                    optionals += " | ";
                                }
                                else
                                {
                                    optionals += " [";
                                }

                                if (arg->has_nargs())
                                {
                                    optionals += arg->get_name();
                                    optionals += format_nargs(*arg);
                                }
                                else
                                {
                                    optionals += arg->get_name();
                                    if (arg->has_store_action())
                                    {
                                        optionals += " ";
                                        optionals += format_arg(*arg);
                                    }
                                }

                                if (arg->is_required())
                                {
                                    // skip
                                }
                                else if (arg->is_mutually_exclusive() && std::next(it) != m_arguments.cend() && arg->is_mutually_exclusive_with(**std::next(it)))
                                {
                                    // skip
                                }
                                else
                                {
                                    optionals += "]";
                                }
                            }
                        }

                        return optionals;
                    }

                    auto format_help_positionals() const -> std::string
                    {
                        auto positionals = std::string();

                        for (auto const & arg : m_arguments
                                              | std::views::filter([](auto const & arg){ return arg->is_positional(); }))
                        {
                            auto arg_line = "  " + format_arg(*arg);

                            if (auto const & help = arg->get_help_message(); !help.empty())
                            {
                                arg_line += help_string_separation(arg_line.size());
                                arg_line += help;
                            }

                            positionals += '\n' + arg_line;
                        }

                        return positionals;
                    }

                    auto format_help_optionals() const -> std::string
                    {
                        auto optionals = std::string();

                        for (auto const & arg : m_arguments
                                              | std::views::filter([](auto const & arg){ return !arg->is_positional(); }))
                        {
                            auto arg_line = std::string("  ");

                            for (auto name_it = arg->get_names().begin(); name_it != arg->get_names().end(); ++name_it)
                            {
                                if (name_it != arg->get_names().begin())
                                {
                                    arg_line += ", ";
                                }

                                arg_line += *name_it;
                                if (arg->has_store_action())
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

                            if (auto const & help = arg->get_help_message(); !help.empty())
                            {
                                arg_line += help_string_separation(arg_line.size());
                                arg_line += help;
                            }

                            optionals += '\n' + arg_line;
                        }

                        return optionals;
                    }

                    auto format_arg(Argument const & argument) const -> std::string
                    {
                        return argument.has_choices()
                            ? "{" + argument.get_joined_choices(",") + "}"
                            : argument.get_metavar_name();
                    }

                    auto format_nargs(Argument const & argument) const -> std::string
                    {
                        auto result = std::string();
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
                            switch (argument.get_nargs_option())
                            {
                                case zero_or_one:
                                    result += std::format(" [{0}]", formatted_arg);
                                    break;
                                case zero_or_more:
                                    result += std::format(" [{0} [{0} ...]]", formatted_arg);
                                    break;
                                case one_or_more:
                                    result += std::format(" {0} [{0} ...]", formatted_arg);
                                    break;
                            }
                        }

                        return result;
                    }

                    auto help_string_separation(std::size_t arg_line_length) const -> std::string_view
                    {
                        constexpr auto fill = std::string_view("\n                        ");
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
                        if (m_options.action == argparse::version)
                        {
                            if (m_options.help.empty())
                            {
                                m_options.help = "show program's version number and exit";
                            }
                        }

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

                    auto version(std::string const & /* version */) -> ArgumentBuilder &
                    {
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

                    auto nargs(Nargs nargs) -> ArgumentBuilder &
                    {
                        m_options.nargs = nargs;
                        return *this;
                    }

                private:
                    auto is_positional() const -> bool
                    {
                        return !m_options.names.front().starts_with('-');
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
