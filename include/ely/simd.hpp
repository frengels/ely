#pragma once

#include <cstdint>
#include <initializer_list>

#if __has_include(<xmmintrin.h>)
#include <xmmintrin.h>
#endif

#if __has_include(<emmintrin.h>)
#include <emmintrin.h>
#endif

#if __has_include(<immintrin.h>)
#include <immintrin.h>
#endif

#include <wmmintrin.h>

#include "ely/defines.h"

namespace ely
{
namespace sse
{
class u128
{
protected:
    __m128i val_;

public:
    u128() = default;

    ELY_ALWAYS_INLINE u128(__m128i val) noexcept : val_(val)
    {}

    ELY_ALWAYS_INLINE u128(const __m128i_u* p) noexcept
        : val_(_mm_loadu_si128(p))
    {}

    friend ELY_ALWAYS_INLINE u128 operator&(const u128& lhs,
                                            const u128& rhs) noexcept
    {
        return _mm_and_si128(lhs.val_, rhs.val_);
    }

    friend ELY_ALWAYS_INLINE u128 operator|(const u128& lhs,
                                            const u128& rhs) noexcept
    {
        return _mm_or_si128(lhs.val_, rhs.val_);
    }

    friend ELY_ALWAYS_INLINE u128 operator^(const u128& lhs,
                                            const u128& rhs) noexcept
    {
        return _mm_xor_si128(lhs.val_, rhs.val_);
    }

    ELY_ALWAYS_INLINE u128 and_not(const u128& other) const noexcept
    {
        return _mm_andnot_si128(val_, other.val_);
    }

    ELY_ALWAYS_INLINE u128 operator~() const noexcept
    {
        return *this ^ _mm_set1_epi8(0xff);
    }

    ELY_ALWAYS_INLINE u128 xor_mul(const u128& other) const noexcept
    {
        return _mm_clmulepi64_si128(val_, other.val_, 0);
    }
};

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

class b8x16 : public u128
{
public:
    using value_type = bool;

public:
    using u128::u128;

    ELY_ALWAYS_INLINE explicit b8x16(bool b) noexcept
        : u128(_mm_set1_epi8(-static_cast<uint8_t>(b)))
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

class u8x16 : public u128
{
    using base_ = u128;

public:
    using value_type = uint8_t;

public:
    using base_::base_;

    ELY_ALWAYS_INLINE u8x16(uint8_t i0,
                            uint8_t i1,
                            uint8_t i2,
                            uint8_t i3,
                            uint8_t i4,
                            uint8_t i5,
                            uint8_t i6,
                            uint8_t i7,
                            uint8_t i8,
                            uint8_t i9,
                            uint8_t i10,
                            uint8_t i11,
                            uint8_t i12,
                            uint8_t i13,
                            uint8_t i14,
                            uint8_t i15)
        : base_(_mm_set_epi8(i15,
                             i14,
                             i13,
                             i12,
                             i11,
                             i10,
                             i9,
                             i8,
                             i7,
                             i6,
                             i5,
                             i4,
                             i3,
                             i2,
                             i1,
                             i0))
    {}

    ELY_ALWAYS_INLINE u8x16(uint8_t broadcast) noexcept
        : base_(_mm_set1_epi8(broadcast))
    {}

    static ELY_ALWAYS_INLINE u8x16 loadu(const uint8_t* vals) noexcept
    {
        return _mm_loadu_si128(const_cast<__m128i_u*>(
            reinterpret_cast<const volatile __m128i_u*>(vals)));
    }

    static ELY_ALWAYS_INLINE u8x16 loadu(const char* p) noexcept
    {
        return loadu(reinterpret_cast<const uint8_t*>(p));
    }

    friend ELY_ALWAYS_INLINE u8x16 operator+(u8x16 lhs, u8x16 rhs) noexcept
    {
        return _mm_add_epi8(lhs.val_, rhs.val_);
    }

    friend ELY_ALWAYS_INLINE u8x16 operator-(u8x16 lhs, u8x16 rhs) noexcept
    {
        return _mm_sub_epi8(lhs.val_, rhs.val_);
    }

    friend ELY_ALWAYS_INLINE b8x16 operator==(u8x16 lhs, u8x16 rhs) noexcept
    {
        return b8x16{_mm_cmpeq_epi8(lhs.val_, rhs.val_)};
    }

    friend ELY_ALWAYS_INLINE b8x16 operator<(u8x16 lhs, u8x16 rhs) noexcept
    {
        return b8x16{_mm_cmplt_epi8(lhs.val_, rhs.val_)};
    }

    friend ELY_ALWAYS_INLINE b8x16 operator>(u8x16 lhs, u8x16 rhs) noexcept
    {
        return b8x16{_mm_cmpgt_epi8(lhs.val_, rhs.val_)};
    }

    static constexpr std::size_t size() noexcept
    {
        return 16;
    }
};

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

    ELY_ALWAYS_INLINE
    b8x64(sse::b8x16 v0, sse::b8x16 v1, sse::b8x16 v2, sse::b8x16 v3)
        : v0_(v0), v1_(v1), v2_(v2), v3_(v3)
    {}

    ELY_ALWAYS_INLINE uint64_t as_bitmask() const noexcept
    {
        return (static_cast<uint64_t>(v0_.as_bitmask()) << 0) |
               (static_cast<uint64_t>(v1_.as_bitmask()) << 16) |
               (static_cast<uint64_t>(v2_.as_bitmask()) << 32) |
               (static_cast<uint64_t>(v3_.as_bitmask()) << 48);
    }
};

class u8x64
{
public:
    using value_type = uint8_t;

private:
    sse::u8x16 v0_;
    sse::u8x16 v1_;
    sse::u8x16 v2_;
    sse::u8x16 v3_;

public:
    u8x64() = default;

    ELY_ALWAYS_INLINE
    u8x64(sse::u8x16 v0, sse::u8x16 v1, sse::u8x16 v2, sse::u8x16 v3)
        : v0_(v0), v1_(v1), v2_(v2), v3_(v3)
    {}

    ELY_ALWAYS_INLINE u8x64(uint8_t x) : v0_(x), v1_(x), v2_(x), v3_(x)
    {}

    static ELY_ALWAYS_INLINE u8x64 loadu(const char* p)
    {
        return u8x64{sse::u8x16::loadu(p),
                     sse::u8x16::loadu(p + 16),
                     sse::u8x16::loadu(p + 32),
                     sse::u8x16::loadu(p + 48)};
    }

    friend ELY_ALWAYS_INLINE b8x64 operator==(u8x64 lhs, u8x64 rhs) noexcept
    {
        return b8x64{lhs.v0_ == rhs.v0_,
                     lhs.v1_ == rhs.v1_,
                     lhs.v2_ == rhs.v2_,
                     lhs.v3_ == rhs.v3_};
    }

    friend ELY_ALWAYS_INLINE b8x64 operator<(u8x64 lhs, u8x64 rhs) noexcept
    {
        return b8x64{lhs.v0_ < rhs.v0_,
                     lhs.v1_ < rhs.v1_,
                     lhs.v2_ < rhs.v2_,
                     lhs.v3_ < rhs.v3_};
    }

    friend ELY_ALWAYS_INLINE b8x64 operator>(u8x64 lhs, u8x64 rhs) noexcept
    {
        return b8x64{lhs.v0_ > rhs.v0_,
                     lhs.v1_ > rhs.v1_,
                     lhs.v2_ > rhs.v2_,
                     lhs.v3_ > rhs.v3_};
    }
};
} // namespace sse

namespace avx
{
class u256
{
protected:
    __m256i val_;

public:
    u256() = default;

    ELY_ALWAYS_INLINE u256(__m256i val) noexcept : val_(val)
    {}

    friend ELY_ALWAYS_INLINE u256 operator&(const u256& lhs,
                                            const u256& rhs) noexcept
    {
        return _mm256_and_si256(lhs.val_, rhs.val_);
    }

    friend ELY_ALWAYS_INLINE u256 operator|(const u256& lhs,
                                            const u256& rhs) noexcept
    {
        return _mm256_or_si256(lhs.val_, rhs.val_);
    }
};

class b8x32 : public u256
{
public:
    using u256::u256;

    ELY_ALWAYS_INLINE uint32_t as_bitmask() const noexcept
    {
        return static_cast<uint32_t>(_mm256_movemask_epi8(val_));
    }

    ELY_ALWAYS_INLINE bool any() const noexcept
    {
        return !_mm256_testz_si256(val_, val_);
    }

    static constexpr std::size_t size() noexcept
    {
        return 16;
    }
};

class u8x32 : public u256
{
public:
    using u256::u256;

    explicit ELY_ALWAYS_INLINE u8x32(char broadcast) noexcept
        : u256(_mm256_set1_epi8(broadcast))
    {}

    static ELY_ALWAYS_INLINE u8x32 loadu(const char* p) noexcept
    {
        return _mm256_loadu_si256(reinterpret_cast<const __m256i_u*>(p));
    }

    friend ELY_ALWAYS_INLINE b8x32 operator==(const u8x32& lhs,
                                              const u8x32& rhs) noexcept
    {
        return _mm256_cmpeq_epi8(lhs.val_, rhs.val_);
    }
};

class b8x64
{
public:
    using value_type = bool;

private:
    avx::b8x32 b0_;
    avx::b8x32 b1_;

public:
    b8x64() = default;

    ELY_ALWAYS_INLINE b8x64(avx::b8x32 b0, avx::b8x32 b1) noexcept
        : b0_(b0), b1_(b1)
    {}

    ELY_ALWAYS_INLINE uint64_t as_bitmask() const noexcept
    {
        return (static_cast<uint64_t>(b0_.as_bitmask()) << 0) |
               (static_cast<uint64_t>(b1_.as_bitmask()) << 32);
    }
};

class u8x64
{
public:
    using value_type = uint8_t;

private:
    avx::u8x32 v0_;
    avx::u8x32 v1_;

public:
    u8x64() = default;

    ELY_ALWAYS_INLINE u8x64(avx::u8x32 v0, avx::u8x32 v1) noexcept
        : v0_(v0), v1_(v1)
    {}

    ELY_ALWAYS_INLINE u8x64(char broadcast) noexcept
        : u8x64(avx::u8x32(broadcast), avx::u8x32(broadcast))
    {}

    static ELY_ALWAYS_INLINE u8x64 loadu(const char* p)
    {
        return u8x64{avx::u8x32::loadu(p), avx::u8x32::loadu(p + 32)};
    }

    friend ELY_ALWAYS_INLINE b8x64 operator==(const u8x64& lhs,
                                              const u8x64& rhs) noexcept
    {
        return b8x64{lhs.v0_ == rhs.v0_, lhs.v1_ == rhs.v1_};
    }
};
} // namespace avx

namespace avx512
{
class u512
{
protected:
    __m512i val_;

public:
    u512() = default;

    ELY_ALWAYS_INLINE u512(__m512i val) noexcept : val_(val)
    {}

    friend ELY_ALWAYS_INLINE u512 operator&(const u512& lhs,
                                            const u512& rhs) noexcept
    {
        return _mm512_and_si512(lhs.val_, rhs.val_);
    }

    friend ELY_ALWAYS_INLINE u512 operator|(const u512& lhs,
                                            const u512& rhs) noexcept
    {
        return _mm512_or_si512(lhs.val_, rhs.val_);
    }
};

class fake_b8x64
{
private:
    uint64_t val_;

public:
    ELY_ALWAYS_INLINE constexpr fake_b8x64(uint64_t val) noexcept : val_(val)
    {}

    ELY_ALWAYS_INLINE constexpr uint64_t as_bitmask() const noexcept
    {
        return val_;
    }
};

class u8x64 : public u512
{
public:
    using u512::u512;

    explicit ELY_ALWAYS_INLINE u8x64(char broadcast) noexcept
        : u512(_mm512_set1_epi8(broadcast))
    {}

    static ELY_ALWAYS_INLINE u8x64 loadu(const char* p) noexcept
    {
        return _mm512_loadu_si512(reinterpret_cast<const __m512i_u*>(p));
    }

    friend ELY_ALWAYS_INLINE fake_b8x64 operator==(const u8x64& lhs,
                                                   const u8x64& rhs) noexcept
    {
        return _mm512_cmpeq_epi8_mask(lhs.val_, rhs.val_);
    }
};
} // namespace avx512
} // namespace ely