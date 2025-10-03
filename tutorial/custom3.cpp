#include "argparse.hpp"
#include <string>
#include <sstream>
#include <cmath>

namespace geometry
{
    struct Point
    {
        Point() = default;

        Point(int xx, int yy) : x(xx), y(yy)
        {
        }

        int x = 0;
        int y = 0;
    };
}

namespace argparse
{
template<>
class Converter<geometry::Point>
{
    public:
        auto from_string(std::string const & s, geometry::Point & p) const -> bool
        {
            std::istringstream iss(s);
            char comma;
            iss >> p.x >> comma >> p.y;
            return !iss.fail();
        }

        auto to_string(geometry::Point const & p) const -> std::string
        {
            return std::to_string(p.x) + "," + std::to_string(p.y);
        }

        auto are_equal(geometry::Point const & l, geometry::Point const & r) const -> bool
        {
            return l.x == r.x && l.y == r.y;
        }
};
}

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("start").type<geometry::Point>().choices({geometry::Point(0, 0), geometry::Point(1, 1), geometry::Point(2, 2)});
    parser.add_argument("end").type<geometry::Point>();
    auto parsed = parser.parse_args(argc, argv);
    auto start = parsed.get_value<geometry::Point>("start");
    auto end = parsed.get_value<geometry::Point>("end");
    auto distance = std::hypot(end.x - start.x, end.y - start.y);
    std::cout << "The distance is " << distance << '\n';
}
