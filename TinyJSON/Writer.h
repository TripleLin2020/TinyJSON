#ifndef JSON_WRITER_H
#define JSON_WRITER_H

#include "Value.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <string>
#include <stack>
#include <charconv>

namespace json
{

template<typename WriteStream> requires requires(WriteStream os) { os.put(""); }
class Writer : noncopyable
{
public:
    explicit Writer(WriteStream& _os) : os(_os) {}

    bool Null() {
        prefix(TYPE_NULL);
        os.put("null");
        return true;
    }

    bool Bool(bool b) {
        prefix(TYPE_BOOL);
        os.put(b ? "true" : "false");
        return true;
    }

    bool Int32(int32_t i32) {
        prefix(TYPE_INT32);

        char buf[12]{};
        if (auto res = std::to_chars(buf, buf + sizeof(buf), i32); res.ec != std::errc()) {
            assert("Failed to convert double to string");
            return false;
        }
        os.put(buf);
        return true;
    }

    bool Int64(int64_t i64) {
        prefix(TYPE_INT64);

        char buf[21]{};
        if (auto res = std::to_chars(buf, buf + sizeof(buf), i64); res.ec != std::errc()) {
            assert("Failed to convert double to string");
            return false;
        }
        os.put(buf);
        return true;
    }

    bool Double(double d) {
        prefix(TYPE_DOUBLE);

        char buf[32]{};

        if (std::isinf(d)) {
            strcpy(buf, "Infinity");
        } else if (std::isnan(d)) {
            strcpy(buf, "NaN");
        } else {
            if (auto res = std::to_chars(buf, buf + sizeof(buf), d); res.ec != std::errc()) {
                assert("Failed to convert double to string");
                return false;
            }

            // type information loss if ".0" not added
            // "1.0" -> double 1 -> "1"
            auto it = std::find_if_not(buf, buf + sizeof(buf), isdigit);
            if (it == buf + sizeof(buf)) {
                strcat(buf, ".0");
            }
        }

        os.put(buf);
        return true;
    }

    bool String(std::string_view s) {
        prefix(TYPE_STRING);
        os.put('"');
        for (auto c: s) {
            auto u = static_cast<unsigned char>(c);
            switch (u) {
                case '\"':
                    os.put("\\\"");
                    break;
                case '\b':
                    os.put("\\b");
                    break;
                case '\f':
                    os.put("\\f");
                    break;
                case '\n':
                    os.put("\\n");
                    break;
                case '\r':
                    os.put("\\r");
                    break;
                case '\t':
                    os.put("\\t");
                    break;
                case '\\':
                    os.put("\\\\");
                    break;
                default:
                    if (u < 0x20) {
                        char buf[7];
                        snprintf(buf, 7, "\\u%04X", u);
                        os.put(buf);
                    } else
                        os.put(c);
                    break;
            }
        }

        os.put('"');
        return true;
    }

    bool StartObject() {
        prefix(TYPE_OBJECT);
        st.emplace(false);
        os.put('{');
        return true;
    }

    bool Key(std::string_view s) {
        prefix(TYPE_STRING);
        os.put('"');
        os.put(s);
        os.put('"');
        return true;
    }

    bool EndObject() {
        assert(!st.empty());
        assert(!st.top().isInArray);
        st.pop();
        os.put('}');
        return true;
    }

    bool StartArray() {
        prefix(TYPE_ARRAY);
        st.emplace(true);
        os.put('[');
        return true;
    }

    bool EndArray() {
        assert(!st.empty());
        assert(st.top().isInArray);
        st.pop();
        os.put(']');
        return true;
    }

private:
    // The function is used to determine whether a prefix such as ',' or ':' needs to be added,
    // when the value is in an object or array.
    void prefix(ValueType type) {
        if (st.empty()) return;

        auto& top = st.top();
        if (top.isInArray) {
            if (top.valueCount > 0) os.put(',');
        } else {
            if (top.valueCount % 2 == 1) {
                // object's value
                os.put(':');
            } else {
                // object's key
                assert(type == TYPE_STRING && "miss quotation mark");
                if (top.valueCount > 0) os.put(',');
            }
        }
        top.valueCount++;
    }

private:
    struct Level
    {
        explicit Level(bool _isInArray) : isInArray(_isInArray), valueCount(0) {}

        bool isInArray;  // true: in array; false: in object
        int valueCount;
    };

private:
    std::stack<Level> st;
    WriteStream& os;
};

}  // namespace json

#endif  //JSON_WRITER_H
