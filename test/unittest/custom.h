#ifndef CUSTOM_H
#define CUSTOM_H

#include "argparse.hpp"
#include <string>


namespace foo
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
class Converter<foo::Custom>
{
    public:
        auto from_string(std::string const & s, foo::Custom & t) const -> bool
        {
            t = foo::Custom(s);
            return true;
        }

        auto to_string(foo::Custom const & t) const -> std::string
        {
            return "<Custom: " + t.m_text + ">";
        }

        auto are_equal(foo::Custom const & lhs, foo::Custom const & rhs) const -> bool
        {
            return lhs.m_text == rhs.m_text;
        }
};
}

#endif /* CUSTOM_H */
