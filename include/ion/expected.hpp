#pragma once
#include <expected>
#include <vector>
#include <utility>

// type constraints
#include <ranges>
#include <iterator>

namespace ion {

template<std::ranges::range Range,
         std::weakly_incrementable OutputIterator,
         std::weakly_incrementable ErrorIterator>

requires std::indirectly_writable<
            OutputIterator,
            typename std::ranges::range_value_t<Range>::value_type>
     and std::indirectly_writable<
            ErrorIterator,
            typename std::ranges::range_value_t<Range>::error_type>

std::pair<OutputIterator, ErrorIterator>
expected_partition(Range && results, OutputIterator output,
                                     ErrorIterator errors)
{
    for (auto && result : results) {
        if (not result) {
            *errors++ = std::move(result).error();
        }
        else {
            *output++ = *std::move(result);
        }
    }
    return std::make_pair(output, errors);
}
}
