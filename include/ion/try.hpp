#pragma once
#include <expected>

// TRY macro acts like Rust's try operator `?`
#define TRY(expr, ...)                                      \
    ({                                                      \
        auto && ref = (expr);                               \
        if (not ref) {                                      \
            __VA_ARGS__;                                    \
            return std::unexpected(ref.error());            \
        }                                                   \
        *ref;                                               \
    })

#define TRY_WHAT(expr, ...)                                 \
    ({                                                      \
        auto && ref = (expr);                               \
        if (not ref) {                                      \
            __VA_ARGS__;                                    \
            return std::unexpected(ref.error().what());     \
        }                                                   \
        *ref;                                               \
    })

#define TRY_VOID(expr, ...)                                 \
    ({                                                      \
        auto && ref = (expr);                               \
        if (not ref) {                                      \
            __VA_ARGS__;                                    \
            return std::unexpected(ref.error());            \
        }                                                   \
        ;                                                   \
    })
