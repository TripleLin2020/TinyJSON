#ifndef JSON_DOCUMENT_H
#define JSON_DOCUMENT_H

#include "TinyJSON/Reader.h"
#include "TinyJSON/StringReadStream.h"
#include "TinyJSON/Value.h"

#include <string>

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

public:  // handler
    bool Null() {
        addValue(Value());
        return true;
    }

    bool Bool(bool b) {
        addValue(Value(b));
        return true;
    }

    bool Int32(int32_t i32) {
        addValue(Value(i32));
        return true;
    }

    bool Int64(int64_t i64) {
        addValue(Value(i64));
        return true;
    }

    bool Double(double d) {
        addValue(Value(d));
        return true;
    }

    bool String(std::string_view s) {
        addValue(Value(s));
        return true;
    }

    bool StartObject() {
        auto value = addValue(Value::emptyObject());
        stack_.emplace_back(value);
        return true;
    }

    bool Key(std::string_view s) {
        addValue(Value(s));
        return true;
    }

    bool EndObject() {
        assert(!stack_.empty());
        assert(stack_.back().type() == TYPE_OBJECT);
        stack_.pop_back();
        return true;
    }

    bool StartArray() {
        auto value = addValue(Value::emptyArray());
        stack_.emplace_back(value);
        return true;
    }

    bool EndArray() {
        assert(!stack_.empty());
        assert(stack_.back().type() == TYPE_ARRAY);
        stack_.pop_back();
        return true;
    }

private:
    Value* addValue(Value&& value) {
        if (seeValue_)
            assert(!stack_.empty() && "root not singular");
        else {
            seeValue_ = true;
            data = value.data;
            return this;
        }

        auto& top = stack_.back();
        if (top.type() == TYPE_ARRAY) {
            top.value->addValue(std::move(value));
            top.valueCount++;
            return top.lastValue();
        } else {
            assert(top.type() == TYPE_OBJECT);

            if (top.valueCount % 2 == 0) {
                // assert(type == TYPE_STRING && "miss quotation mark");
                key_ = std::move(value);
                top.valueCount++;
                return &key_;
            } else {
                top.value->addPair(std::move(key_), std::move(value));
                top.valueCount++;
                return top.lastValue();
            }
        }
    }

private:
    struct Level
    {
        explicit Level(Value* value_) : value(value_), valueCount(0) {}

        ValueType type() const { return value->getType(); }

        Value* lastValue() {
            if (type() == TYPE_ARRAY) {
                return &std::get<ArrayPtr>(value->data)->back();
            } else {
                return &std::get<ObjectPtr>(value->data)->back().second;
            }
        }

        Value* value;
        int valueCount;
    };

private:
    std::vector<Level> stack_;
    Value key_;
    bool seeValue_ = false;
};

}  // namespace json

#endif  // JSON_DOCUMENT_H
