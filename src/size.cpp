#include "gold/size.hpp"

float gold::sq_dist(gold::size const & lhs, gold::size const & rhs)
{
    auto const dw = lhs.width - rhs.width;
    auto const dh = lhs.height - rhs.height;
    return dw*dw + dh*dh;
}
YAML::Node YAML::convert<gold::size>::encode(const gold::size & size)
{
    if (std::abs(size.width - size.height) >= .1f) {
        YAML::Node node;
        node.push_back(size.width);
        node.push_back(size.height);
        return node;
    }
    else {
        return YAML::Node{ size.width };
    }
}