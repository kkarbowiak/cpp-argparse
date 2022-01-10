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

        public:
            std::string m_text;
    };
}

namespace argparse
{
template<>
inline auto from_string(std::string const & s, bar::Custom & c) -> bool
{
    c = bar::Custom(s);
    return true;
}

template<>
inline auto to_string(bar::Custom const & t) -> std::string
{
    return "<Custom: " + t.m_text + ">";
}

template<>
inline auto are_equal(bar::Custom const & lhs, bar::Custom const & rhs) -> bool
{
    return lhs.m_text == rhs.m_text;
}
}

#endif /* CUSTOMB_H */
