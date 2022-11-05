#include "argparse.h"
#include <string>
#include <sstream>
#include <cmath>

namespace geometry
{
    struct Point
    {
        Point() : x(0), y(0)
        {
        }

        Point(int x, int y) : x(x), y(y)
        {
        }

        int x;
        int y;
    };
}

namespace argparse
{
template<>
inline auto from_string(std::string const & s, geometry::Point & p) -> bool
{
    std::istringstream iss(s);
    char comma;
    iss >> p.x >> comma >> p.y;
    return !iss.fail();
}

template<>
inline auto to_string(geometry::Point const & p) -> std::string
{
    return std::to_string(p.x) + "," + std::to_string(p.y);
}

template<>
inline auto are_equal(geometry::Point const & l, geometry::Point const & r) -> bool
{
    return l.x == r.x && l.y == r.y;
}
}

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("start").type<geometry::Point>();
    parser.add_argument("end").type<geometry::Point>();
    auto parsed = parser.parse_args(argc, argv);
    auto start = parsed.get_value<geometry::Point>("start");
    auto end = parsed.get_value<geometry::Point>("end");
    auto distance = std::hypot(end.x - start.x, end.y - start.y);
    std::cout << "The distance is " << distance << '\n';
}
