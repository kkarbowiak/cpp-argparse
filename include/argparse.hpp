/*
    Copyright 2018 - 2025 Krzysztof Karbowiak

    cpp-argparse v3.2.0

    See https://github.com/kkarbowiak/cpp-argparse

    Distributed under MIT license
*/

#pragma once

#include <algorithm>
#include <any>
#include <format>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <print>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeinfo>
#include <utility>
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

    class name_error
      : public std::logic_error
    {
        public:
            using logic_error::logic_error;
    };

    class type_error
      : public std::logic_error
    {
        public:
            using logic_error::logic_error;
    };

    class option_error
      : public std::logic_error
    {
        public:
            using logic_error::logic_error;
    };

    inline auto operator|(Handle lhs, Handle rhs) -> Handle
    {
        return static_cast<Handle>(std::to_underlying(lhs) | std::to_underlying(rhs));
    }

    inline auto operator&(Handle lhs, Handle rhs) -> int
    {
        return std::to_underlying(lhs) & std::to_underlying(rhs);
    }

    template<typename T>
    class Converter
    {
        public:
            auto from_string(std::string const & s) const -> std::optional<T>
            {
                auto iss = std::istringstream(s);
                auto t = T();
                iss >> t;

                if (!iss.fail() && (iss.eof() || iss.peek() == std::istringstream::traits_type::eof()))
                {
                    return t;
                }
                return std::nullopt;
            }

            auto to_string(T const & t) const -> std::string
            {
                auto ostr = std::ostringstream();
                ostr << t;

                return ostr.str();
            }

            auto are_equal(T const & lhs, T const & rhs) const -> bool
            {
                return lhs == rhs;
            }
    };

    template<typename T>
    inline auto from_string(std::string const & s) -> std::optional<T>
    {
        auto const conv = Converter<T>();
        return conv.from_string(s);
    }

    template<typename T>
    inline auto to_string(T const & t) -> std::string
    {
        auto const conv = Converter<T>();
        return conv.to_string(t);
    }

    template<typename T>
    inline auto are_equal(T const & lhs, T const & rhs) -> bool
    {
        auto const conv = Converter<T>();
        return conv.are_equal(lhs, rhs);
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
                        if (m_value.type() != typeid(T))
                        {
                            throw type_error(std::format("wrong type: requested '{}' for argument of type '{}'", typeid(T).name(), m_value.type().name()));
                        }

                        return std::any_cast<T>(m_value);
                    }

                private:
                    std::any m_value;
            };

            class Parameters
            {
                public:
                    auto get(std::string_view name) const -> Value
                    {
                        if (auto const it = m_parameters.find(name); it == m_parameters.end())
                        {
                            throw name_error(std::format("no such argument: '{}'", name));
                        }
                        else
                        {
                            return it->second;
                        }
                    }

                    auto get_value(std::string_view name) const -> std::string
                    {
                        return get(name).get();
                    }

                    template<typename T>
                    auto get_value(std::string_view name) const -> T
                    {
                        return get(name).get<T>();
                    }

                    auto insert(std::string const & name, std::any const & value) -> void
                    {
                        (void) m_parameters.try_emplace(name, value);
                    }

                private:
                    std::map<std::string, Value, std::less<>> m_parameters;
            };

            struct Token
            {
                std::string m_token;
                bool m_consumed = false;
            };

            using Tokens = std::vector<Token>;
            using OptString = std::optional<std::string>;

            class HelpRequested {};
            class VersionRequested {};

        public:
            template<typename ...Args>
            decltype(auto) add_argument(Args &&... names)
            {
                return ArgumentBuilder(m_arguments, m_version, std::vector<std::string>{names...});
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
                        std::println("{}", format_help());
                        std::exit(EXIT_SUCCESS);
                    }

                    return get_parameters(m_arguments | std::views::transform(cast_to_argument));
                }
                catch (VersionRequested const &)
                {
                    if (m_handle & Handle::version)
                    {
                        std::println("{}", format_version());
                        std::exit(EXIT_SUCCESS);
                    }

                    return get_parameters(m_arguments | std::views::transform(cast_to_argument));
                }
                catch (parsing_error const & e)
                {
                    if (m_handle & Handle::errors)
                    {
                        std::println("{}", e.what());
                        std::println("{}", format_help());
                        std::exit(EXIT_FAILURE);
                    }

                    throw;
                }
            }

            auto add_mutually_exclusive_group()
            {
                return MutuallyExclusiveGroup(m_arguments, m_version);
            }

            auto prog(std::string prog) -> ArgumentParser &&
            {
                m_prog = std::move(prog);

                return std::move(*this);
            }

            auto usage(std::string usage) -> ArgumentParser &&
            {
                m_usage = std::move(usage);

                return std::move(*this);
            }

            auto description(std::string description) -> ArgumentParser &&
            {
                m_description = std::move(description);

                return std::move(*this);
            }

            auto epilog(std::string epilog) -> ArgumentParser &&
            {
                m_epilog = std::move(epilog);

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
                return Formatter::format_usage(m_arguments | std::views::transform(cast_to_formattable), m_usage, m_prog);
            }

            auto format_help() const -> std::string
            {
                return Formatter::format_help(m_arguments | std::views::transform(cast_to_formattable), m_prog, m_usage, m_description, m_epilog);
            }

            auto format_version() const -> std::string
            {
                return Formatter::format_version(m_version, m_prog);
            }

            ArgumentParser()
            {
                add_argument("-h", "--help").action(help).help("show this help message and exit");
            }

        private:
            static auto extract_filename(std::string_view path) -> std::string_view
            {
                if (auto path_separator = path.find_last_of("/\\"); path_separator != std::string_view::npos)
                {
                    return path.substr(path_separator + 1);
                }

                return path;
            }

            auto parse_args(Tokens tokens) -> Parameters
            {
                auto arguments = m_arguments | std::views::transform(cast_to_argument);

                parse_optional_arguments(arguments, tokens);
                parse_positional_arguments(arguments, tokens);

                consume_pseudo_arguments(tokens);

                check_unrecognised_arguments(tokens);
                check_excluded_arguments(arguments);
                check_missing_arguments(arguments);

                return get_parameters(arguments);
            }

            static auto get_tokens(int argc, char const * const argv[]) -> Tokens
            {
                return Tokens(&argv[1], &argv[argc]);
            }

            static auto join(std::vector<std::string> const & strings, std::string_view separator) -> std::string
            {
                return join(strings | std::views::all, separator);
            }

            static auto join(std::ranges::view auto strings, std::string_view separator) -> std::string
            {
                auto const joined = std::ranges::fold_left_first(strings, [=](auto l, auto const & r) { l += separator; l += r; return std::move(l); });
                return joined.value_or(std::string());
            }

            static auto parse_optional_arguments(std::ranges::view auto arguments, Tokens & tokens) -> void
            {
                for (auto & argument : arguments
                    | std::views::filter([](auto const & argument) { return !argument.is_positional() && argument.expects_argument(); }))
                {
                    argument.parse_tokens(tokens);
                }

                for (auto & argument : arguments
                    | std::views::filter([](auto const & argument) { return !argument.is_positional() && !argument.expects_argument(); }))
                {
                    argument.parse_tokens(tokens);
                }
            }

            static auto parse_positional_arguments(std::ranges::view auto arguments, Tokens & tokens) -> void
            {
                for (auto & argument : arguments
                    | std::views::filter(&Argument::is_positional))
                {
                    argument.parse_tokens(tokens);
                }
            }

            static auto consume_pseudo_arguments(Tokens & tokens) -> void
            {
                for (auto & token : tokens
                    | std::views::filter([](auto const & t) { return t.m_token == "--"; }))
                {
                    token.m_consumed = true;
                }
            }

            static auto check_unrecognised_arguments(Tokens const & tokens) -> void
            {
                auto unconsumed = tokens
                    | std::views::filter([](auto const & token) { return !token.m_consumed; });
                if (!unconsumed.empty())
                {
                    throw parsing_error(std::format("unrecognised arguments: {}", join(unconsumed | std::views::transform(&Token::m_token), " ")));
                }
            }

            static auto check_excluded_arguments(std::ranges::view auto arguments) -> void
            {
                auto const filter = [](auto const & argument) { return argument.is_present() && argument.is_mutually_exclusive(); };

                for (auto const & argument1 : arguments | std::views::filter(filter))
                {
                    for (auto const & argument2 : arguments | std::views::filter(filter))
                    {
                        if ((&argument2 != &argument1) && argument2.is_mutually_exclusive_with(argument1))
                        {
                            throw parsing_error(std::format("argument {}: not allowed with argument {}", argument2.get_joined_names(), argument1.get_joined_names()));
                        }
                    }
                }
            }

            static auto check_missing_arguments(std::ranges::view auto arguments) -> void
            {
                auto error_message = OptString();

                for (auto const & argument : arguments
                    | std::views::filter([](auto const & argument) { return argument.is_required() && !argument.has_value(); }))
                {
                    if (!error_message)
                    {
                        error_message = "the following arguments are required: " + argument.get_joined_names();
                    }
                    else
                    {
                        *error_message += " " + argument.get_joined_names();
                    }
                }

                if (error_message)
                {
                    throw parsing_error(*error_message);
                }
            }

            static auto get_parameters(std::ranges::view auto arguments) -> Parameters
            {
                auto result = Parameters();

                for (auto const & argument : arguments)
                {
                    result.insert(argument.get_dest_name(), argument.get_value());
                }

                return result;
            }

            static auto replace_prog(std::string text, OptString const & replacement) -> std::string
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

                    virtual auto from_string(std::string const & string) const -> std::any = 0;
                    virtual auto to_string(std::any const & value) const -> std::string = 0;
                    virtual auto compare(std::any const & lhs, std::any const & rhs) const -> bool = 0;
                    virtual auto transform(std::vector<std::any> const & values) const -> std::any = 0;
                    virtual auto append(std::any const & value, std::any & values) const -> void = 0;
                    virtual auto size(std::any const & value) const -> std::size_t = 0;
            };

            template<typename T>
            class TypeHandlerT final : public TypeHandler
            {
                public:
                    auto from_string(std::string const & string) const -> std::any override
                    {
                        if constexpr (std::is_same_v<std::string, T>)
                        {
                            return std::any(string);
                        }
                        else
                        {
                            if (auto const optvalue = argparse::from_string<T>(string); optvalue.has_value())
                            {
                                return std::any(*optvalue);
                            }
                            else
                            {
                                return std::any();
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
                            return argparse::to_string(std::any_cast<T>(value));
                        }
                    }

                    auto compare(std::any const & lhs, std::any const & rhs) const -> bool override
                    {
                        return argparse::are_equal(std::any_cast<T>(lhs), std::any_cast<T>(rhs));
                    }

                    auto transform(std::vector<std::any> const & values) const -> std::any override
                    {
                        return std::any(
                            values
                            | std::views::transform([](auto const & value) { return std::any_cast<T>(value); })
                            | std::ranges::to<std::vector>());
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

            class Argument
            {
                public:
                    virtual auto parse_tokens(Tokens & tokens) -> void = 0;
                    virtual auto is_positional() const -> bool = 0;
                    virtual auto is_present() const -> bool = 0;
                    virtual auto is_required() const -> bool = 0;
                    virtual auto is_mutually_exclusive() const -> bool = 0;
                    virtual auto is_mutually_exclusive_with(Argument const & other) const -> bool = 0;
                    virtual auto expects_argument() const -> bool = 0;
                    virtual auto has_value() const -> bool = 0;
                    virtual auto get_value() const -> std::any = 0;
                    virtual auto get_dest_name() const -> std::string = 0;
                    virtual auto get_joined_names() const -> std::string = 0;

                protected:
                    ~Argument() = default;
            };

            class Formattable
            {
                public:
                    virtual auto get_name() const -> std::string const & = 0;
                    virtual auto get_names() const -> std::vector<std::string> const & = 0;
                    virtual auto get_help() const -> std::string const & = 0;
                    virtual auto is_positional() const -> bool = 0;
                    virtual auto is_required() const -> bool = 0;
                    virtual auto is_mutually_exclusive() const -> bool = 0;
                    virtual auto is_mutually_exclusive_with(Formattable const & other) const -> bool = 0;
                    virtual auto has_nargs() const -> bool = 0;
                    virtual auto has_nargs_number() const -> bool = 0;
                    virtual auto has_choices() const -> bool = 0;
                    virtual auto expects_argument() const -> bool = 0;
                    virtual auto get_joined_choices(std::string_view separator) const -> std::string = 0;
                    virtual auto get_metavar_name() const -> std::string = 0;
                    virtual auto get_nargs_number() const -> std::size_t = 0;
                    virtual auto get_nargs_option() const -> Nargs = 0;

                protected:
                    ~Formattable() = default;
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
                bool required = false;
                std::vector<std::any> choices;
                std::optional<std::variant<std::size_t, Nargs>> nargs;
                MutuallyExclusiveGroup const * mutually_exclusive_group = nullptr;
                std::unique_ptr<TypeHandler> type_handler = std::make_unique<TypeHandlerT<std::string>>();
            };

            class ArgumentImpl
            {
                public:
                    explicit ArgumentImpl(Options options)
                      : m_options(std::move(options))
                    {
                    }

                    auto get_name() const -> std::string const &
                    {
                        return m_options.names.front();
                    }

                    auto get_names() const -> std::vector<std::string> const &
                    {
                        return m_options.names;
                    }

                    auto get_joined_names() const -> std::string
                    {
                        return join(m_options.names, "/");
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

                    auto is_mutually_exclusive_with(ArgumentImpl const & other) const -> bool
                    {
                        return (m_options.mutually_exclusive_group != nullptr) && (m_options.mutually_exclusive_group == other.m_options.mutually_exclusive_group);
                    }

                    auto expects_argument() const -> bool
                    {
                        return m_options.action == store || m_options.action == append;
                    }

                    auto get_help() const -> std::string const &
                    {
                        return m_options.help;
                    }

                    auto get_default() const -> std::any const &
                    {
                        return m_options.default_;
                    }

                    auto get_const() const -> std::any const &
                    {
                        return m_options.const_;
                    }

                    auto get_metavar() const -> std::string const &
                    {
                        return m_options.metavar;
                    }

                    auto get_dest() const -> std::string const &
                    {
                        return m_options.dest;
                    }

                    auto get_required() const -> bool
                    {
                        return m_options.required;
                    }

                    auto get_action() const -> Action
                    {
                        return m_options.action;
                    }

                    auto has_choices() const -> bool
                    {
                        return !m_options.choices.empty();
                    }

                    auto get_joined_choices(std::string_view separator) const -> std::string
                    {
                        return join(m_options.choices | std::views::transform([&](auto const & choice) { return m_options.type_handler->to_string(choice); }), separator);
                    }

                    auto parse_arguments(std::ranges::view auto tokens, std::function<std::string()> name_for_error) const -> std::any
                    {
                        auto const values = consume_tokens(tokens, name_for_error);
                        return m_options.type_handler->transform(values);
                    }

                    auto consume_token(Token & token, std::function<std::string()> name_for_error) const -> std::any
                    {
                        token.m_consumed = true;
                        return process_token(token.m_token, name_for_error);
                    }

                    auto process_token(std::string const & token, std::function<std::string()> name_for_error) const -> std::any
                    {
                        auto const value = m_options.type_handler->from_string(token);
                        if (!value.has_value())
                        {
                            throw parsing_error(std::format("argument {}: invalid value: '{}'", name_for_error(), token));
                        }
                        check_choices(value);
                        return value;
                    }

                    auto consume_tokens(std::ranges::view auto tokens, std::function<std::string()> name_for_error) const -> std::vector<std::any>
                    {
                        auto result = std::vector<std::any>();
                        auto consumed = std::vector<Token *>();
                        for (auto & token : tokens)
                        {
                            result.push_back(process_token(token.m_token, name_for_error));
                            consumed.push_back(&token);
                        }
                        std::ranges::for_each(consumed, [](auto token) { token->m_consumed = true; });
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
                                get_joined_names(),
                                m_options.type_handler->to_string(value),
                                get_joined_choices(", "));
                            throw parsing_error(message);
                        }
                    }

                    auto get_transformed(std::vector<std::any> const & values) const -> std::any
                    {
                        return m_options.type_handler->transform(values);
                    }

                    auto get_size(std::any const & value) const -> std::size_t
                    {
                        return m_options.type_handler->size(value);
                    }

                    auto append_value(std::any const & value, std::any & values) const -> void
                    {
                        m_options.type_handler->append(value, values);
                    }

                    static auto is_negative_number(std::string const & token) -> bool
                    {
                        if (auto const parsed = from_string<double>(token); parsed.has_value())
                        {
                            return true;
                        }
                        return false;
                    }

                private:
                    Options m_options;
            };

            class StoreAction
            {
                public:
                    auto perform(ArgumentImpl const & impl, std::any & value, std::function<std::string()> name_for_error, std::string const & val, std::ranges::view auto tokens) const -> void
                    {
                        if (impl.has_nargs())
                        {
                            if (impl.has_nargs_number())
                            {
                                value = parse_arguments_number(impl, name_for_error, tokens);
                            }
                            else
                            {
                                value = parse_arguments_option(impl, name_for_error, tokens);
                            }
                        }
                        else
                        {
                            if (val.empty())
                            {
                                value = impl.consume_token(tokens.front(), name_for_error);
                            }
                            else
                            {
                                value = impl.process_token(val, name_for_error);
                            }
                        }
                    }

                    auto check_errors(ArgumentImpl const & impl, std::string_view value, std::ranges::view auto tokens) const -> void
                    {
                        if (!impl.has_nargs() && value.empty() && tokens.empty())
                        {
                            throw parsing_error(std::format("argument {}: expected one argument", impl.get_joined_names()));
                        }
                    }

                    auto assign_non_present_value(ArgumentImpl const & impl, std::any & value) const -> void
                    {
                        value = impl.get_default();
                    }

                private:
                    auto parse_arguments_number(ArgumentImpl const & impl, std::function<std::string()> name_for_error, std::ranges::view auto tokens) const -> std::any
                    {
                        auto const nargs_number = impl.get_nargs_number();
                        auto const values = impl.consume_tokens(tokens | std::views::take(nargs_number), name_for_error);
                        if (values.size() < nargs_number)
                        {
                            throw parsing_error(std::format("argument {}: expected {} argument{}", impl.get_joined_names(), std::to_string(nargs_number), nargs_number > 1 ? "s" : ""));
                        }
                        return impl.get_transformed(values);
                    }

                    auto parse_arguments_option(ArgumentImpl const & impl, std::function<std::string()> name_for_error, std::ranges::view auto tokens) const -> std::any
                    {
                        switch (impl.get_nargs_option())
                        {
                            case zero_or_one:
                            {
                                if (!tokens.empty())
                                {
                                    return impl.consume_token(tokens.front(), name_for_error);
                                }
                                else
                                {
                                    return impl.get_const();
                                }
                            }
                            case zero_or_more:
                            {
                                return impl.parse_arguments(tokens, name_for_error);
                            }
                            case one_or_more:
                            {
                                if (auto const values = impl.consume_tokens(tokens, name_for_error); !values.empty())
                                {
                                    return impl.get_transformed(values);
                                }
                                else
                                {
                                    throw parsing_error(std::format("argument {}: expected at least one argument", impl.get_joined_names()));
                                }
                            }
                        }

                        std::unreachable();
                    }
            };

            class StoreConstAction
            {
                public:
                    auto perform(ArgumentImpl const & impl, std::any & value, std::function<std::string()> /* name_for_error */, std::string const & /* val */, std::ranges::view auto /* tokens */) const -> void
                    {
                        value = impl.get_const();
                    }

                    auto check_errors(ArgumentImpl const & impl, std::string_view value, std::ranges::view auto /* tokens */) const -> void
                    {
                        if (!value.empty())
                        {
                            throw parsing_error(std::format("argument {}: ignored explicit argument '{}'", impl.get_joined_names(), value));
                        }
                    }

                    auto assign_non_present_value(ArgumentImpl const & impl, std::any & value) const -> void
                    {
                        value = impl.get_default();
                    }
            };

            class StoreTrueAction
            {
                public:
                    auto perform(ArgumentImpl const & /* impl */, std::any & value, std::function<std::string()> /* name_for_error */, std::string const & /* val */, std::ranges::view auto /* tokens */) const -> void
                    {
                        value = true;
                    }

                    auto check_errors(ArgumentImpl const & impl, std::string_view value, std::ranges::view auto /* tokens */) const -> void
                    {
                        if (!value.empty())
                        {
                            throw parsing_error(std::format("argument {}: ignored explicit argument '{}'", impl.get_joined_names(), value));
                        }
                    }

                    auto assign_non_present_value(ArgumentImpl const & /* impl */, std::any & value) const -> void
                    {
                        value = false;
                    }
            };

            class StoreFalseAction
            {
                public:
                    auto perform(ArgumentImpl const & /* impl */, std::any & value, std::function<std::string()> /* name_for_error */, std::string const & /* val */, std::ranges::view auto /* tokens */) const -> void
                    {
                        value = false;
                    }

                    auto check_errors(ArgumentImpl const & impl, std::string_view value, std::ranges::view auto /* tokens */) const -> void
                    {
                        if (!value.empty())
                        {
                            throw parsing_error(std::format("argument {}: ignored explicit argument '{}'", impl.get_joined_names(), value));
                        }
                    }

                    auto assign_non_present_value(ArgumentImpl const & /* impl */, std::any & value) const -> void
                    {
                        value = true;
                    }
            };

            class HelpAction
            {
                public:
                    auto perform(ArgumentImpl const & /* impl */, std::any & value, std::function<std::string()> /* name_for_error */, std::string const & /* val */, std::ranges::view auto /* tokens */) const -> void
                    {
                        value = true;
                        throw HelpRequested();
                    }

                    auto check_errors(ArgumentImpl const & /* impl */, std::string_view /* value */, std::ranges::view auto /* tokens */) const -> void
                    {
                    }

                    auto assign_non_present_value(ArgumentImpl const & /* impl */, std::any & value) const -> void
                    {
                        value = false;
                    }
            };

            class VersionAction
            {
                public:
                    auto perform(ArgumentImpl const & /* impl */, std::any & value, std::function<std::string()> /* name_for_error */, std::string const & /* val */, std::ranges::view auto /* tokens */) const -> void
                    {
                        value = true;
                        throw VersionRequested();
                    }

                    auto check_errors(ArgumentImpl const & /* impl */, std::string_view /* value */, std::ranges::view auto /* tokens */) const -> void
                    {
                    }

                    auto assign_non_present_value(ArgumentImpl const & /* impl */, std::any & value) const -> void
                    {
                        value = false;
                    }
            };

            class CountAction
            {
                public:
                    auto perform(ArgumentImpl const & /* impl */, std::any & value, std::function<std::string()> /* name_for_error */, std::string const & /* val */, std::ranges::view auto /* tokens */) const -> void
                    {
                        if (!value.has_value())
                        {
                            value = 1;
                        }
                        else
                        {
                            ++std::any_cast<int &>(value);
                        }
                    }

                    auto check_errors(ArgumentImpl const & impl, std::string_view value, std::ranges::view auto /* tokens */) const -> void
                    {
                        if (!value.empty())
                        {
                            throw parsing_error(std::format("argument {}: ignored explicit argument '{}'", impl.get_joined_names(), value));
                        }
                    }

                    auto assign_non_present_value(ArgumentImpl const & impl, std::any & value) const -> void
                    {
                        value = impl.get_default();
                    }
            };

            class AppendAction
            {
                public:
                    auto perform(ArgumentImpl const & impl, std::any & value, std::function<std::string()> name_for_error, std::string const & val, std::ranges::view auto tokens) const -> void
                    {
                        if (val.empty())
                        {
                            if (!value.has_value())
                            {
                                auto const values = impl.consume_tokens(tokens | std::views::take(1), name_for_error);
                                value = impl.get_transformed(values);
                            }
                            else
                            {
                                auto const v = impl.consume_token(tokens.front(), name_for_error);
                                impl.append_value(v, value);
                            }
                        }
                        else
                        {
                            if (!value.has_value())
                            {
                                auto const values = impl.consume_tokens(std::views::single(Token{val}), name_for_error);
                                value = impl.get_transformed(values);
                            }
                            else
                            {
                                auto const v = impl.process_token(val, name_for_error);
                                impl.append_value(v, value);
                            }
                        }
                    }

                    auto check_errors(ArgumentImpl const & impl, std::string_view value, std::ranges::view auto tokens) const -> void
                    {
                        if (value.empty() && tokens.empty())
                        {
                            throw parsing_error(std::format("argument {}: expected one argument", impl.get_joined_names()));
                        }
                    }

                    auto assign_non_present_value(ArgumentImpl const & impl, std::any & value) const -> void
                    {
                        value = impl.get_default();
                    }
            };

            class PositionalArgument final : public Argument, public Formattable
            {
                private:
                    auto parse_arguments_option(std::ranges::view auto tokens) -> std::any
                    {
                        switch (get_nargs_option())
                        {
                            case zero_or_one:
                            {
                                if (!tokens.empty())
                                {
                                    return m_impl.consume_token(tokens.front(), get_name_for_error());
                                }
                                else
                                {
                                    return m_impl.get_default();
                                }
                            }
                            case zero_or_more:
                            {
                                return m_impl.parse_arguments(tokens, get_name_for_error());
                            }
                            case one_or_more:
                            {
                                if (auto const values = m_impl.consume_tokens(tokens, get_name_for_error()); !values.empty())
                                {
                                    return m_impl.get_transformed(values);
                                }
                                else
                                {
                                    return std::any();
                                }
                            }
                        }

                        std::unreachable();
                    }

                    auto get_name_for_error() const -> std::function<std::string()>
                    {
                        return [&]() { return get_dest_name(); };
                    }

                    static auto get_consumable(Tokens & tokens)
                    {
                        return tokens
                            | std::views::drop_while([](auto const & token)
                                {
                                    return token.m_consumed;
                                })
                            | std::views::take_while([](auto const & token)
                                {
                                    return !token.m_consumed;
                                })
                            | std::views::filter([past_pseudo_arg = false](auto const & token) mutable
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
                                    if (!token.m_token.starts_with("-"))
                                    {
                                        return true;
                                    }
                                    if (ArgumentImpl::is_negative_number(token.m_token))
                                    {
                                        return true;
                                    }
                                    return false;
                                });
                    }

                public:
                    explicit PositionalArgument(Options options)
                      : m_impl(std::move(options))
                    {
                    }

                    PositionalArgument(PositionalArgument && other) noexcept = default;

                    PositionalArgument & operator=(PositionalArgument && other) noexcept = default;

                    auto parse_tokens(Tokens & tokens) -> void override
                    {
                        auto consumable = get_consumable(tokens);

                        if (has_nargs())
                        {
                            if (has_nargs_number())
                            {
                                m_value = m_impl.parse_arguments(consumable | std::views::take(get_nargs_number()), get_name_for_error());
                            }
                            else
                            {
                                m_value = parse_arguments_option(consumable);
                            }
                        }
                        else
                        {
                            if (!consumable.empty())
                            {
                                m_value = m_impl.consume_token(consumable.front(), get_name_for_error());
                            }
                        }
                    }

                    auto get_dest_name() const -> std::string override
                    {
                        return m_impl.get_dest().empty()
                            ? get_name()
                            : m_impl.get_dest();
                    }

                    auto get_metavar_name() const -> std::string override
                    {
                        return m_impl.get_metavar().empty()
                            ? get_name()
                            : m_impl.get_metavar();
                    }

                    auto has_value() const -> bool override
                    {
                        return has_nargs() && has_nargs_number()
                            ? m_impl.get_size(m_value) == get_nargs_number()
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

                    auto is_mutually_exclusive() const -> bool override
                    {
                        return m_impl.is_mutually_exclusive();
                    }

                    auto is_mutually_exclusive_with(Argument const & other) const -> bool override
                    {
                        return m_impl.is_mutually_exclusive_with(static_cast<PositionalArgument const &>(other).m_impl);
                    }

                    auto is_mutually_exclusive_with(Formattable const & other) const -> bool override
                    {
                        return m_impl.is_mutually_exclusive_with(static_cast<PositionalArgument const &>(other).m_impl);
                    }

                    auto expects_argument() const -> bool override
                    {
                        return m_impl.expects_argument();
                    }

                    auto get_joined_names() const -> std::string override
                    {
                        return m_impl.get_joined_names();
                    }

                    auto get_name() const -> std::string const & override
                    {
                        return m_impl.get_name();
                    }

                    auto get_names() const -> std::vector<std::string> const & override
                    {
                        return m_impl.get_names();
                    }

                    auto get_help() const -> std::string const & override
                    {
                        return m_impl.get_help();
                    }

                    auto has_nargs() const -> bool override
                    {
                        return m_impl.has_nargs();
                    }

                    auto has_nargs_number() const -> bool override
                    {
                        return m_impl.has_nargs_number();
                    }

                    auto has_choices() const -> bool override
                    {
                        return m_impl.has_choices();
                    }

                    auto get_joined_choices(std::string_view separator) const -> std::string override
                    {
                        return m_impl.get_joined_choices(separator);
                    }

                    auto get_nargs_number() const -> std::size_t override
                    {
                        return m_impl.get_nargs_number();
                    }

                    auto get_nargs_option() const -> Nargs override
                    {
                        return m_impl.get_nargs_option();
                    }

                private:
                    ArgumentImpl m_impl;
                    std::any m_value;
            };

            class OptionalArgument final : public Argument, public Formattable
            {
                private:
                    auto perform_action(std::string const & value, std::ranges::view auto tokens) -> void
                    {
                        std::visit([&](auto const & action) { action.perform(m_impl, m_value, get_name_for_error(), value, tokens); }, m_action);
                    }

                    auto has_arg(auto it) const -> std::string_view
                    {
                        for (auto const & name: get_names())
                        {
                            if (name[1] != '-')
                            {
                                if (it->m_token.starts_with("-") && !it->m_token.starts_with("--") && it->m_token.contains(name[1]))
                                {
                                    return name;
                                }
                            }
                            else
                            {
                                auto const [first_it, second_it] = std::ranges::mismatch(name, it->m_token);
                                if (first_it == name.end() && (second_it == it->m_token.end() || *second_it == '='))
                                {
                                    return name;
                                }
                            }
                        }

                        return std::string_view();
                    }

                    auto consume_name(auto it, std::string_view name) const -> std::string
                    {
                        auto & token = *it;

                        if (token.m_token.starts_with("--"))
                        {
                            token.m_consumed = true;

                            if (auto const pos = token.m_token.find('='); pos != std::string::npos)
                            {
                                return token.m_token.substr(pos + 1);
                            }

                            return "";
                        }

                        if (token.m_token.size() == 2)
                        {
                            token.m_consumed = true;
                            return "";
                        }

                        auto const pos = token.m_token.find(name[1]);
                        token.m_token.erase(pos, 1);

                        if (!expects_argument())
                        {
                            return "";
                        }

                        if (pos == 1)
                        {
                            token.m_consumed = true;
                            return token.m_token.substr(pos);
                        }
                        else
                        {
                            auto const value = token.m_token.substr(pos);
                            token.m_token.resize(pos);
                            return value;
                        }
                    }

                    auto get_name_for_dest() const -> std::string
                    {
                        for (auto const & name : get_names())
                        {
                            if (name.starts_with("--"))
                            {
                                return name.substr(2);
                            }
                        }

                        return get_name().substr(1);
                    }

                    auto get_name_for_error() const -> std::function<std::string()>
                    {
                        return [&]() { return get_joined_names(); };
                    }

                    auto check_errors(std::string_view value, std::ranges::view auto tokens) const -> void
                    {
                        std::visit([&](auto const & action) { action.check_errors(m_impl, value, tokens); }, m_action);
                    }

                    auto assign_non_present_value() -> void
                    {
                        std::visit([&](auto const & action) { action.assign_non_present_value(m_impl, m_value); }, m_action);
                    }

                    static auto get_consumable(Tokens & tokens)
                    {
                        return tokens
                            | std::views::drop_while([](auto const & token) { return token.m_consumed; })
                            | std::views::take_while([](auto const & token) { return token.m_token != "--"; });
                    }

                    static auto get_consumable_args(auto it, std::ranges::view auto consumable)
                    {
                        return std::ranges::subrange(std::ranges::next(it), consumable.end())
                            | std::views::take_while([](auto const & token)
                                {
                                    if (!token.m_token.starts_with("-"))
                                    {
                                        return true;
                                    }
                                    if (ArgumentImpl::is_negative_number(token.m_token))
                                    {
                                        return true;
                                    }
                                    return false;
                                });
                    }

                    auto create_action() const -> std::variant<StoreAction, StoreConstAction, StoreTrueAction, StoreFalseAction, HelpAction, VersionAction, CountAction, AppendAction>
                    {
                        switch (m_impl.get_action())
                        {
                            case store:
                                return StoreAction();
                            case store_true:
                                return StoreTrueAction();
                            case store_false:
                                return StoreFalseAction();
                            case store_const:
                                return StoreConstAction();
                            case help:
                                return HelpAction();
                            case version:
                                return VersionAction();
                            case count:
                                return CountAction();
                            case append:
                                return AppendAction();
                        }

                        std::unreachable();
                    }

                private:
                    ArgumentImpl m_impl;
                    std::any m_value;
                    bool m_present = false;
                    std::variant<StoreAction, StoreConstAction, StoreTrueAction, StoreFalseAction, HelpAction, VersionAction, CountAction, AppendAction> m_action;

                public:
                    explicit OptionalArgument(Options options)
                      : m_impl(std::move(options))
                      , m_action(create_action())
                    {
                    }

                    OptionalArgument(OptionalArgument && other) noexcept = default;

                    OptionalArgument & operator=(OptionalArgument && other) noexcept = default;

                    auto parse_tokens(Tokens & tokens) -> void override
                    {
                        auto consumable = get_consumable(tokens);

                        for (auto it = consumable.begin(); it != consumable.end();)
                        {
                            if (auto const name = has_arg(it); !name.empty())
                            {
                                auto const value = consume_name(it, name);
                                auto consumable_args = get_consumable_args(it, consumable);

                                check_errors(value, consumable_args);

                                perform_action(value, consumable_args);

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
                            assign_non_present_value();
                        }
                    }

                    auto get_dest_name() const -> std::string override
                    {
                        if (!m_impl.get_dest().empty())
                        {
                            return m_impl.get_dest();
                        }

                        auto dest = get_name_for_dest();

                        std::ranges::replace(dest, '-', '_');

                        return dest;
                    }

                    auto get_metavar_name() const -> std::string override
                    {
                        if (!m_impl.get_metavar().empty())
                        {
                            return m_impl.get_metavar();
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
                        return m_impl.get_required();
                    }

                    auto is_positional() const -> bool override
                    {
                        return false;
                    }

                    auto is_present() const -> bool override
                    {
                        return m_present;
                    }

                    auto is_mutually_exclusive() const -> bool override
                    {
                        return m_impl.is_mutually_exclusive();
                    }

                    auto is_mutually_exclusive_with(Argument const & other) const -> bool override
                    {
                        return m_impl.is_mutually_exclusive_with(static_cast<OptionalArgument const &>(other).m_impl);
                    }

                    auto is_mutually_exclusive_with(Formattable const & other) const -> bool override
                    {
                        return m_impl.is_mutually_exclusive_with(static_cast<OptionalArgument const &>(other).m_impl);
                    }

                    auto expects_argument() const -> bool override
                    {
                        return m_impl.expects_argument();
                    }

                    auto get_joined_names() const -> std::string override
                    {
                        return m_impl.get_joined_names();
                    }

                    auto get_name() const -> std::string const & override
                    {
                        return m_impl.get_name();
                    }

                    auto get_names() const -> std::vector<std::string> const & override
                    {
                        return m_impl.get_names();
                    }

                    auto get_help() const -> std::string const & override
                    {
                        return m_impl.get_help();
                    }

                    auto has_nargs() const -> bool override
                    {
                        return m_impl.has_nargs();
                    }

                    auto has_nargs_number() const -> bool override
                    {
                        return m_impl.has_nargs_number();
                    }

                    auto has_choices() const -> bool override
                    {
                        return m_impl.has_choices();
                    }

                    auto get_joined_choices(std::string_view separator) const -> std::string override
                    {
                        return m_impl.get_joined_choices(separator);
                    }

                    auto get_nargs_number() const -> std::size_t override
                    {
                        return m_impl.get_nargs_number();
                    }

                    auto get_nargs_option() const -> Nargs override
                    {
                        return m_impl.get_nargs_option();
                    }
            };

            using ArgumentVariant = std::variant<PositionalArgument, OptionalArgument>;
            using Arguments = std::vector<ArgumentVariant>;

            class Formatter
            {
                public:
                    static auto format_usage(std::ranges::view auto arguments, OptString const & usage, OptString const & prog) -> std::string
                    {
                        if (usage)
                        {
                            return std::format("usage: {}", replace_prog(*usage, prog));
                        }

                        return std::format("usage: {}{}{}", *prog, format_usage_optionals(arguments), format_usage_positionals(arguments));
                    }

                    static auto format_help(std::ranges::view auto arguments, OptString const & prog, OptString const & usage, OptString const & description, OptString const & epilog) -> std::string
                    {
                        auto message = format_usage(arguments, usage, prog);
                        auto positionals = format_help_positionals(arguments, prog);
                        auto optionals = format_help_optionals(arguments, prog);

                        if (description)
                        {
                            message += "\n\n" + replace_prog(*description, prog);
                        }

                        if (!positionals.empty())
                        {
                            message += "\n\npositional arguments:" + positionals;
                        }

                        if (!optionals.empty())
                        {
                            message += "\n\noptional arguments:" + optionals;
                        }

                        if (epilog)
                        {
                            message += "\n\n" + replace_prog(*epilog, prog);
                        }

                        return message;
                    }

                    static auto format_version(OptString const & version, OptString const & prog) -> std::string
                    {
                        return replace_prog(*version, prog);
                    }

                private:
                    static auto format_usage_positionals(std::ranges::view auto arguments) -> std::string
                    {
                        auto usage_text = std::string();

                        for (auto const & argument : arguments
                            | std::views::filter(&Formattable::is_positional))
                        {
                            if (argument.has_nargs())
                            {
                                usage_text += format_nargs(argument);
                            }
                            else
                            {
                                usage_text += " ";
                                usage_text += format_arg(argument);
                            }
                        }

                        return usage_text;
                    }

                    static auto format_usage_optionals(std::ranges::view auto arguments) -> std::string
                    {
                        auto usage_text = std::string();

                        auto non_positionals = arguments | std::views::filter([](auto const & argument) { return !argument.is_positional(); });

                        for (auto it = non_positionals.begin(); it != non_positionals.end(); ++it)
                        {
                            auto const & argument = *it;

                            if (argument.is_required())
                            {
                                usage_text += " ";
                            }
                            else if (argument.is_mutually_exclusive() && it != non_positionals.begin() && argument.is_mutually_exclusive_with(*std::ranges::prev(it)))
                            {
                                usage_text += " | ";
                            }
                            else
                            {
                                usage_text += " [";
                            }

                            if (argument.has_nargs())
                            {
                                usage_text += argument.get_name();
                                usage_text += format_nargs(argument);
                            }
                            else
                            {
                                usage_text += argument.get_name();
                                if (argument.expects_argument())
                                {
                                    usage_text += " ";
                                    usage_text += format_arg(argument);
                                }
                            }

                            if (argument.is_required())
                            {
                                // skip
                            }
                            else if (argument.is_mutually_exclusive() && std::ranges::next(it) != non_positionals.end() && argument.is_mutually_exclusive_with(*std::ranges::next(it)))
                            {
                                // skip
                            }
                            else
                            {
                                usage_text += "]";
                            }
                        }

                        return usage_text;
                    }

                    static auto format_help_positionals(std::ranges::view auto arguments, OptString const & prog) -> std::string
                    {
                        auto help_text = std::string();

                        for (auto const & argument : arguments
                            | std::views::filter(&Formattable::is_positional))
                        {
                            auto help_line = "  " + format_arg(argument);

                            if (auto const & help = argument.get_help(); !help.empty())
                            {
                                help_line += help_string_separation(help_line.size());
                                help_line += replace_prog(help, prog);
                            }

                            help_text += '\n' + help_line;
                        }

                        return help_text;
                    }

                    static auto format_help_optionals(std::ranges::view auto arguments, OptString const & prog) -> std::string
                    {
                        auto help_text = std::string();

                        for (auto const & argument : arguments
                            | std::views::filter([](auto const & argument) { return !argument.is_positional(); }))
                        {
                            auto help_line = std::string("  ");
                            auto const formatted_arg = format(argument);

                            for (auto name_it = argument.get_names().begin(); name_it != argument.get_names().end(); ++name_it)
                            {
                                if (name_it != argument.get_names().begin())
                                {
                                    help_line += ", ";
                                }

                                help_line += *name_it;
                                help_line += formatted_arg;
                            }

                            if (auto const & help = argument.get_help(); !help.empty())
                            {
                                help_line += help_string_separation(help_line.size());
                                help_line += replace_prog(help, prog);
                            }

                            help_text += '\n' + help_line;
                        }

                        return help_text;
                    }

                    static auto format(Formattable const & argument) -> std::string
                    {
                        if (!argument.expects_argument())
                        {
                            return std::string();
                        }

                        if (argument.has_nargs())
                        {
                            return format_nargs(argument);
                        }
                        else
                        {
                            return " " + format_arg(argument);
                        }
                    }

                    static auto format_arg(Formattable const & argument) -> std::string
                    {
                        return argument.has_choices()
                            ? "{" + argument.get_joined_choices(",") + "}"
                            : argument.get_metavar_name();
                    }

                    static auto format_nargs(Formattable const & argument) -> std::string
                    {
                        auto const formatted_arg = format_arg(argument);

                        if (argument.has_nargs_number())
                        {
                            return std::ranges::fold_left(std::views::repeat(" " + formatted_arg, argument.get_nargs_number()), std::string(), std::plus());
                        }
                        else
                        {
                            switch (argument.get_nargs_option())
                            {
                                case zero_or_one:
                                    return std::format(" [{0}]", formatted_arg);
                                case zero_or_more:
                                    return std::format(" [{0} [{0} ...]]", formatted_arg);
                                case one_or_more:
                                    return std::format(" {0} [{0} ...]", formatted_arg);
                            }

                            std::unreachable();
                        }
                    }

                    static auto help_string_separation(std::size_t help_line_length) -> std::string_view
                    {
                        constexpr auto fill = std::string_view("\n                        ");
                        return help_line_length < 23
                            ? fill.substr(help_line_length + 1)
                            : fill;
                    }
            };

            class MutuallyExclusiveGroup
            {
                public:
                    MutuallyExclusiveGroup(Arguments & arguments, OptString & version)
                      : m_arguments(arguments)
                      , m_version(version)
                    {
                    }

                    template<typename ...Args>
                    decltype(auto) add_argument(Args &&... names)
                    {
                        return ArgumentBuilder(m_arguments, m_version, std::vector<std::string>{names...}, this);
                    }

                private:
                    Arguments & m_arguments;
                    OptString & m_version;
            };

            class ArgumentBuilder
            {
                public:
                    ArgumentBuilder(Arguments & arguments, OptString & version, std::vector<std::string> names, MutuallyExclusiveGroup const * group = nullptr)
                      : m_arguments(arguments)
                      , m_version(version)
                    {
                        m_options.names = std::move(names);
                        m_options.mutually_exclusive_group = group;
                    }

                    ~ArgumentBuilder()
                    {
                        if ((m_options.action == argparse::version) && m_options.help.empty())
                        {
                            m_options.help = "show program's version number and exit";
                        }

                        if (is_positional())
                        {
                            m_arguments.push_back(PositionalArgument(std::move(m_options)));
                        }
                        else
                        {
                            m_arguments.push_back(OptionalArgument(std::move(m_options)));
                        }
                    }

                    auto help(std::string help) -> ArgumentBuilder &
                    {
                        m_options.help = std::move(help);
                        return *this;
                    }

                    auto version(std::string version) -> ArgumentBuilder &
                    {
                        m_version = std::move(version);
                        return *this;
                    }

                    auto metavar(std::string metavar) -> ArgumentBuilder &
                    {
                        m_options.metavar = std::move(metavar);
                        return *this;
                    }

                    auto dest(std::string dest) -> ArgumentBuilder &
                    {
                        if (is_positional())
                        {
                            throw option_error("'dest' is an invalid argument for positionals");
                        }
                        m_options.dest = std::move(dest);
                        return *this;
                    }

                    auto action(Action action) -> ArgumentBuilder &
                    {
                        m_options.action = action;
                        return *this;
                    }

                    auto const_(std::any const_) -> ArgumentBuilder &
                    {
                        m_options.const_ = std::move(const_);
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

                    auto default_(std::any default_) -> ArgumentBuilder &
                    {
                        m_options.default_ = std::move(default_);
                        return *this;
                    }

                    auto required(bool required) -> ArgumentBuilder &
                    {
                        if (is_positional())
                        {
                            throw option_error("'required' is an invalid argument for positionals");
                        }
                        m_options.required = required;
                        return *this;
                    }

                    auto choices(std::vector<std::any> choices) -> ArgumentBuilder &
                    {
                        m_options.choices = std::move(choices);
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
                    Arguments & m_arguments;
                    OptString & m_version;
                    Options m_options;
            };

            static auto cast_to_argument(ArgumentVariant & av) -> Argument &
            {
                return std::visit([](auto & argument) -> Argument & { return argument; } , av);
            }

            static auto cast_to_formattable(ArgumentVariant const & av) -> Formattable const &
            {
                return std::visit([](auto & argument) -> Formattable const & { return argument; } , av);
            }

            Arguments m_arguments;
            OptString m_prog;
            OptString m_usage;
            OptString m_description;
            OptString m_epilog;
            OptString m_version;
            Handle m_handle = Handle::errors_help_version;
    };
}
