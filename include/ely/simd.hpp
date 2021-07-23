#pragma once

#include <cstdint>
#include <initializer_list>

#if __has_include(<xmmintrin.h>)
#include <xmmintrin.h>
#endif

#if __has_include(<emmintrin.h>)
#include <emmintrin.h>
#endif

#if __has_include(<avxintrin.h>)
#include <avxintrin.h>
#endif

#if __has_include(<immintrin.h>)
#include <immintrin.h>
#endif

#include "ely/defines.h"

namespace ely
{
namespace sse
{
class f32x4
{
public:
    using value_type = float;

private:
    __m128 val_;

public:
    f32x4() = default;

    ELY_ALWAYS_INLINE f32x4(__m128 underlying) : val_(underlying)
    {}

    ELY_ALWAYS_INLINE f32x4(float f0, float f1, float f2, float f3)
        : val_(_mm_set_ps(f0, f1, f2, f3))
    {}

    ELY_ALWAYS_INLINE f32x4(float broadcast) : val_(_mm_set_ps1(broadcast))
    {}

    friend ELY_ALWAYS_INLINE f32x4 operator+(f32x4 lhs, f32x4 rhs) noexcept
    {
        return _mm_add_ps(lhs.val_, rhs.val_);
    }
};

class b8x16
{
public:
    using value_type = bool;

private:
    __m128i val_;

public:
    b8x16() = default;

    ELY_ALWAYS_INLINE explicit b8x16(__m128i val) noexcept : val_(val)
    {}

    ELY_ALWAYS_INLINE explicit b8x16(bool b) noexcept
        : val_(_mm_set1_epi8(-static_cast<uint8_t>(b)))
    {}

    ELY_ALWAYS_INLINE uint16_t as_bitmask() const noexcept
    {
        return static_cast<uint16_t>(_mm_movemask_epi8(val_));
    }

    ELY_ALWAYS_INLINE bool any() const noexcept
    {
        return !_mm_testz_si128(val_, val_);
    }

    static constexpr std::size_t size() noexcept
    {
        return 16;
    }
};

class i8x16
{
public:
    using value_type = int8_t;

private:
    __m128i val_;

public:
    i8x16() = default;

    ELY_ALWAYS_INLINE i8x16(int8_t i0,
                            int8_t i1,
                            int8_t i2,
                            int8_t i3,
                            int8_t i4,
                            int8_t i5,
                            int8_t i6,
                            int8_t i7,
                            int8_t i8,
                            int8_t i9,
                            int8_t i10,
                            int8_t i11,
                            int8_t i12,
                            int8_t i13,
                            int8_t i14,
                            int8_t i15)
        : val_(_mm_set_epi8(i0,
                            i1,
                            i2,
                            i3,
                            i4,
                            i5,
                            i6,
                            i7,
                            i8,
                            i9,
                            i10,
                            i11,
                            i12,
                            i13,
                            i14,
                            i15))
    {}

    ELY_ALWAYS_INLINE i8x16(__m128i val) noexcept : val_(val)
    {}

    ELY_ALWAYS_INLINE i8x16(int8_t broadcast) noexcept
        : val_(_mm_set1_epi8(broadcast))
    {}

    friend ELY_ALWAYS_INLINE i8x16 operator+(i8x16 lhs, i8x16 rhs) noexcept
    {
        return _mm_add_epi8(lhs.val_, rhs.val_);
    }

    friend ELY_ALWAYS_INLINE i8x16 operator-(i8x16 lhs, i8x16 rhs) noexcept
    {
        return _mm_sub_epi8(lhs.val_, rhs.val_);
    }

    friend ELY_ALWAYS_INLINE b8x16 operator==(i8x16 lhs, i8x16 rhs) noexcept
    {
        return b8x16{_mm_cmpeq_epi8(lhs.val_, rhs.val_)};
    }

    friend ELY_ALWAYS_INLINE b8x16 operator<(i8x16 lhs, i8x16 rhs) noexcept
    {
        return b8x16{_mm_cmplt_epi8(lhs.val_, rhs.val_)};
    }

    friend ELY_ALWAYS_INLINE b8x16 operator>(i8x16 lhs, i8x16 rhs) noexcept
    {
        return b8x16{_mm_cmpgt_epi8(lhs.val_, rhs.val_)};
    }

    static constexpr std::size_t size() noexcept
    {
        return 16;
    }
};
} // namespace sse

class b8x64
{
public:
    using value_type = bool;

private:
    sse::b8x16 v0_;
    sse::b8x16 v1_;
    sse::b8x16 v2_;
    sse::b8x16 v3_;

public:
    b8x64() = default;
};

class i8x64
{
public:
    using value_type = int8_t;

private:
    sse::i8x16 v0_;
    sse::i8x16 v1_;
    sse::i8x16 v2_;
    sse::i8x16 v3_;

public:
    i8x64() = default;

    ELY_ALWAYS_INLINE
    i8x64(sse::i8x16 v0, sse::i8x16 v1, sse::i8x16 v2, sse::i8x16 v3)
        : v0_(v0), v1_(v1), v2_(v2), v3_(v3)
    {}

    friend ELY_ALWAYS_INLINE b8x64 operator==(i8x64 lhs, i8x64 rhs) noexcept
    {
        return b8x64{lhs.v0_ == rhs.v0_,
                     lhs.v1_ == rhs.v1_,
                     lhs.v2_ == rhs.v2_,
                     lhs.v3_ == rhs.v3_};
    }

    friend ELY_ALWAYS_INLINE b8x64 operator<(i8x64 lhs, i8x64 rhs) noexcept
    {
        return b8x64{lhs.v0_ < rhs.v0_,
                     lhs.v1_ < rhs.v1_,
                     lhs.v2_ < rhs.v2_,
                     lhs.v3_ < rhs.v3_};
    }

    friend ELY_ALWAYS_INLINE b8x64 operator>(i8x64 lhs, i8x64 rhs) noexcept
    {
        return b8x64{lhs.v0_ > rhs.v0_,
                     lhs.v1_ > rhs.v1_,
                     lhs.v2_ > rhs.v2_,
                     lhs.v3_ > rhs.v3_};
    }
};
} // namespace ely