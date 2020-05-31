#pragma once

// UTL: The universal utility library
// Copyright 2019-2020 Force67.

// downlevel support for older cpp versions

#include <cstdio>

// std::bitcast support for pre-cpp20
#if defined(__cpp_lib_bit_cast) && (__cpp_lib_bit_cast >= 201806L)
#include <bit>
#else
namespace std {
template <class To, class From, typename = std::enable_if_t<sizeof(To) == sizeof(From)>>
constexpr To bit_cast(const From& from) noexcept {
    static_assert(sizeof(To) == sizeof(From), "std::bit_cast<>: incompatible type size");

    To result;
    std::memcpy(&result, &from, sizeof(From));
    return result;
}
} // namespace std
#endif
