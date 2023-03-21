#ifndef TINY_JSON_DOCUMENT_H
#define TINY_JSON_DOCUMENT_H

#include "Reader.h"
#include "ReadStream.h"
#include "Value.h"

#include <string>
#include <stack>

namespace json
{

class Document : public Value
{
public:
    ParseError parse(const char* json, size_t len) { return parse(std::string_view(json, len)); }

    ParseError parse(std::string_view json) {
        StringReadStream is(json);
        return parseStream(is);
    }

    template<typename ReadStream>
    ParseError parseStream(ReadStream& is) {
        return Reader::parse(is, *this);
    }

public:
    bool Null() {
        add(Value());
        return true;
    }

    bool Bool(bool b) {
        add(Value(b));
        return true;
    }

    bool Int32(int32_t i32) {
        add(Value(i32));
        return true;
    }

    bool Int64(int64_t i64) {
        add(Value(i64));
        return true;
    }

    bool Double(double d) {
        add(Value(d));
        return true;
    }

    bool String(std::string_view s) {
        add(Value(s));
        return true;
    }

    bool StartObject() {
        st.emplace(add(Value::emptyObject()));
        return true;
    }

    bool Key(std::string_view s) {
        add(Value(s));
        return true;
    }

    bool EndObject() {
        assert(!st.empty());
        assert(st.top().type() == TYPE_OBJECT_PTR);
        st.pop();
        return true;
    }

    bool StartArray() {
        st.emplace(add(Value::emptyArray()));
        return true;
    }

    bool EndArray() {
        assert(!st.empty());
        assert(st.top().type() == TYPE_ARRAY_PTR);
        st.pop();
        return true;
    }

private:
    Value* add(Value&& value) {
        if (isFirstValue) {
            isFirstValue = false;
            data = value.data;
            return this;
        } else {
            assert(!st.empty() && "root not singular");
        }

        Level& top = st.top();
        if (top.type() == TYPE_ARRAY_PTR) {
            top.value->addToArray(std::move(value));
            top.valueCount++;
            return top.lastValue();
        } else {
            assert(top.type() == TYPE_OBJECT_PTR);

            if (top.valueCount % 2 == 0) {
                key = std::move(value);
                top.valueCount++;
                return &key;
            } else {
                top.value->addPair(std::move(key), std::move(value));
                top.valueCount++;
                return top.lastValue();
            }
        }
    }

private:
    struct Level
    {
        explicit Level(Value* value_) : value(value_), valueCount(0) {}

        [[nodiscard]] ValueType type() const { return value->getType(); }

        [[nodiscard]] Value* lastValue() const {
            if (type() == TYPE_ARRAY_PTR) {
                return &std::get<ArrayPtr>(value->data)->back();
            } else {
                assert(type() == TYPE_OBJECT_PTR);
                return &std::get<ObjectPtr>(value->data)->back().second;
            }
        }

        Value* value;
        int valueCount;
    };

private:
    std::stack<Level> st;
    Value key;
    bool isFirstValue = true;
};

}  // namespace json

#endif  // TINY_JSON_DOCUMENT_H
