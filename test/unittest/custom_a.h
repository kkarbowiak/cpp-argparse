#ifndef CUSTOMA_H
#define CUSTOMA_H

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

namespace foo
{
inline auto operator==(Custom const & lhs, Custom const & rhs) -> bool
{
    return lhs.m_text == rhs.m_text;
}

inline auto from_string(std::string const & s, Custom & c) -> bool
{
    c = Custom(s);
    return true;
}

inline auto to_string(Custom const& c) -> std::string
{
    return "<Custom: " + c.m_text + ">";
}
}

#endif /* CUSTOMA_H */
