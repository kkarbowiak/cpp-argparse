/*
    Copyright 2018 - 2025 Krzysztof Karbowiak

    cpp-argparse v2.7.1

    See https://github.com/kkarbowiak/cpp-argparse

    Distributed under MIT license
*/

#ifndef CPP_ARGPARSE_H__DDK
#define CPP_ARGPARSE_H__DDK

#include <algorithm>
#include <any>
#include <format>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>
#include <cstdlib>


namespace argparse
{
    enum Action
    {
        store,
        store_true,
        store_false,
        store_const,
        count,
        append,
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
        none = 0,
        errors = 1,
        help = 2,
        version = 4,
        errors_and_help = errors | help,
        errors_help_version = errors | help | version
    };

    class parsing_error
      : public std::runtime_error
    {
        public:
            using runtime_error::runtime_error;
    };

    inline auto operator|(Handle lhs, Handle rhs) -> Handle
    {
        return static_cast<Handle>(static_cast<int>(lhs) & static_cast<int>(rhs));
    }

    inline auto operator&(Handle lhs, Handle rhs) -> int
    {
        return static_cast<int>(lhs) & static_cast<int>(rhs);
    }

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
            struct Token
            {
                std::string m_token;
                bool m_consumed = false;
            };

            using tokens = std::vector<Token>;
            using optstring = std::optional<std::string>;

            class HelpRequested {};
            class VersionRequested {};

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
                    if (m_handle & Handle::help)
                    {
                        std::cout << format_help() << std::endl;
                        std::exit(EXIT_SUCCESS);
                    }

                    return get_parameters();
                }
                catch (VersionRequested const &)
                {
                    if (m_handle & Handle::version)
                    {
                        std::cout << format_version() << std::endl;
                        std::exit(EXIT_SUCCESS);
                    }

                    return get_parameters();
                }
                catch (parsing_error const & e)
                {
                    if (m_handle & Handle::errors)
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

            auto format_version() const -> std::string
            {
                if (auto it = std::ranges::find_if(m_arguments, [](auto && arg) { return arg->has_version_action(); }); it != m_arguments.end())
                {
                    return replace_prog((*it)->get_version(), m_prog);
                }

                return "";
            }

            ArgumentParser()
              : m_arguments()
              , m_prog()
              , m_description()
              , m_epilog()
              , m_handle(Handle::errors_help_version)
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
                parse_optional_arguments(args);
                parse_positional_arguments(args);

                consume_pseudo_arguments(args);

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
                return join(strings | std::views::all, separator);
            }

            static auto join(std::ranges::view auto strings, std::string const & separator) -> std::string
            {
                auto result = std::string();

                for (auto const & string : strings | std::views::take(1))
                {
                    result += string;
                }
                for (auto const & string : strings | std::views::drop(1))
                {
                    result += separator;
                    result += string;
                }

                return result;
            }

            auto parse_optional_arguments(tokens & args) -> void
            {
                for (auto const & arg : m_arguments
                    | std::views::filter([](auto && arg){ return !arg->is_positional() && arg->expects_argument(); }))
                {
                    arg->parse_args(args);
                }

                for (auto const & arg : m_arguments
                    | std::views::filter([](auto && arg){ return !arg->is_positional() && !arg->expects_argument(); }))
                {
                    arg->parse_args(args);
                }
            }

            auto parse_positional_arguments(tokens & args) -> void
            {
                for (auto const & arg : m_arguments
                    | std::views::filter([](auto && arg){ return arg->is_positional(); }))
                {
                    arg->parse_args(args);
                }
            }

            static auto consume_pseudo_arguments(tokens & args) -> void
            {
                for (auto & arg : args
                    | std::views::filter([](auto const & arg) { return arg.m_token == "--"; }))
                {
                    arg.m_consumed = true;
                }
            }

            auto ensure_no_unrecognised_arguments(tokens const & args) const -> void
            {
                auto unconsumed = args
                    | std::views::filter([](auto const & token) { return !token.m_consumed; });
                if (!unconsumed.empty())
                {
                    throw parsing_error(std::format("unrecognised arguments: {}", join(unconsumed | std::views::transform([](auto const & token) { return token.m_token; }), " ")));
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

            static auto replace_prog(std::string text, optstring const & replacement) -> std::string
            {
                if (!replacement)
                {
                    return text;
                }

                constexpr auto pattern = std::string_view("{prog}");
                auto pos = text.find(pattern);
                while (pos != std::string::npos)
                {
                    text.replace(pos, pattern.size(), *replacement);
                    pos = text.find(pattern, pos + replacement->size());
                }
                return text;
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
                    virtual auto append(std::any const & value, std::any & values) const -> void = 0;
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
                        auto const transformation = values
                            | std::views::transform([](auto const & value) { return std::any_cast<T>(value); });
                        return std::any(std::vector(transformation.begin(), transformation.end()));
                    }

                    auto append(std::any const & value, std::any & values) const -> void override
                    {
                        std::any_cast<std::vector<T> &>(values).push_back(std::any_cast<T>(value));
                    }

                    auto size(std::any const & value) const -> std::size_t override
                    {
                        return std::any_cast<std::vector<T> const &>(value).size();
                    }
            };

            struct Options
            {
                std::vector<std::string> names;
                std::string help;
                std::string version;
                std::string metavar;
                std::string dest;
                Action action = store;
                std::any const_;
                std::any default_;
                bool required = false;
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

                    virtual auto parse_args(tokens & args) -> void = 0;
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

                    auto expects_argument() const -> bool
                    {
                        return m_options.action == store || m_options.action == append;
                    }

                    auto has_version_action() const -> bool
                    {
                        return m_options.action == version;
                    }

                    auto get_version() const -> std::string const &
                    {
                        return m_options.version;
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
                        for (auto const & choice : m_options.choices | std::views::take(1))
                        {
                            result += m_options.type_handler->to_string(choice);
                        }
                        for (auto const & choice : m_options.choices | std::views::drop(1))
                        {
                            result += separator;
                            result += m_options.type_handler->to_string(choice);
                        }
                        return result;
                    }

                protected:
                    virtual auto get_name_for_error() const -> std::string = 0;

                    auto parse_arguments(std::ranges::view auto args) -> void
                    {
                        auto const values = consume_args(args);
                        m_value = m_options.type_handler->transform(values);
                    }

                    auto consume_arg(Token & arg) const -> std::any
                    {
                        arg.m_consumed = true;
                        return process_arg(arg.m_token);
                    }

                    auto process_arg(std::string const & arg) const -> std::any
                    {
                        std::any value;
                        if (!m_options.type_handler->from_string(arg, value))
                        {
                            throw parsing_error(std::format("argument {}: invalid value: '{}'", get_name_for_error(), arg));
                        }
                        check_choices(value);
                        return value;
                    }

                    auto consume_args(std::ranges::view auto args) const -> std::vector<std::any>
                    {
                        auto result = std::vector<std::any>();
                        auto consumed = std::vector<Token *>();
                        for (auto & arg : args)
                        {
                            result.push_back(process_arg(arg.m_token));
                            consumed.push_back(&arg);
                        }
                        std::ranges::for_each(consumed, [](auto arg) { arg->m_consumed = true; });
                        return result;
                    }

                    auto check_choices(std::any const & value) const -> void
                    {
                        if (m_options.choices.empty())
                        {
                            return;
                        }

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
                    std::any m_value;
            };

            class PositionalArgument final : public Argument
            {
                public:
                    explicit PositionalArgument(Options options)
                      : Argument(std::move(options))
                    {
                    }

                    auto parse_args(tokens & args) -> void override
                    {
                        bool past_pseudo_arg = false;
                        auto consumable = args
                            | std::views::drop_while([](auto const & token)
                                {
                                    return token.m_consumed;
                                })
                            | std::views::take_while([](auto const & token)
                                {
                                    return !token.m_consumed;
                                })
                            | std::views::filter([&past_pseudo_arg](auto const & token)
                                {
                                    if (past_pseudo_arg && (token.m_token != "--"))
                                    {
                                        return true;
                                    }
                                    if (token.m_token == "--")
                                    {
                                        past_pseudo_arg = true;
                                        return false;
                                    }
                                    return !token.m_token.starts_with("-");
                                });
                        if (has_nargs())
                        {
                            if (has_nargs_number())
                            {
                                parse_arguments(consumable | std::views::take(get_nargs_number()));
                            }
                            else
                            {
                                parse_arguments_option(consumable);
                            }
                        }
                        else
                        {
                            if (!consumable.empty())
                            {
                                m_value = consume_arg(consumable.front());
                            }
                        }
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
                    auto parse_arguments_option(std::ranges::view auto args) -> void
                    {
                        switch (get_nargs_option())
                        {
                            case zero_or_one:
                            {
                                if (!args.empty())
                                {
                                    m_value = consume_arg(args.front());
                                }
                                else
                                {
                                    m_value = m_options.default_;
                                }
                                break;
                            }
                            case zero_or_more:
                            {
                                parse_arguments(args);
                                break;
                            }
                            case one_or_more:
                            {
                                auto const values = consume_args(args);
                                if (!values.empty())
                                {
                                    m_value = m_options.type_handler->transform(values);
                                }
                                break;
                            }
                        }
                    }

                    auto get_name_for_error() const -> std::string override
                    {
                        return get_dest_name();
                    }
            };

            class OptionalArgument final : public Argument
            {
                public:
                    explicit OptionalArgument(Options options)
                      : Argument(std::move(options))
                      , m_present(false)
                    {
                    }

                    auto parse_args(tokens & args) -> void override
                    {
                        auto consumable = args
                            | std::views::drop_while([](auto const & token) { return token.m_consumed; })
                            | std::views::take_while([](auto const & token) { return token.m_token != "--"; });
                        for (auto it = consumable.begin(); it != consumable.end();)
                        {
                            if (auto [found, name] = has_arg(it); found)
                            {
                                auto const value = consume_name(it, name);
                                auto consumable_args = std::ranges::subrange(std::next(it), consumable.end())
                                    | std::views::take_while([](auto const & token) { return !token.m_token.starts_with("-"); });

                                switch (m_options.action)
                                {
                                    case store_true:
                                    case store_false:
                                    case store_const:
                                    case count:
                                        if (!value.empty())
                                        {
                                            throw parsing_error(std::format("argument {}: ignored explicit argument '{}'", join(get_names(), "/"), value));
                                        }
                                        break;
                                    default:
                                        break;
                                }

                                switch (m_options.action)
                                {
                                    case store:
                                        if (has_nargs())
                                        {
                                            if (has_nargs_number())
                                            {
                                                parse_arguments_number(consumable_args);
                                            }
                                            else
                                            {
                                                parse_arguments_option(consumable_args);
                                            }
                                        }
                                        else
                                        {
                                            if (value.empty())
                                            {
                                                if (consumable_args.empty())
                                                {
                                                    throw parsing_error(std::format("argument {}: expected one argument", join(get_names(), "/")));
                                                }

                                                m_value = consume_arg(consumable_args.front());
                                            }
                                            else
                                            {
                                                m_value = process_arg(value);
                                            }
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
                                    case count:
                                        if (!m_value.has_value())
                                        {
                                            m_value = 1;
                                        }
                                        else
                                        {
                                            ++std::any_cast<int &>(m_value);
                                        }
                                        break;
                                    case append:
                                        if (value.empty())
                                        {
                                            if (consumable_args.empty())
                                            {
                                                throw parsing_error(std::format("argument {}: expected one argument", join(get_names(), "/")));
                                            }

                                            if (!m_value.has_value())
                                            {
                                                auto const values = consume_args(consumable_args | std::views::take(1));
                                                m_value = m_options.type_handler->transform(values);
                                            }
                                            else
                                            {
                                                auto const val = consume_arg(consumable_args.front());
                                                m_options.type_handler->append(val, m_value);
                                            }
                                        }
                                        else
                                        {
                                            if (!m_value.has_value())
                                            {
                                                auto const values = consume_args(std::views::single(Token{value}));
                                                m_value = m_options.type_handler->transform(values);
                                            }
                                            else
                                            {
                                                auto const val = process_arg(value);
                                                m_options.type_handler->append(val, m_value);
                                            }
                                        }
                                        break;
                                    case help:
                                        m_value = true;
                                        throw HelpRequested();
                                    case version:
                                        m_value = true;
                                        throw VersionRequested();
                                }

                                m_present = true;

                                if (it->m_consumed)
                                {
                                    ++it;
                                }
                            }
                            else
                            {
                                ++it;
                            }
                        }

                        if (!m_present)
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
                    auto parse_arguments_number(std::ranges::view auto args) -> void
                    {
                        auto const nargs_number = get_nargs_number();
                        auto const values = consume_args(args | std::views::take(nargs_number));
                        if (values.size() < nargs_number)
                        {
                            throw parsing_error(std::format("argument {}: expected {} argument{}", join(get_names(), "/"), std::to_string(nargs_number), nargs_number > 1 ? "s" : ""));
                        }
                        m_value = m_options.type_handler->transform(values);
                    }

                    auto parse_arguments_option(std::ranges::view auto args) -> void
                    {
                        switch (get_nargs_option())
                        {
                            case zero_or_one:
                            {
                                if (args.empty())
                                {
                                    m_value = m_options.const_;
                                }
                                else
                                {
                                    m_value = consume_arg(args.front());
                                }
                                break;
                            }
                            case zero_or_more:
                            {
                                parse_arguments(args);
                                break;
                            }
                            case one_or_more:
                            {
                                auto const values = consume_args(args);
                                if (values.empty())
                                {
                                    throw parsing_error(std::format("argument {}: expected at least one argument", join(get_names(), "/")));
                                }
                                m_value = m_options.type_handler->transform(values);
                                break;
                            }
                        }
                    }

                    auto has_arg(auto it) const -> std::pair<bool, std::string>
                    {
                        for (auto const & name: m_options.names)
                        {
                            if (name[1] != '-')
                            {
                                if (it->m_token.starts_with("-") && !it->m_token.starts_with("--") && it->m_token.find(name[1]) != std::string::npos)
                                {
                                    return {true, name};
                                }
                            }
                            else
                            {
                                auto const [first_it, second_it] = std::ranges::mismatch(name, it->m_token);
                                if (first_it == name.end() && (second_it == it->m_token.end() || *second_it == '='))
                                {
                                    return {true, name};
                                }
                            }
                        }

                        return {false, ""};
                    }

                    auto consume_name(auto it, std::string const & name) const -> std::string
                    {
                        if (auto const & arg = *it; arg.m_token.starts_with("--"))
                        {
                            if (auto const pos = arg.m_token.find('='); pos != std::string::npos)
                            {
                                auto const value = arg.m_token.substr(pos + 1);
                                it->m_consumed = true;
                                return value;
                            }
                            else
                            {
                                it->m_consumed = true;
                                return "";
                            }
                        }
                        else
                        {
                            if (it->m_token.size() != 2)
                            {
                                auto const pos = it->m_token.find(name[1]);
                                it->m_token.erase(pos, 1);
                                if (expects_argument())
                                {
                                    if (pos == 1)
                                    {
                                        it->m_consumed = true;
                                        return it->m_token.substr(pos);
                                    }
                                    else
                                    {
                                        auto const prefix = it->m_token.substr(0, pos);
                                        auto const value = it->m_token.substr(pos);
                                        *it = Token{prefix};
                                        return value;
                                    }
                                }
                                else
                                {
                                    return "";
                                }
                            }
                            else
                            {
                                it->m_consumed = true;
                                return "";
                            }
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

                    auto get_name_for_error() const -> std::string override
                    {
                        return join(get_names(), "/");
                    }

                private:
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
                            return std::format("usage: {}", replace_prog(*m_usage, m_prog));
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
                            message += "\n\n" + replace_prog(*m_description, m_prog);
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
                            message += "\n\n" + replace_prog(*m_epilog, m_prog);
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
                                    if (arg->expects_argument())
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
                                arg_line += replace_prog(help, m_prog);
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
                                if (arg->expects_argument())
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
                                arg_line += replace_prog(help, m_prog);
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

                    auto version(std::string const & version) -> ArgumentBuilder &
                    {
                        m_options.version = version;
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
