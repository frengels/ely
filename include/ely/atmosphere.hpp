#pragma once

#include <cstdint>
#include <string>

#include "ely/scanner.hpp"
#include "ely/variant.hpp"

namespace ely
{

namespace atmosphere
{
class Whitespace
{
private:
    std::size_t len;

public:
    explicit constexpr Whitespace(std::size_t len) : len(len)
    {}

    template<typename I>
    constexpr Whitespace([[maybe_unused]] I first, std::size_t len)
        : Whitespace(len)
    {}

    constexpr std::size_t size() const
    {
        return len;
    }
};

class SmallWhitespace
{
private:
    std::uint8_t len;

public:
    explicit constexpr SmallWhitespace(std::uint8_t len) : len(len)
    {}

    template<typename I>
    constexpr SmallWhitespace([[maybe_unused]] I first, std::size_t len)
        : SmallWhitespace(static_cast<std::uint8_t>(len))
    {
        ELY_ASSERT(len <= std::numeric_limits<std::uint8_t>::max(),
                   "to fit in SmallWhitespace the length must be 255 or less");
    }

    constexpr std::size_t size() const
    {
        return static_cast<std::size_t>(len);
    }
};

class Tab
{
private:
    std::size_t len;

public:
    constexpr Tab(std::size_t len) : len(len)
    {}

    template<typename I>
    constexpr Tab([[maybe_unused]] I first, std::size_t len) : Tab(len)
    {}

    constexpr std::size_t size() const
    {
        return len;
    }
};

class SmallTab
{
private:
    std::uint8_t len;

public:
    constexpr SmallTab(std::uint8_t len) : len(len)
    {}

    template<typename I>
    constexpr SmallTab([[maybe_unused]] I first, std::size_t len)
        : SmallTab(static_cast<std::uint8_t>(len))
    {
        ELY_ASSERT(len <= std::numeric_limits<std::uint8_t>::max(),
                   "to fit in SmallTab the length must be 255 or less");
    }

    constexpr std::size_t size() const
    {
        return static_cast<std::size_t>(len);
    }
};

class NewlineCr
{
public:
    NewlineCr() = default;

    template<typename I>
    constexpr NewlineCr([[maybe_unused]] I           first,
                        [[maybe_unused]] std::size_t len)
        : NewlineCr()
    {}

    static constexpr std::size_t size()
    {
        return 1;
    }
};

class NewlineLf
{
public:
    NewlineLf() = default;

    template<typename I>
    constexpr NewlineLf([[maybe_unused]] I           first,
                        [[maybe_unused]] std::size_t len)
        : NewlineLf()
    {}

    static constexpr std::size_t size()
    {
        return 1;
    }
};

class NewlineCrlf
{
public:
    NewlineCrlf() = default;

    template<typename I>
    constexpr NewlineCrlf([[maybe_unused]] I           first,
                          [[maybe_unused]] std::size_t len)
        : NewlineCrlf()
    {}

    static constexpr std::size_t size()
    {
        return 2;
    }
};

class Comment
{
private:
    std::string str;

public:
    Comment() = default;

    template<typename I>
    constexpr Comment(I first, std::size_t len)
        : str(std::next(first), std::next(first, len))
    {}

    template<typename... Args>
    constexpr Comment(std::in_place_t, Args&&... args)
        : str(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return 1 + str.size();
    }
};
} // namespace atmosphere

class Atmosphere
{
private:
    using VariantType = ely::Variant<atmosphere::Whitespace,
                                     atmosphere::Tab,
                                     atmosphere::NewlineCr,
                                     atmosphere::NewlineLf,
                                     atmosphere::NewlineCrlf,
                                     atmosphere::Comment>;

    VariantType variant_;

public:
    template<typename T, typename... Args>
    explicit constexpr Atmosphere(std::in_place_type_t<T>, Args&&... args)
        : variant_(std::in_place_type<T>, static_cast<Args&&>(args)...)
    {}

    template<typename I>
    explicit constexpr Atmosphere(Lexeme<I> lexeme)
        : variant_([&]() -> VariantType {
              ELY_ASSERT(ely::lexeme_is_atmosphere(lexeme.kind),
                         "Atmosphere must be made from atmosphere");

              switch (lexeme.kind)
              {
              case LexemeKind::Whitespace:
                  return atmosphere::Whitespace(lexeme.start, lexeme.size());
              case LexemeKind::Tab:
                  return atmosphere::Tab(lexeme.start, lexeme.size());
              case LexemeKind::NewlineCr:
                  return atmosphere::NewlineCr(lexeme.start, lexeme.size());
              case LexemeKind::NewlineLf:
                  return atmosphere::NewlineLf(lexeme.start, lexeme.size());
              case LexemeKind::NewlineCrlf:
                  return atmosphere::NewlineCrlf(lexeme.start, lexeme.size());
              case LexemeKind::Comment:
                  return atmosphere::Comment(lexeme.start, lexeme.size());
              default:
                  __builtin_unreachable();
              }
          }())
    {}

    template<typename F>
    constexpr auto visit(F&& fn) const& -> decltype(auto)
    {
        return ely::visit(variant_, static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) & -> decltype(auto)
    {
        return ely::visit(variant_, static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) && -> decltype(auto)
    {
        return ely::visit(std::move(variant_), static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) const&& -> decltype(auto)
    {
        return ely::visit(std::move(variant_), static_cast<F&&>(fn));
    }

    constexpr std::size_t size() const
    {
        return visit([](const auto& x) -> std::size_t { return x.size(); });
    }
};

class SmallAtmosphere
{
private:
    using VariantType = ely::Variant<atmosphere::SmallWhitespace,
                                     atmosphere::SmallTab,
                                     atmosphere::NewlineCr,
                                     atmosphere::NewlineLf,
                                     atmosphere::NewlineCrlf>;

private:
    VariantType variant_;

public:
    template<typename T, typename... Args>
    explicit constexpr SmallAtmosphere(std::in_place_type_t<T>, Args&&... args)
        : variant_(std::in_place_type<T>, static_cast<Args&&>(args)...)
    {}

    template<typename I>
    explicit constexpr SmallAtmosphere(Lexeme<I> lexeme)
        : variant_([&]() -> VariantType {
              ELY_ASSERT(ely::lexeme_is_atmosphere(lexeme.kind) &&
                             lexeme.kind != LexemeKind::Comment,
                         "Only non-comment atmosphere is allowed");

              switch (lexeme.kind)
              {
              case LexemeKind::Whitespace:
                  return atmosphere::SmallWhitespace(lexeme.start,
                                                     lexeme.size());
              case LexemeKind::Tab:
                  return atmosphere::SmallTab(lexeme.start, lexeme.size());
              case LexemeKind::NewlineCr:
                  return atmosphere::NewlineCr(lexeme.start, lexeme.size());
              case LexemeKind::NewlineLf:
                  return atmosphere::NewlineLf(lexeme.start, lexeme.size());
              case LexemeKind::NewlineCrlf:
                  return atmosphere::NewlineCrlf(lexeme.start, lexeme.size());
              default:
                  __builtin_unreachable();
              }
          }())
    {}

    template<typename F>
    constexpr auto visit(F&& fn) & -> decltype(auto)
    {
        return ely::visit(variant_, static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) const& -> decltype(auto)
    {
        return ely::visit(variant_, static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) && -> decltype(auto)
    {
        return ely::visit(std::move(variant_), static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) const&& -> decltype(auto)
    {
        return ely::visit(std::move(variant_), static_cast<F&&>(fn));
    }

    constexpr std::size_t size() const
    {
        return visit(
            [](const auto& atmo) -> std::size_t { return atmo.size(); });
    }

    ELY_CONSTEXPR_STRING Atmosphere to_atmosphere() const
    {
        return visit([](const auto& atmo) -> Atmosphere {
            using ty = std::remove_cvref_t<decltype(atmo)>;

            if constexpr (std::is_same_v<ty, atmosphere::SmallWhitespace>)
            {
                return Atmosphere(std::in_place_type<atmosphere::Whitespace>,
                                  atmo.size());
            }
            else if constexpr (std::is_same_v<ty, atmosphere::SmallTab>)
            {
                return Atmosphere(std::in_place_type<atmosphere::Tab>,
                                  atmo.size());
            }
            else
            {
                return Atmosphere(std::in_place_type<ty>, atmo);
            }
        });
    }
};

namespace detail
{
class SmallAtmosphereList
{
private:
    std::vector<SmallAtmosphere> list_;

public:
    SmallAtmosphereList() = default;

    template<typename... Args>
    constexpr void emplace_back(Args&&... args)
    {
        list_.emplace_back(static_cast<Args&&>(args)...);
    }

    ELY_CONSTEXPR_VECTOR std::size_t atmosphere_size() const
    {
        return std::accumulate(
            list_.begin(),
            list_.end(),
            std::size_t{},
            [](std::size_t cur_sz, const auto& atmosphere) -> std::size_t {
                return cur_sz + atmosphere.size();
            });
    }

    ELY_CONSTEXPR_VECTOR std::size_t size() const
    {
        return list_.size();
    }
};

class LargeAtmosphereList
{
private:
    std::vector<Atmosphere> list_;

public:
    LargeAtmosphereList() = default;

    template<typename... Args>
    constexpr void emplace_back(Args&&... args)
    {
        list_.emplace_back(static_cast<Args&&>(args)...);
    }

    ELY_CONSTEXPR_VECTOR std::size_t atmosphere_size() const
    {
        return std::accumulate(
            list_.begin(),
            list_.end(),
            std::size_t{},
            [](std::size_t cur_sz, const auto& atmosphere) -> std::size_t {
                return cur_sz + atmosphere.size();
            });
    }

    ELY_CONSTEXPR_VECTOR std::size_t size() const
    {
        return list_.size();
    }
};
} // namespace detail

class AtmosphereList
{
private:
    std::vector<Atmosphere> list_;

public:
    AtmosphereList() = default;

    template<typename... Args>
    ELY_CONSTEXPR_VECTOR void emplace_back(Args&&... args)
    {
        list_.emplace_back(static_cast<Args&&>(args)...);
    }

    ELY_CONSTEXPR_VECTOR std::size_t atmosphere_size() const
    {
        return std::accumulate(
            list_.begin(),
            list_.end(),
            std::size_t{},
            [](std::size_t cur_sz, const auto& atmosphere) -> std::size_t {
                return cur_sz + atmosphere.size();
            });
    }

    template<typename F>
    constexpr F for_each(F fn) const
    {
        // we don't offer a
        return std::for_each(list_.begin(), list_.end(), std::move(fn));
    }

    ELY_CONSTEXPR_VECTOR std::size_t size() const
    {
        return list_.size();
    }
};
} // namespace ely