#pragma once

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

// gets used for all identifier, int, float, string, and char lits
struct StringLit
{
    uint32_t len;
    uint32_t idx;
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

    ely::stx::Position end_pos_;

public:
    Tape() = default;

    template<typename Stream>
    void append_stream(Stream& stream)
    {}

    template<typename I>
    void emplace_back_string(I first, I last)
    {
        TapeElement te;

        std::size_t offset = string_pool_.size();
        string_pool_.append(first, last);

        std::size_t len = string_pool_.size() - offset;

        te.kind_    = TapeKind::StringLit;
        te.str_.len = static_cast<uint32_t>(len);
        te.str_.idx = static_cast<uint32_t>(string_pool_.size());

        tape_elements_.emplace_back(te);
    }

    void emplace_back_prefix_abbrev(lexeme::PrefixAbbrev kind)
    {
        TapeElement te;
        te.kind_ = prefix_abbrev_to_tape(kind);

        tape_elements_.emplace_back(te);
    }
};
} // namespace tape
} // namespace ely