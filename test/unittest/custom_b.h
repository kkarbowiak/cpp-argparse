#ifndef CUSTOMB_H
#define CUSTOMB_H

#include "argparse.hpp"
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
class Converter<bar::Custom>
{
    public:
        auto from_string(std::string const & s, bar::Custom & t) const -> bool
        {
            t = bar::Custom(s);
            return true;
        }

        auto to_string(bar::Custom const & t) const -> std::string
        {
            return "<Custom: " + t.m_text + ">";
        }

        auto are_equal(bar::Custom const & lhs, bar::Custom const & rhs) const -> bool
        {
            return lhs.m_text == rhs.m_text;
        }
};
}

#endif /* CUSTOMB_H */
