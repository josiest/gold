#pragma once

#include <tl/expected.hpp>
#include <string>

namespace au {

template<typename expected_t>
using result = tl::expected<expected_t, std::string>;
}
