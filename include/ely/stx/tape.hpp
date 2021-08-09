#pragma once

#include <string>

#include "ely/stx/location.hpp"
#include "ely/union.hpp"
#include "ely/vector.hpp"

namespace ely
{
namespace tape
{
enum struct TapeKind : uint8_t
{
    ListStart,
    TupleStart,
    AttributesStart,

    ListEnd,
    TupleEnd,
    AttributesEnd,

    IntLit, // TODO: split this into shorter variants to save string space and
            // improve performance
    FloatLit,
    StringLit,
    CharLit,
    BoolLit,
    Identifier,
    Var,
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
    std::string string_pool_;

    ely::stx::Position end_pos_;

public:
    Tape() = default;

    template<typename Stream>
    void append_stream(Stream& stream)
    {}

    template<typename I>
    void emplace_back_string(TapeKind kind, I first, I last)
    {
        TapeElement te;

        std::size_t offset = string_pool_.size();
        string_pool_.append(first, last);

        std::size_t len = string_pool_.size() - offset;

        te.kind_    = kind;
        te.str_.len = static_cast<uint32_t>(len);
        te.str_.idx = static_cast<uint32_t>(string_pool_.size());

        tape_elements_.emplace_back(te);
    }

    void emplace_back_abbrev(TapeKind kind)
    {
        TapeElement te;

        te.kind_ = kind;

        tape_elements_.emplace_back(te);
    }
};
} // namespace tape
} // namespace ely