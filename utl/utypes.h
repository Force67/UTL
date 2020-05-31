#pragma once

// UTL: The universal utility library
// Copyright 2019-2020 Force67.

#include <cstdint>
#include <memory>

#include "stl_downlevel.h"

namespace utl {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using f32 = float;
using f64 = double;

union alignas(2) f16 {
    u16 _u16;
    u8 _u8[2];

    explicit f16(u16 raw) : _u16(raw) {}

    explicit operator f32() const {
        // See http://stackoverflow.com/a/26779139
        // The conversion doesn't handle NaN/Inf
        u32 raw = ((_u16 & 0x8000) << 16) |             // Sign (just moved)
                  (((_u16 & 0x7c00) + 0x1C000) << 13) | // Exponent ( exp - 15 + 127)
                  ((_u16 & 0x03FF) << 13);              // Mantissa

        return std::bit_cast<f32>(raw);
    }
};

// this shouldnd exist; however i hate how std ptrs look
template <class T>
using SharedPtr = std::shared_ptr<T>;
template <class T>
using UniquePtr = std::unique_ptr<T>;
// template <class T> using MakeShared = std::make_shared<T>;
// template <class T> using MakeUnique = std::make_unique<T>;
}

#ifndef UTL_DISABLE_GLOBAL_TYPES

using f16 = utl::f16;
using f32 = utl::f32;
using f64 = utl::f64;
using i64 = utl::i64;
using u64 = utl::u64;
using i32 = utl::i32;
using u32 = utl::u32;
using i16 = utl::i16;
using u16 = utl::u16;
using i8 = utl::i8;
using u8 = utl::u8;

#endif