#pragma once

#include <cstdint>

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

} // namespace ely