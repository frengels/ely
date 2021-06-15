#pragma once

#include <cstdint>
#include <span>
#include <string>

#include "ely/vector.hpp"
#include "ely/scanner.hpp"
#include "ely/variant.hpp"

namespace ely
{

namespace atmosphere
{
class Whitespace
{
private:
    std::size_t len{1};

public:
    Whitespace() = default;

    explicit constexpr Whitespace(std::size_t len) : len(len)
    {
        ELY_ASSERT(len != 0, "whitespace must be at least 1 in length");
    }

    template<typename I>
    constexpr Whitespace([[maybe_unused]] Lexeme<I> lex)
        : Whitespace(lex.size())
    {
        ELY_ASSERT(lex.kind == LexemeKind::Whitespace, "expected Whitespace");
    }

    constexpr std::size_t size() const
    {
        return len;
    }
};

class Tab
{
private:
    std::size_t len{1};

public:
    Tab() = default;

    explicit constexpr Tab(std::size_t len) : len(len)
    {
        ELY_ASSERT(len != 0, "tab must be at least 1 in length");
    }

    template<typename I>
    explicit constexpr Tab(Lexeme<I> lex) : len(lex.size())
    {
        ELY_ASSERT(lex.kind == LexemeKind::Tab, "expected Tab");
    }

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
    explicit constexpr NewlineCr([[maybe_unused]] Lexeme<I> lex)
    {
        ELY_ASSERT(lex.kind == LexemeKind::NewlineCr, "expected NewlineCr");
    }

    static constexpr std::string_view str() noexcept
    {
        return std::string_view{"\r"};
    }

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
    explicit constexpr NewlineLf([[maybe_unused]] Lexeme<I> lex)
    {
        ELY_ASSERT(lex.kind == LexemeKind::NewlineLf, "expected NewlineLf");
    }

    static constexpr std::string_view str() noexcept
    {
        return std::string_view{"\n"};
    }

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
    explicit constexpr NewlineCrlf([[maybe_unused]] Lexeme<I> lex)
    {
        ELY_ASSERT(lex.kind == LexemeKind::NewlineCrlf, "expected NewlineCrlf");
    }

    static constexpr std::string_view str() noexcept
    {
        return std::string_view{"\r\n"};
    }

    static constexpr std::size_t size()
    {
        return 2;
    }
};

class Comment
{
private:
    std::string str_;

public:
    Comment() = default;

    template<typename I>
    explicit constexpr Comment(Lexeme<I> lex) : str_(lex.begin(), lex.end())
    {
        ELY_ASSERT(lex.kind == LexemeKind::Comment, "expected Comment");
    }

    template<typename... Args>
    constexpr Comment(std::in_place_t, Args&&... args)
        : str_(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::string_view str() const
    {
        return static_cast<std::string_view>(str_);
    }

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return str_.size();
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
        : variant_([](Lexeme<I> lex) -> VariantType {
              ELY_ASSERT(ely::lexeme_is_atmosphere(lex.kind),
                         "Atmosphere must be made from atmosphere");

              switch (lex.kind)
              {
              case LexemeKind::Whitespace:
                  return atmosphere::Whitespace(lex);
              case LexemeKind::Tab:
                  return atmosphere::Tab(lex);
              case LexemeKind::NewlineCr:
                  return atmosphere::NewlineCr(lex);
              case LexemeKind::NewlineLf:
                  return atmosphere::NewlineLf(lex);
              case LexemeKind::NewlineCrlf:
                  return atmosphere::NewlineCrlf(lex);
              case LexemeKind::Comment:
                  return atmosphere::Comment(lex);
              default:
                  // this condition has been checked by the assert above
                  __builtin_unreachable();
              }
          }(lexeme))
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

enum class AtmospherePosition
{
    Leading,
    Trailing,
};

template<AtmospherePosition Pos>
class AtmosphereList
{
public:
    static constexpr auto position = Pos;

private:
    ely::Vector<Atmosphere> list_;
    std::size_t             size_;

public:
    AtmosphereList() = default;

    template<typename I>
    bool try_emplace_back(Lexeme<I> lex)
    {
        constexpr auto check_atmosphere = []() {
            if constexpr (position == AtmospherePosition::Leading)
            {
                return [](LexemeKind kind) {
                    return lexeme_is_leading_atmosphere(kind);
                };
            }
            else if constexpr (position == AtmospherePosition::Trailing)
            {
                return [](LexemeKind kind) {
                    return lexeme_is_trailing_atmosphere(kind);
                };
            }
        }();

        if (check_atmosphere(lex.kind))
        {
            list_.emplace_back(lex);
            size_ += lex.size();
            return true;
        }

        return false;
    }

    ELY_CONSTEXPR_VECTOR std::span<const Atmosphere> elements() const
    {
        return {list_.begin(), list_.end()};
    }

    constexpr std::size_t size() const
    {
        return size_;
    }
};
} // namespace ely