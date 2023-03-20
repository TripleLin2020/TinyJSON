#ifndef JSON_READER_H
#define JSON_READER_H

#include "Exception.h"
#include "Value.h"
#include "ReadStream.h"

#include <cassert>
#include <cmath>
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include <charconv>

namespace json
{

class Reader : noncopyable
{
public:
    template<typename RS, typename Handler>
    requires std::is_base_of_v<ReadStream<typename RS::Buffer_Type>, RS>
    static ParseError parse(RS& is, Handler& handler) {
        try {
            parseWhitespace(is);
            parseValue(is, handler);
            parseWhitespace(is);
            if (is.hasNext()) throw Exception(PARSE_ROOT_NOT_SINGULAR);
            return PARSE_OK;
        } catch (Exception& e) {
            return e.err();
        }
    }

private:
#define CALL(expr) \
    if (!(expr)) throw Exception(PARSE_USER_STOPPED)

    template<typename ReadStream>
    static unsigned parseHex4(ReadStream& is) {
        // unicode stuff from Milo's tutorial
        unsigned u = 0;
        for (int i = 0; i < 4; i++) {
            u <<= 4;
            switch (char ch = is.next()) {
                case '0' ... '9':
                    u |= static_cast<unsigned>(ch - '0');
                    break;
                case 'a' ... 'f':
                    u |= static_cast<unsigned>(ch - 'a' + 10);
                    break;
                case 'A' ... 'F':
                    u |= static_cast<unsigned>(ch - 'A' + 10);
                    break;
                default:
                    throw Exception(PARSE_BAD_UNICODE_HEX);
            }
        }
        return u;
    }

    template<typename T>
    requires std::is_base_of_v<ReadStream<typename T::Buffer_Type>, T>
    static void parseWhitespace(T& is) {
        while (is.hasNext()) {
            char ch = is.peek();
            if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
                is.next();
            } else {
                break;
            }
        }
    }

    template<typename RS, typename Handler>
    requires std::is_base_of_v<ReadStream<typename RS::Buffer_Type>, RS>
    static void parseLiteral(RS& is, Handler& handler, const char* literal, ValueType type) {
        char c = *literal;

        is.assertNext(*literal++);
        while (*literal != '\0' && *literal == is.peek()) {
            literal++;
            is.next();
        }
        if (*literal == '\0') {
            switch (type) {
                case TYPE_NULL:
                    CALL(handler.Null());
                    break;
                case TYPE_BOOL:
                    CALL(handler.Bool(c == 't'));
                    break;
                case TYPE_DOUBLE:
                    CALL(handler.Double(c == 'N' ? NAN : INFINITY));
                    break;
                default:
                    assert(false && "bad type");
                    throw Exception(PARSE_BAD_VALUE);
            }
        }
    }

    template<typename RS, typename Handler>
    requires std::is_base_of_v<ReadStream<typename RS::Buffer_Type>, RS>
    static void parseNumber(RS& is, Handler& handler) {
        // parse 'NaN' (Not a Number) and 'Infinity'
        if (is.peek() == 'N') {
            parseLiteral(is, handler, "NaN", TYPE_DOUBLE);
            return;
        } else if (is.peek() == 'I') {
            parseLiteral(is, handler, "Infinity", TYPE_DOUBLE);
            return;
        }

        auto start = is.getIter();

        if (is.peek() == '-') is.next();

        if (is.peek() == '0') {
            is.next();
            if (isdigit(is.peek())) throw Exception(PARSE_BAD_VALUE);
        } else if (isDigit19(is.peek())) {
            is.next();
            while (isDigit(is.peek())) is.next();
        } else {
            throw Exception(PARSE_BAD_VALUE);
        }

        ValueType expectType = TYPE_NULL;

        if (is.peek() == '.') {
            expectType = TYPE_DOUBLE;
            is.next();
            if (!isDigit(is.peek())) throw Exception(PARSE_BAD_VALUE);
            is.next();
            while (isDigit(is.peek())) is.next();
        }
        if (is.peek() == 'e' || is.peek() == 'E') {
            expectType = TYPE_DOUBLE;
            is.next();
            if (is.peek() == '+' || is.peek() == '-') is.next();
            if (!isDigit(is.peek())) throw Exception(PARSE_BAD_VALUE);
            is.next();
            while (isDigit(is.peek())) is.next();
        }

        // Whether i32 or i64 is specified
        if (is.peek() == 'i') {
            is.next();
            if (expectType == TYPE_DOUBLE) throw Exception(PARSE_BAD_VALUE);
            switch (is.next()) {
                case '3':
                    if (is.next() != '2') throw Exception(PARSE_BAD_VALUE);
                    expectType = TYPE_INT32;
                    break;
                case '6':
                    if (is.next() != '4') throw Exception(PARSE_BAD_VALUE);
                    expectType = TYPE_INT64;
                    break;
                default:
                    throw Exception(PARSE_BAD_VALUE);
            }
        }

        auto end = is.getIter();
        if (start == end) throw Exception(PARSE_BAD_VALUE);

        if (expectType == TYPE_DOUBLE) {
            double d;
            if (auto res = std::from_chars(start, end, d); res.ec != std::errc()) {
                assert("Fail to convert string to double");
                throw Exception(PARSE_BAD_VALUE);
            }
            CALL(handler.Double(d));
        } else {
            int64_t i64;
            if (auto res = std::from_chars(start, end, i64); res.ec != std::errc()) {
                assert("Fail to convert string to int64");
                throw Exception(PARSE_BAD_VALUE);
            }
            if (expectType == TYPE_INT64) {
                CALL(handler.Int64(i64));
            } else if (expectType == TYPE_INT32) {
                if (i64 > std::numeric_limits<int32_t>::max() || i64 < std::numeric_limits<int32_t>::min()) {
                    throw std::out_of_range("int32_t overflow");
                }
                CALL(handler.Int32(static_cast<int32_t>(i64)));
            } else if (i64 <= std::numeric_limits<int32_t>::max() && i64 >= std::numeric_limits<int32_t>::min()) {
                CALL(handler.Int32(static_cast<int32_t>(i64)));
            } else {
                CALL(handler.Int64(i64));
            }
        }
    }

    template<typename RS, typename Handler>
    requires std::is_base_of_v<ReadStream<typename RS::Buffer_Type>, RS>
    static void parseString(RS& is, Handler& handler, bool isKey) {
        is.assertNext('"');
        std::string buffer;
        while (is.hasNext()) {
            switch (char ch = is.next()) {
                case '"':
                    if (isKey) {
                        CALL(handler.Key(std::move(buffer)));
                    } else {
                        CALL(handler.String(std::move(buffer)));
                    }
                    return;
                case '\x01' ... '\x1f':
                    throw Exception(PARSE_BAD_STRING_CHAR);
                case '\\':
                    switch (is.next()) {
                        case '"':
                            buffer.push_back('"');
                            break;
                        case '\\':
                            buffer.push_back('\\');
                            break;
                        case '/':
                            buffer.push_back('/');
                            break;
                        case 'b':
                            buffer.push_back('\b');
                            break;
                        case 'f':
                            buffer.push_back('\f');
                            break;
                        case 'n':
                            buffer.push_back('\n');
                            break;
                        case 'r':
                            buffer.push_back('\r');
                            break;
                        case 't':
                            buffer.push_back('\t');
                            break;
                        case 'u': {
                            // unicode stuff from Milo's tutorial
                            unsigned u = parseHex4(is);
                            if (u >= 0xD800 && u <= 0xDBFF) {
                                if (is.next() != '\\') throw Exception(PARSE_BAD_UNICODE_SURROGATE);
                                if (is.next() != 'u') throw Exception(PARSE_BAD_UNICODE_SURROGATE);
                                unsigned u2 = parseHex4(is);
                                if (u2 >= 0xDC00 && u2 <= 0xDFFF)
                                    u = 0x10000 + (u - 0xD800) * 0x400 + (u2 - 0xDC00);
                                else
                                    throw Exception(PARSE_BAD_UNICODE_SURROGATE);
                            }
                            encodeUtf8(buffer, u);
                            break;
                        }
                        default:
                            throw Exception(PARSE_BAD_STRING_ESCAPE);
                    }
                    break;
                default:
                    buffer.push_back(ch);
            }
        }
        throw Exception(PARSE_MISS_QUOTATION_MARK);
    }

    template<typename RS, typename Handler>
    requires std::is_base_of_v<ReadStream<typename RS::Buffer_Type>, RS>
    static void parseArray(RS& is, Handler& handler) {
        CALL(handler.StartArray());

        is.assertNext('[');
        parseWhitespace(is);
        if (is.peek() == ']') {
            is.next();
            CALL(handler.EndArray());
            return;
        }

        while (true) {
            parseValue(is, handler);
            parseWhitespace(is);
            switch (is.next()) {
                case ',':
                    parseWhitespace(is);
                    break;
                case ']':
                    CALL(handler.EndArray());
                    return;
                default:
                    throw Exception(PARSE_MISS_COMMA_OR_SQUARE_BRACKET);
            }
        }
    }

    template<typename RS, typename Handler>
    requires std::is_base_of_v<ReadStream<typename RS::Buffer_Type>, RS>
    static void parseObject(RS& is, Handler& handler) {
        CALL(handler.StartObject());

        is.assertNext('{');
        parseWhitespace(is);
        if (is.peek() == '}') {
            is.next();
            CALL(handler.EndObject());
            return;
        }

        while (true) {
            if (is.peek() != '"') throw Exception(PARSE_MISS_KEY);

            parseString(is, handler, true);

            parseWhitespace(is);
            if (is.next() != ':') throw Exception(PARSE_MISS_COLON);
            parseWhitespace(is);

            parseValue(is, handler);
            parseWhitespace(is);
            switch (is.next()) {
                case ',':
                    parseWhitespace(is);
                    break;
                case '}':
                    CALL(handler.EndObject());
                    return;
                default:
                    throw Exception(PARSE_MISS_COMMA_OR_CURLY_BRACKET);
            }
        }
    }

#undef CALL

    template<typename RS, typename Handler>
    requires std::is_base_of_v<ReadStream<typename RS::Buffer_Type>, RS>
    static void parseValue(RS& is, Handler& handler) {
        if (!is.hasNext()) throw Exception(PARSE_EXPECT_VALUE);

        switch (is.peek()) {
            case 'n':
                return parseLiteral(is, handler, "null", TYPE_NULL);
            case 't':
                return parseLiteral(is, handler, "true", TYPE_BOOL);
            case 'f':
                return parseLiteral(is, handler, "false", TYPE_BOOL);
            case '"':
                return parseString(is, handler, false);
            case '[':
                return parseArray(is, handler);
            case '{':
                return parseObject(is, handler);
            default:
                return parseNumber(is, handler);
        }
    }

private:
    static bool isDigit(char ch) { return ch >= '0' && ch <= '9'; }

    static bool isDigit19(char ch) { return ch >= '1' && ch <= '9'; }

    static void encodeUtf8(std::string& buffer, unsigned u) {
        switch (u) {
            case 0x00 ... 0x7F:
                buffer.push_back(static_cast<char>(u & 0xFF));
                break;
            case 0x080 ... 0x7FF:
                buffer.push_back(static_cast<char>(0xC0 | (u >> 6)));
                buffer.push_back(static_cast<char>(0x80 | (u & 0x3F)));
                break;
            case 0x0800 ... 0xFFFF:
                buffer.push_back(static_cast<char>(0xE0 | (u >> 12)));
                buffer.push_back(static_cast<char>(0x80 | ((u >> 6) & 0x3F)));
                buffer.push_back(static_cast<char>(0x80 | (u & 0x3F)));
                break;
            case 0x010000 ... 0x10FFFF:
                buffer.push_back(static_cast<char>(0xF0 | (u >> 18)));
                buffer.push_back(static_cast<char>(0x80 | ((u >> 12) & 0x3F)));
                buffer.push_back(static_cast<char>(0x80 | ((u >> 6) & 0x3F)));
                buffer.push_back(static_cast<char>(0x80 | (u & 0x3F)));
                break;
            default:
                assert(false && "out of range");
        }
    }
};

}  // namespace json

#endif  // JSON_READER_H
