#pragma once

#include <algorithm>
#include <optional>
#include <string>

#include "ely/stx/location.hpp"
#include "ely/union.hpp"
#include "ely/vector.hpp"

namespace ely
{
namespace tape
{
enum struct AtmosphereKind : uint8_t
{
    Whitespace,
    Tab,
    Comment,
    NewlineCr, // these newlines can be compressed into one tape element
    NewlineLf,
    NewlineCrlf,
};

struct Whitespace
{
    uint32_t len;
};

struct Tab
{
    uint32_t len;
};
struct Comment
{
    uint32_t len;
    uint32_t idx;
};

struct Atmosphere
{
    AtmosphereKind kind_;

    union
    {
        char       empty_;
        Whitespace ws_;
        Tab        tab_;
        Comment    comment_;
    };
};

enum struct TapeKind : uint8_t
{
    ListStart,
    TupleStart,
    AttributesStart,

    ListEnd,
    TupleEnd,
    AttributesEnd,

    MissingListEnd,
    MissingTupleEnd,
    MissingAttributesEnd,

    IntLit, // TODO: split this into shorter variants to save string space and
            // improve performance
    FloatLit,
    StringLit,
    CharLit,
    BoolLit,
    // the identifier should be interned to reduce memory
    // consumption from duplicates
    Identifier,
    // var is followed by the lefthand element and then righthand
    // element
    // offsets will have to be calculated for getting the correct atmosphere
    // span, id<:>ty
    // Both sides can be arbitrary sexps such as !&x:(signed u32)
    Var,
    MissingVarId, // <id>:ty
    MissingVarTy, // id:<ty>
    Quote,
    SyntaxQuote,
    At,
    Unquote,
    SyntaxUnquote,
    UnquoteSplicing,
    SyntaxUnquoteSplicing,
    Exclamation,
    Question,
    Ampersand,
    QuasiQuote,
    QuasiSyntax,
    UnterminatedStringLit,
    InvalidNumberSign,
};

ELY_ALWAYS_INLINE constexpr TapeKind
prefix_abbrev_to_tape(lexeme::PrefixAbbrev kind) noexcept
{
    switch (kind)
    {
    case lexeme::PrefixAbbrev::Quote:
        return TapeKind::Quote;
    case lexeme::PrefixAbbrev::SyntaxQuote:
        return TapeKind::SyntaxQuote;
    case lexeme::PrefixAbbrev::At:
        return TapeKind::At;
    case lexeme::PrefixAbbrev::Unquote:
        return TapeKind::Unquote;
    case lexeme::PrefixAbbrev::SyntaxUnquote:
        return TapeKind::SyntaxUnquote;
    case lexeme::PrefixAbbrev::UnquoteSplicing:
        return TapeKind::UnquoteSplicing;
    case lexeme::PrefixAbbrev::SyntaxUnquoteSplicing:
        return TapeKind::SyntaxUnquoteSplicing;
    case lexeme::PrefixAbbrev::Exclamation:
        return TapeKind::Exclamation;
    case lexeme::PrefixAbbrev::Question:
        return TapeKind::Question;
    case lexeme::PrefixAbbrev::Ampersand:
        return TapeKind::Ampersand;
    case lexeme::PrefixAbbrev::QuasiQuote:
        return TapeKind::QuasiQuote;
    case lexeme::PrefixAbbrev::QuasiSyntax:
        return TapeKind::QuasiSyntax;
    default:
        __builtin_unreachable();
    }
}

class StringHandle
{
private:
    uint32_t len_;
    uint32_t id_;

public:
    StringHandle() = default;

    constexpr StringHandle(uint32_t id, uint32_t len) noexcept
        : len_(len), id_(id)
    {}

    constexpr uint32_t id() const noexcept
    {
        return id_;
    }

    constexpr uint32_t size() const noexcept
    {
        return len_;
    }
};

// gets used for all identifier, int, float, string, and char lits
struct StringLit
{
    StringHandle handle;
};

using Identifier = StringLit;
using IntLit     = StringLit;
using FloatLit   = StringLit;
using CharLit    = StringLit;

struct BoolLit
{
    bool value;
};

struct ListStart
{
    uint32_t end_offset;
};

struct ListEnd
{
    uint32_t start_offset;
};

struct TapeElement
{
    TapeKind kind_;
    union
    {
        char       empty_;
        StringLit  str_;
        Identifier id_;
        IntLit     int_;
        FloatLit   float_;
        CharLit    char_;
        BoolLit    bool_;

        ListStart list_start_;
        ListEnd   list_end_;
    };
};

// TODO, we can bring this down to 8 bytes using some UB
static_assert(sizeof(TapeElement) == 12,
              "Tape elements cannot exceed 12 bytes in size");

class Tape
{
private:
    // this reallocates too much most likely
    ely::Vector<TapeElement> tape_elements_;
    // TODO: this isn't really a string pool currently
    std::string             string_pool_;
    ely::Vector<Atmosphere> atmosphere_pool_;
    ely::Vector<uint32_t>   newline_offsets_;

    ely::stx::Position end_pos_;

public:
    Tape() = default;

    template<typename InputIt>
    inline StringHandle emplace_string(InputIt first, InputIt last)
    {
        uint32_t id = static_cast<uint32_t>(string_pool_.size());
        string_pool_.append(first, last);

        uint32_t len = static_cast<uint32_t>(string_pool_.size()) - id;

        return {id, len};
    }

    template<typename... Args>
    inline StringHandle emplace_string(Args&&... args)
    {
        uint32_t id = static_cast<uint32_t>(string_pool_.size());
        string_pool_.append(static_cast<Args&&>(args)...);

        uint32_t len = static_cast<uint32_t>(string_pool_.size()) - id;

        return {id, len};
    }

    constexpr uint32_t string_pool_size() const
    {
        return static_cast<uint32_t>(string_pool_.size());
    }

    constexpr uint32_t atmosphere_pool_size() const
    {
        return static_cast<uint32_t>(atmosphere_pool_.size());
    }

    template<typename... Args>
    inline decltype(auto) emplace_back(Args&&... args)
    {
        return tape_elements_.emplace_back(static_cast<Args&&>(args)...);
    }

    std::optional<uint32_t> offset_at_line(uint32_t line) const
    {
        if (line <= 1)
        {
            return 0;
        }
        else if ((line + 2) >= newline_offsets_.size())
        {
            return std::nullopt;
        }
        else
        {
            // negate by one since line 0 doesn't exist so we don't store its
            // offset.
            // we do however store offset for line 1, which is always 0
            return newline_offsets_[line - 2];
        }
    }

    std::optional<uint32_t> line_at_offset(uint32_t offset) const
    {
        // this iterator is the one above the one we want *higher_it > offset
        auto higher_it = std::upper_bound(
            newline_offsets_.begin(), newline_offsets_.end(), offset);

        if (higher_it == newline_offsets_.end())
        {
            return std::nullopt;
        }
        else
        {
            return std::distance(newline_offsets_.begin(), higher_it) + 2;
        }
    }
};
} // namespace tape

/// reads from token stream into a tape
template<typename Stream>
class TapeReader
{
private:
    Stream tok_stream_;

public:
    TapeReader() = default;

    constexpr tape::Tape read()
    {
        auto tape = tape::Tape{};

        auto tok = tok_stream_.next();
    }
};
} // namespace ely