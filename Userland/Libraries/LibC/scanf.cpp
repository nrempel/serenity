/*
 * Copyright (c) 2021, the SerenityOS developers.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <AK/Assertions.h>
#include <AK/Format.h>
#include <AK/GenericLexer.h>
#include <AK/StdLibExtras.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum LengthModifier {
    None,
    Default,
    Char,
    Short,
    Long,
    LongLong,
    IntMax,
    Size,
    PtrDiff,
    LongDouble,
};

enum ConversionSpecifier {
    Unspecified,
    Decimal,
    Integer,
    Octal,
    Unsigned,
    Hex,
    Floating,
    String,
    UseScanList,
    Character,
    Pointer,
    OutputNumberOfBytes,
    Invalid,
};

enum class ReadKind {
    Normal,
    Octal,
    Hex,
    Infer,
};

template<typename T, typename ApT, ReadKind kind = ReadKind::Normal>
struct ReadElementConcrete {
    bool operator()(GenericLexer&, va_list)
    {
        return false;
    }
};

template<typename ApT, ReadKind kind>
struct ReadElementConcrete<int, ApT, kind> {
    bool operator()(GenericLexer& lexer, va_list* ap)
    {
        lexer.ignore_while(isspace);

        auto* ptr = ap ? va_arg(*ap, ApT*) : nullptr;
        long value = 0;
        char* endptr = nullptr;
        auto nptr = lexer.remaining().characters_without_null_termination();
        if constexpr (kind == ReadKind::Normal)
            value = strtol(nptr, &endptr, 10);
        if constexpr (kind == ReadKind::Octal)
            value = strtol(nptr, &endptr, 8);
        if constexpr (kind == ReadKind::Hex)
            value = strtol(nptr, &endptr, 16);
        if constexpr (kind == ReadKind::Infer)
            value = strtol(nptr, &endptr, 0);

        if (!endptr)
            return false;

        if (endptr == nptr)
            return false;

        auto diff = endptr - nptr;
        VERIFY(diff > 0);
        lexer.ignore((size_t)diff);

        if (ptr)
            *ptr = value;
        return true;
    }
};

template<typename ApT, ReadKind kind>
struct ReadElementConcrete<char, ApT, kind> {
    bool operator()(GenericLexer& lexer, va_list* ap)
    {
        static_assert(kind == ReadKind::Normal, "Can't read a non-normal character");

        auto* ptr = ap ? va_arg(*ap, ApT*) : nullptr;

        if (lexer.is_eof())
            return false;

        auto ch = lexer.consume();
        if (ptr)
            *ptr = ch;
        return true;
    }
};

template<typename ApT, ReadKind kind>
struct ReadElementConcrete<unsigned, ApT, kind> {
    bool operator()(GenericLexer& lexer, va_list* ap)
    {
        lexer.ignore_while(isspace);

        auto* ptr = ap ? va_arg(*ap, ApT*) : nullptr;
        unsigned long value = 0;
        char* endptr = nullptr;
        auto nptr = lexer.remaining().characters_without_null_termination();
        if constexpr (kind == ReadKind::Normal)
            value = strtoul(nptr, &endptr, 10);
        if constexpr (kind == ReadKind::Octal)
            value = strtoul(nptr, &endptr, 8);
        if constexpr (kind == ReadKind::Hex)
            value = strtoul(nptr, &endptr, 16);
        if constexpr (kind == ReadKind::Infer)
            value = strtoul(nptr, &endptr, 0);

        if (!endptr)
            return false;

        if (endptr == nptr)
            return false;

        auto diff = endptr - nptr;
        VERIFY(diff > 0);
        lexer.ignore((size_t)diff);

        if (ptr)
            *ptr = value;
        return true;
    }
};

template<typename ApT, ReadKind kind>
struct ReadElementConcrete<long long, ApT, kind> {
    bool operator()(GenericLexer& lexer, va_list* ap)
    {
        lexer.ignore_while(isspace);

        auto* ptr = ap ? va_arg(*ap, ApT*) : nullptr;
        long long value = 0;
        char* endptr = nullptr;
        auto nptr = lexer.remaining().characters_without_null_termination();
        if constexpr (kind == ReadKind::Normal)
            value = strtoll(nptr, &endptr, 10);
        if constexpr (kind == ReadKind::Octal)
            value = strtoll(nptr, &endptr, 8);
        if constexpr (kind == ReadKind::Hex)
            value = strtoll(nptr, &endptr, 16);
        if constexpr (kind == ReadKind::Infer)
            value = strtoll(nptr, &endptr, 0);

        if (!endptr)
            return false;

        if (endptr == nptr)
            return false;

        auto diff = endptr - nptr;
        VERIFY(diff > 0);
        lexer.ignore((size_t)diff);

        if (ptr)
            *ptr = value;
        return true;
    }
};

template<typename ApT, ReadKind kind>
struct ReadElementConcrete<unsigned long long, ApT, kind> {
    bool operator()(GenericLexer& lexer, va_list* ap)
    {
        lexer.ignore_while(isspace);

        auto* ptr = ap ? va_arg(*ap, ApT*) : nullptr;
        unsigned long long value = 0;
        char* endptr = nullptr;
        auto nptr = lexer.remaining().characters_without_null_termination();
        if constexpr (kind == ReadKind::Normal)
            value = strtoull(nptr, &endptr, 10);
        if constexpr (kind == ReadKind::Octal)
            value = strtoull(nptr, &endptr, 8);
        if constexpr (kind == ReadKind::Hex)
            value = strtoull(nptr, &endptr, 16);
        if constexpr (kind == ReadKind::Infer)
            value = strtoull(nptr, &endptr, 0);

        if (!endptr)
            return false;

        if (endptr == nptr)
            return false;

        auto diff = endptr - nptr;
        VERIFY(diff > 0);
        lexer.ignore((size_t)diff);

        if (ptr)
            *ptr = value;
        return true;
    }
};

template<typename ApT, ReadKind kind>
struct ReadElementConcrete<float, ApT, kind> {
    bool operator()(GenericLexer& lexer, va_list* ap)
    {
        lexer.ignore_while(isspace);

        auto* ptr = ap ? va_arg(*ap, ApT*) : nullptr;

        double value = 0;
        char* endptr = nullptr;
        auto nptr = lexer.remaining().characters_without_null_termination();
        if constexpr (kind == ReadKind::Normal)
            value = strtod(nptr, &endptr);
        else
            return false;

        if (!endptr)
            return false;

        if (endptr == nptr)
            return false;

        auto diff = endptr - nptr;
        VERIFY(diff > 0);
        lexer.ignore((size_t)diff);

        if (ptr)
            *ptr = value;
        return true;
    }
};

template<typename T, ReadKind kind>
struct ReadElement {
    bool operator()(LengthModifier length_modifier, GenericLexer& input_lexer, va_list* ap)
    {
        switch (length_modifier) {
        default:
        case None:
            VERIFY_NOT_REACHED();
        case Default:
            return ReadElementConcrete<T, T, kind> {}(input_lexer, ap);
        case Char:
            return ReadElementConcrete<T, char, kind> {}(input_lexer, ap);
        case Short:
            return ReadElementConcrete<T, short, kind> {}(input_lexer, ap);
        case Long:
            if constexpr (IsSame<T, int>)
                return ReadElementConcrete<T, long, kind> {}(input_lexer, ap);
            if constexpr (IsSame<T, unsigned>)
                return ReadElementConcrete<T, unsigned, kind> {}(input_lexer, ap);
            if constexpr (IsSame<T, float>)
                return ReadElementConcrete<int, double, kind> {}(input_lexer, ap);
            return false;
        case LongLong:
            if constexpr (IsSame<T, int>)
                return ReadElementConcrete<long long, long long, kind> {}(input_lexer, ap);
            if constexpr (IsSame<T, unsigned>)
                return ReadElementConcrete<unsigned long long, unsigned long long, kind> {}(input_lexer, ap);
            if constexpr (IsSame<T, float>)
                return ReadElementConcrete<long long, double, kind> {}(input_lexer, ap);
            return false;
        case IntMax:
            return ReadElementConcrete<T, intmax_t, kind> {}(input_lexer, ap);
        case Size:
            return ReadElementConcrete<T, size_t, kind> {}(input_lexer, ap);
        case PtrDiff:
            return ReadElementConcrete<T, ptrdiff_t, kind> {}(input_lexer, ap);
        case LongDouble:
            return ReadElementConcrete<T, long double, kind> {}(input_lexer, ap);
        }
    }
};

template<>
struct ReadElement<char*, ReadKind::Normal> {
    ReadElement(StringView scan_set = {}, bool invert = false)
        : scan_set(scan_set.is_null() ? " \t\n\f\r" : scan_set)
        , invert(scan_set.is_null() ? true : invert)
        , was_null(scan_set.is_null())
    {
    }

    bool operator()(LengthModifier length_modifier, GenericLexer& input_lexer, va_list* ap)
    {
        // FIXME: Implement wide strings and such.
        if (length_modifier != LengthModifier::Default)
            return false;

        if (was_null)
            input_lexer.ignore_while(isspace);

        auto* ptr = ap ? va_arg(*ap, char*) : nullptr;
        auto str = input_lexer.consume_while([this](auto c) { return this->matches(c); });
        if (str.is_empty())
            return false;

        memcpy(ptr, str.characters_without_null_termination(), str.length());
        ptr[str.length()] = 0;

        return true;
    }

private:
    bool matches(char c) const
    {
        return invert ^ scan_set.contains(c);
    }

    const StringView scan_set;
    bool invert { false };
    bool was_null { false };
};

template<>
struct ReadElement<void*, ReadKind::Normal> {
    bool operator()(LengthModifier length_modifier, GenericLexer& input_lexer, va_list* ap)
    {
        if (length_modifier != LengthModifier::Default)
            return false;

        input_lexer.ignore_while(isspace);

        auto* ptr = ap ? va_arg(*ap, void**) : nullptr;
        auto str = input_lexer.consume_while([this](auto c) { return this->should_consume(c); });

        if (count != 8) {
        fail:;
            for (size_t i = 0; i < count; ++i)
                input_lexer.retreat();
            return false;
        }

        char buf[9] { 0 };
        memcpy(buf, str.characters_without_null_termination(), 8);
        buf[8] = 0;
        char* endptr = nullptr;
        auto value = strtoull(buf, &endptr, 16);

        if (endptr != &buf[8])
            goto fail;

        memcpy(ptr, &value, sizeof(value));
        return true;
    }

private:
    bool should_consume(char c)
    {
        if (count == 8)
            return false;
        if (!isxdigit(c))
            return false;

        ++count;
        return true;
    }
    size_t count { 0 };
};

extern "C" int vsscanf(const char* input, const char* format, va_list ap)
{
    GenericLexer format_lexer { format };
    GenericLexer input_lexer { input };

    int elements_matched = 0;

    while (!format_lexer.is_eof()) {
        format_lexer.ignore_while(isspace);
        if (!format_lexer.next_is('%')) {
        read_one_literal:;
            input_lexer.ignore_while(isspace);
            if (format_lexer.is_eof())
                break;

            auto next_char = format_lexer.consume();
            if (!input_lexer.consume_specific(next_char))
                return elements_matched;
            continue;
        }

        if (format_lexer.next_is("%%")) {
            format_lexer.ignore();
            goto read_one_literal;
        }

        format_lexer.ignore(); // '%'

        bool suppress_assignment = false;
        if (format_lexer.next_is('*')) {
            suppress_assignment = true;
            format_lexer.ignore();
        }

        // Parse width specification
        [[maybe_unused]] int width_specifier = 0;
        if (format_lexer.next_is(isdigit)) {
            auto width_digits = format_lexer.consume_while([](char c) { return isdigit(c); });
            width_specifier = width_digits.to_int().value();
            // FIXME: Actually use width specifier
        }

        bool invert_scanlist = false;
        StringView scanlist;
        LengthModifier length_modifier { None };
        ConversionSpecifier conversion_specifier { Unspecified };
    reread_lookahead:;
        auto format_lookahead = format_lexer.peek();
        if (length_modifier == None) {
            switch (format_lookahead) {
            case 'h':
                if (format_lexer.peek(1) == 'h') {
                    format_lexer.consume(2);
                    length_modifier = Char;
                } else {
                    format_lexer.consume(1);
                    length_modifier = Short;
                }
                break;
            case 'l':
                if (format_lexer.peek(1) == 'l') {
                    format_lexer.consume(2);
                    length_modifier = LongLong;
                } else {
                    format_lexer.consume(1);
                    length_modifier = Long;
                }
                break;
            case 'j':
                format_lexer.consume();
                length_modifier = IntMax;
                break;
            case 'z':
                format_lexer.consume();
                length_modifier = Size;
                break;
            case 't':
                format_lexer.consume();
                length_modifier = PtrDiff;
                break;
            case 'L':
                format_lexer.consume();
                length_modifier = LongDouble;
                break;
            default:
                length_modifier = Default;
                break;
            }
            goto reread_lookahead;
        }
        if (conversion_specifier == Unspecified) {
            switch (format_lookahead) {
            case 'd':
                format_lexer.consume();
                conversion_specifier = Decimal;
                break;
            case 'i':
                format_lexer.consume();
                conversion_specifier = Integer;
                break;
            case 'o':
                format_lexer.consume();
                conversion_specifier = Octal;
                break;
            case 'u':
                format_lexer.consume();
                conversion_specifier = Unsigned;
                break;
            case 'x':
                format_lexer.consume();
                conversion_specifier = Hex;
                break;
            case 'a':
            case 'e':
            case 'f':
            case 'g':
                format_lexer.consume();
                conversion_specifier = Floating;
                break;
            case 's':
                format_lexer.consume();
                conversion_specifier = String;
                break;
            case '[':
                format_lexer.consume();
                scanlist = format_lexer.consume_until(']');
                if (scanlist.starts_with('^')) {
                    scanlist = scanlist.substring_view(1);
                    invert_scanlist = true;
                }
                conversion_specifier = UseScanList;
                break;
            case 'c':
                format_lexer.consume();
                conversion_specifier = Character;
                break;
            case 'p':
                format_lexer.consume();
                conversion_specifier = Pointer;
                break;
            case 'n':
                format_lexer.consume();
                conversion_specifier = OutputNumberOfBytes;
                break;
            case 'C':
                format_lexer.consume();
                length_modifier = Long;
                conversion_specifier = Character;
                break;
            case 'S':
                format_lexer.consume();
                length_modifier = Long;
                conversion_specifier = String;
                break;
            default:
                format_lexer.consume();
                conversion_specifier = Invalid;
                break;
            }
        }

        auto* ap_or_null = !suppress_assignment ? (va_list*)&ap : nullptr;

        // Now try to read.
        switch (conversion_specifier) {
        case Invalid:
        case Unspecified:
        default:
            // "undefined behaviour", let's be nice and crash.
            dbgln("Invalid conversion specifier {} in scanf!", (int)conversion_specifier);
            VERIFY_NOT_REACHED();
        case Decimal:
            if (!ReadElement<int, ReadKind::Normal> {}(length_modifier, input_lexer, ap_or_null))
                format_lexer.consume_all();
            else
                ++elements_matched;
            break;
        case Integer:
            if (!ReadElement<int, ReadKind::Infer> {}(length_modifier, input_lexer, ap_or_null))
                format_lexer.consume_all();
            else
                ++elements_matched;
            break;
        case Octal:
            if (!ReadElement<unsigned, ReadKind::Octal> {}(length_modifier, input_lexer, ap_or_null))
                format_lexer.consume_all();
            else
                ++elements_matched;
            break;
        case Unsigned:
            if (!ReadElement<unsigned, ReadKind::Normal> {}(length_modifier, input_lexer, ap_or_null))
                format_lexer.consume_all();
            else
                ++elements_matched;
            break;
        case Hex:
            if (!ReadElement<unsigned, ReadKind::Hex> {}(length_modifier, input_lexer, ap_or_null))
                format_lexer.consume_all();
            else
                ++elements_matched;
            break;
        case Floating:
            if (!ReadElement<float, ReadKind::Normal> {}(length_modifier, input_lexer, ap_or_null))
                format_lexer.consume_all();
            else
                ++elements_matched;
            break;
        case String:
            if (!ReadElement<char*, ReadKind::Normal> {}(length_modifier, input_lexer, ap_or_null))
                format_lexer.consume_all();
            else
                ++elements_matched;
            break;
        case UseScanList:
            if (!ReadElement<char*, ReadKind::Normal> { scanlist, invert_scanlist }(length_modifier, input_lexer, ap_or_null))
                format_lexer.consume_all();
            else
                ++elements_matched;
            break;
        case Character:
            if (!ReadElement<char, ReadKind::Normal> {}(length_modifier, input_lexer, ap_or_null))
                format_lexer.consume_all();
            else
                ++elements_matched;
            break;
        case Pointer:
            if (!ReadElement<void*, ReadKind::Normal> {}(length_modifier, input_lexer, ap_or_null))
                format_lexer.consume_all();
            else
                ++elements_matched;
            break;
        case OutputNumberOfBytes: {
            if (!suppress_assignment) {
                auto* ptr = va_arg(ap, int*);
                *ptr = input_lexer.tell();
            }
            ++elements_matched;
            break;
        }
        }
    }

    return elements_matched;
}
