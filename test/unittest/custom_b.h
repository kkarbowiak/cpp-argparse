#ifndef CUSTOMB_H
#define CUSTOMB_H

#include <string>


namespace bar
{
    class Custom
    {
        public:
            Custom() = default;
            explicit Custom(std::string const & text)
                : m_text(text)
            {
            }

            auto operator==(Custom const & other) const -> bool
            {
                return m_text == other.m_text;
            }

        public:
            std::string m_text;
    };
}

namespace argparse
{
template<>
inline auto from_string<bar::Custom>(std::string const & s, bar::Custom & c) -> bool
{
    c = bar::Custom(s);
    return true;
}

template<>
inline auto to_string<bar::Custom>(bar::Custom const & t) -> std::string
{
    return "<Custom: " + t.m_text + ">";
}

template<>
inline auto are_equal<bar::Custom>(bar::Custom const & lhs, bar::Custom const & rhs) -> bool
{
    return lhs == rhs;
}
}

#endif /* CUSTOMB_H */
