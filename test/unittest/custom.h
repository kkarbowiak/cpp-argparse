#ifndef CUSTOM_H
#define CUSTOM_H

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

            auto operator==(Custom const & other) const -> bool
            {
                return m_text == other.m_text;
            }

        public:
            std::string m_text;
    };
}

namespace foo
{
inline auto from_string(std::string const & s, Custom & c) -> void
{
    c = Custom(s);
}

inline auto to_string(Custom const& c) -> std::string
{
    return "<Custom: " + c.m_text + ">";
}
}

#endif /* CUSTOM_H */
