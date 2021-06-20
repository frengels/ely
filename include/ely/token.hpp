#pragma once

#include <cstdint>
#include <functional>
#include <numeric>
#include <ranges>
#include <span>
#include <string>
#include <vector>

#include "ely/assert.h"
#include "ely/atmosphere.hpp"
#include "ely/defines.h"
#include "ely/variant.hpp"

namespace ely
{

template<typename T>
concept Poison = requires
{
    typename T::poison_tag;
};

namespace token
{

    class LParen
    {
    public:
        LParen() = default;

        template<typename I>
        explicit constexpr LParen([[maybe_unused]] Lexeme<I> lex)
        {
            ELY_ASSERT(lex.kind == LexemeKind::LParen, "expected LParen");
        }

        static constexpr std::size_t size()
        {
            return 1;
        }
    };

    class RParen
    {
    public:
        RParen() = default;

        template<typename I>
        explicit constexpr RParen([[maybe_unused]] Lexeme<I> lex)
        {
            ELY_ASSERT(lex.kind == LexemeKind::RParen, "expected RParen");
        }

        static constexpr std::size_t size()
        {
            return 1;
        }
    };

    class LBracket
    {
    public:
        LBracket() = default;

        template<typename I>
        explicit constexpr LBracket([[maybe_unused]] Lexeme<I> lex)
        {
            ELY_ASSERT(lex.kind == LexemeKind::LBracket, "expected LBracket");
        }

        static constexpr std::size_t size()
        {
            return 1;
        }
    };

    class RBracket
    {
    public:
        RBracket() = default;

        template<typename I>
        explicit constexpr RBracket([[maybe_unused]] Lexeme<I> lex)
        {
            ELY_ASSERT(lex.kind == LexemeKind::RBracket, "expected RBracket");
        }

        static constexpr std::size_t size()
        {
            return 1;
        }
    };

    class LBrace
    {
    public:
        LBrace() = default;

        template<typename I>
        explicit constexpr LBrace([[maybe_unused]] Lexeme<I> lex)
        {
            ELY_ASSERT(lex.kind == LexemeKind::LBrace, "expected LBrace");
        }

        static constexpr std::size_t size()
        {
            return 1;
        }
    };

    class RBrace
    {
    public:
        RBrace() = default;

        template<typename I>
        explicit constexpr RBrace([[maybe_unused]] Lexeme<I> lex)
        {
            ELY_ASSERT(lex.kind == LexemeKind::RBrace, "expected RBrace");
        }

        static constexpr std::size_t size()
        {
            return 1;
        }
    };

    class Identifier
    {
    private:
        std::string name_;

    public:
        Identifier() = default;

        template<typename I>
        explicit constexpr Identifier(Lexeme<I> lex)
            : name_(lex.begin(), lex.end())
        {
            ELY_ASSERT(lex.kind == LexemeKind::Identifier,
                       "expected Identifier");
        }

        template<typename... Args>
        constexpr Identifier(std::in_place_t, Args&&... args)
            : name_(static_cast<Args&&>(args)...)
        {}

        ELY_CONSTEXPR_STRING std::string_view name() const noexcept
        {
            return static_cast<std::string_view>(name_);
        }

        ELY_CONSTEXPR_STRING std::size_t size() const
        {
            return name_.size();
        }
    };

    class IntLit
    {
    private:
        std::string str_;

    public:
        IntLit() = default;

        template<typename I>
        explicit constexpr IntLit(Lexeme<I> lex) : str_(lex.begin(), lex.end())
        {
            ELY_ASSERT(lex.kind == LexemeKind::IntLit, "expected IntLit");
        }

        template<typename... Args>
        constexpr IntLit(std::in_place_t, Args&&... args)
            : str_(static_cast<Args&&>(args)...)
        {}

        ELY_CONSTEXPR_STRING std::string_view str() const noexcept
        {
            return static_cast<std::string_view>(str_);
        }

        ELY_CONSTEXPR_STRING std::size_t size() const
        {
            return str_.size();
        }
    };

    class FloatLit
    {
    private:
        std::string str_;

    public:
        FloatLit() = default;

        template<typename I>
        explicit constexpr FloatLit(Lexeme<I> lex)
            : str_(lex.begin(), lex.end())
        {
            ELY_ASSERT(lex.kind == LexemeKind::FloatLit, "expected FloatLit");
        }

        template<typename... Args>
        constexpr FloatLit(std::in_place_t, Args&&... args)
            : str_(static_cast<Args&&>(args)...)
        {}

        ELY_CONSTEXPR_STRING std::string_view str() const noexcept
        {
            return static_cast<std::string_view>(str_);
        }

        ELY_CONSTEXPR_STRING std::size_t size() const
        {
            return str_.size();
        }
    };

    class CharLit
    {
    private:
        std::string str_;

    public:
        CharLit() = default;

        template<typename I>
        explicit constexpr CharLit(Lexeme<I> lex) : str_(lex.begin(), lex.end())
        {
            ELY_ASSERT(lex.kind == LexemeKind::CharLit, "expected CharLit");
        }

        template<typename... Args>
        constexpr CharLit(std::in_place_t, Args&&... args)
            : str_(static_cast<Args&&>(args)...)
        {}

        ELY_CONSTEXPR_STRING std::string_view str() const noexcept
        {
            return static_cast<std::string_view>(str_);
        }

        ELY_CONSTEXPR_STRING std::size_t size() const
        {
            return str_.size();
        }
    };

    class StringLit
    {
    private:
        std::string str_;

    public:
        StringLit() = default;

        template<typename I>
        constexpr StringLit(Lexeme<I> lex) : str_(lex.begin(), lex.end())
        {
            ELY_ASSERT(lex.kind == LexemeKind::StringLit, "expected StringLit");
        }

        template<typename... Args>
        constexpr StringLit(std::in_place_t, Args&&... args)
            : str_(static_cast<Args&&>(args)...)
        {}

        ELY_CONSTEXPR_STRING std::string_view str() const noexcept
        {
            return static_cast<std::string_view>(str_);
        }

        ELY_CONSTEXPR_STRING std::size_t size() const
        {
            return str_.size();
        }
    };

    class KeywordLit
    {
    private:
        std::string str_;

    public:
        KeywordLit() = default;

        template<typename I>
        explicit constexpr KeywordLit(Lexeme<I> lex)
            : str_(lex.begin(), lex.end())
        {
            ELY_ASSERT(lex.kind == LexemeKind::KeywordLit,
                       "expected KeywordLit");
        }

        template<typename... Args>
        constexpr KeywordLit(std::in_place_t, Args&&... args)
            : str_(static_cast<Args&&>(args)...)
        {}

        ELY_CONSTEXPR_STRING std::string_view str() const noexcept
        {
            return static_cast<std::string_view>(str_);
        }

        ELY_CONSTEXPR_STRING std::size_t size() const
        {
            return str_.size();
        }
    };

    class BoolLit
    {
    private:
        bool b{false};

    public:
        BoolLit() = default;

        // assuming the contents are either '#t' or '#f'
        template<typename I>
        explicit constexpr BoolLit(Lexeme<I> lex)
            : b(*std::next(lex.begin()) == 't')
        {
            ELY_ASSERT(lex.kind == LexemeKind::BoolLit, "expected BoolLit");
        }

        explicit constexpr BoolLit(bool b) : b(b)
        {}

        constexpr bool value() const
        {
            return b;
        }

        constexpr std::string_view str() const
        {
            return b ? std::string_view{"#t"} : std::string_view{"#f"};
        }

        static constexpr std::size_t size()
        {
            return 2;
        }
    };

    class UnterminatedStringLit
    {
    public:
        using poison_tag = void;

    private:
        std::string str_;

    public:
        template<typename I>
        explicit constexpr UnterminatedStringLit(Lexeme<I> lex)
            : str_(lex.begin(), lex.end())
        {
            ELY_ASSERT(lex.kind == LexemeKind::StringLit, "expected StringLit");
        }

        ELY_CONSTEXPR_STRING std::string_view str() const noexcept
        {
            return static_cast<std::string_view>(str_);
        }

        ELY_CONSTEXPR_STRING std::size_t size() const noexcept
        {
            return str_.size();
        }
    };

    class InvalidNumberSign
    {
    public:
        using poison_tag = void;

    private:
        std::string str_;

    public:
        template<typename I>
        explicit constexpr InvalidNumberSign(Lexeme<I> lex)
            : str_(lex.begin(), lex.end())
        {
            ELY_ASSERT(lex.kind == LexemeKind::InvalidNumberSign,
                       "expected InvalidNumberSign");
        }

        ELY_CONSTEXPR_STRING std::string_view str() const noexcept
        {
            return static_cast<std::string_view>(str_);
        }

        ELY_CONSTEXPR_STRING std::size_t size() const noexcept
        {
            return str_.size();
        }
    };

    class Eof
    {
    public:
        Eof() = default;

        template<typename I>
        explicit constexpr Eof([[maybe_unused]] Lexeme<I> lex)
        {
            ELY_ASSERT(lex.kind == LexemeKind::Eof, "expected Eof");
        }

        static constexpr std::size_t size()
        {
            return 0;
        }
    };
} // namespace token

template<typename... Toks>
class TokenVariant : public ely::Variant<Toks...>
{
    using base = ely::Variant<Toks...>;

private:
    AtmosphereList<AtmospherePosition::Leading>  leading_;
    AtmosphereList<AtmospherePosition::Trailing> trailing_;

public:
    template<typename T, typename... Args>
    constexpr TokenVariant(
        AtmosphereList<AtmospherePosition::Leading>&&  leading,
        AtmosphereList<AtmospherePosition::Trailing>&& trailing,
        std::in_place_type_t<T>                        t,
        Args&&... args)
        : leading_(std::move(leading)), trailing_(std::move(trailing)),
          base(t, static_cast<Args&&>(args)...)
    {}

    constexpr TokenVariant(
        AtmosphereList<AtmospherePosition::Leading>&&  leading,
        AtmosphereList<AtmospherePosition::Trailing>&& trailing,
        base&&                                         tok)
        : leading_(std::move(leading)), trailing_(std::move(trailing)),
          base(std::move(tok))
    {}

    using base::visit;

    template<typename F>
    constexpr auto visit_all(F&& fn) & -> decltype(auto)
    {
        return visit([&](auto& tok) -> decltype(auto) {
            return std::invoke(static_cast<F&&>(fn), tok, leading_, trailing_);
        });
    }

    template<typename F>
    constexpr auto visit_all(F&& fn) const& -> decltype(auto)
    {
        return visit([&](const auto& tok) -> decltype(auto) {
            return std::invoke(static_cast<F&&>(fn), tok, leading_, trailing_);
        });
    }

    template<typename F>
    constexpr auto visit_all(F&& fn) && -> decltype(auto)
    {
        return std::move(*this).visit([&](auto&& tok) -> decltype(auto) {
            return std::invoke(static_cast<F&&>(fn),
                               std::move(tok),
                               std::move(leading_),
                               std::move(trailing_));
        });
    }

    template<typename F>
    constexpr auto visit_all(F&& fn) const&& -> decltype(auto)
    {
        return std::move(*this).visit([&](const auto&& tok) -> decltype(auto) {
            return std::invoke(static_cast<F&&>(fn),
                               std::move(tok),
                               std::move(leading_),
                               std::move(trailing_));
        });
    }

    constexpr bool is_poison() const noexcept
    {
        return visit([](const auto& tok) {
            using tok_ty = std::remove_cvref_t<decltype(tok)>;
            return ely::Poison<tok_ty>;
        });
    }

    template<typename T>
    friend constexpr bool holds(const TokenVariant& self) noexcept
    {
        return self.visit([](const auto& x) {
            using ty = std::remove_cvref_t<decltype(x)>;
            return std::is_same_v<T, ty>;
        });
    }

    template<typename T>
    friend constexpr T& unsafe_get(TokenVariant& self) noexcept
    {
        return *get_if<T>(&self.variant_);
    }

    template<typename T>
    friend constexpr T&& unsafe_get(TokenVariant&& self) noexcept
    {
        return static_cast<T&&>(*get_if<T>(&self.variant_));
    }

    constexpr const AtmosphereList<AtmospherePosition::Leading>&
    leading_atmosphere() const
    {
        return leading_;
    }

    constexpr const AtmosphereList<AtmospherePosition::Trailing>&
    trailing_atmosphere() const
    {
        return trailing_;
    }

    constexpr std::size_t leading_size() const
    {
        return leading_.size();
    }

    constexpr std::size_t trailing_size() const
    {
        return trailing_.size();
    }

    constexpr std::size_t size() const
    {
        return leading_size() + trailing_size() + inner_size();
    }

    constexpr std::size_t inner_size() const
    {
        return visit([](const auto& tok) -> std::size_t { return tok.size(); });
    }
};

using Token = TokenVariant<token::LParen,
                           token::RParen,
                           token::LBracket,
                           token::RBracket,
                           token::LBrace,
                           token::RBrace,
                           token::Identifier,
                           token::IntLit,
                           token::FloatLit,
                           token::CharLit,
                           token::StringLit,
                           token::KeywordLit,
                           token::BoolLit,
                           token::UnterminatedStringLit,
                           token::InvalidNumberSign,
                           token::Eof>;

template<typename I>
Token make_token(AtmosphereList<AtmospherePosition::Leading>&&  leading,
                 AtmosphereList<AtmospherePosition::Trailing>&& trailing,
                 Lexeme<I>                                      lex)
{

#define DISPATCH(tok)                                                          \
    case LexemeKind::tok:                                                      \
        return Token(std::move(leading),                                       \
                     std::move(trailing),                                      \
                     std::in_place_type<token::tok>,                           \
                     lex);
    switch (lex.kind)
    {
        DISPATCH(LParen);
        DISPATCH(RParen);
        DISPATCH(LBracket);
        DISPATCH(RBracket);
        DISPATCH(LBrace);
        DISPATCH(RBrace);
        DISPATCH(Identifier);
        DISPATCH(IntLit);
        DISPATCH(FloatLit);
        DISPATCH(CharLit);
        DISPATCH(StringLit);
        DISPATCH(KeywordLit);
        DISPATCH(BoolLit);
        DISPATCH(InvalidNumberSign);
        DISPATCH(UnterminatedStringLit);
        DISPATCH(Eof);
    default:
        __builtin_unreachable();
    }
#undef DISPATCH
}
} // namespace ely