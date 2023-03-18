#ifndef JSON_VALUE_H
#define JSON_VALUE_H

#include <cassert>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <algorithm>
#include <atomic>
#include <variant>

#include <TinyJSON/noncopyable.h>

namespace json
{


class Value;

typedef std::string String;
typedef std::shared_ptr<String> StringPtr;
typedef std::vector<Value> Array;
typedef std::shared_ptr<Array> ArrayPtr;
typedef std::pair<StringPtr, Value> Pair;
typedef std::vector<Pair> Object;
typedef std::shared_ptr<Object> ObjectPtr;

enum ValueType : size_t
{
    TYPE_NULL = 0,
    TYPE_BOOL,
    TYPE_INT32,
    TYPE_INT64,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_OBJECT,
};

class Document;

class Value
{
    friend class Document;

public:
    Value() : data(std::monostate()) {}

    template<typename T>
    requires std::convertible_to<T, std::variant<bool, int32_t, int64_t, double>>
    explicit Value(T newData) : data(newData) {}

    explicit Value(std::string_view s) : data(std::make_shared<String>(s.begin(), s.end())) {}

    explicit Value(const char* s) : data(std::make_shared<String>(s, s + strlen(s))) {}

    Value(const char* s, size_t len) : Value(std::string_view(s, len)) {}

    Value(const Value& rhs) : data(rhs.data) {}

    Value(Value&& rhs) : data(rhs.data) { rhs.data = std::monostate(); }

    Value& operator=(Value&& rhs) {
        if (this == &rhs) return *this;
        data = rhs.data;
        rhs.data = std::monostate();
        return *this;
    }

    ~Value() = default;

    static Value emptyString() {
        Value v;
        v.data = std::make_shared<String>();
        return v;
    }

    static Value emptyArray() {
        Value v;
        v.data = std::make_shared<Array>();
        return v;
    }

    static Value emptyObject() {
        Value v;
        v.data = std::make_shared<Object>();
        return v;
    }

    [[nodiscard]] ValueType getType() const { return static_cast<ValueType>(data.index()); }

    template<typename T>
    requires std::convertible_to<T, std::variant<bool, int32_t, int64_t, double, StringPtr, ArrayPtr, ObjectPtr>>
    // 获取类成员变量data保存的值，可以是bool, int32_t, int64_t, double基础类型，或StringPtr, ArrayPtr, ObjectPtr类型的智能指针
    [[nodiscard]] T getData() const {
        return std::get<T>(data);
    }

    template<typename T>
    requires std::convertible_to<T, std::variant<bool, int32_t, int64_t, double>>
    [[nodiscard]] Value& setData(T newData) {
        data = newData;
        return *this;
    }

    template<typename T>
    requires std::convertible_to<T, std::variant<String, Array, Object>>
    [[nodiscard]] Value& setData(T& newData) {
        data = std::make_shared<T>(newData);
        return *this;
    }

    [[nodiscard]] Value& operator[](const std::string& key) {
        Pair* p = findPair(key);
        assert(p && "Key does not exist");
        return p->second;
    }

    [[nodiscard]] const Value& operator[](const std::string& key) const { return const_cast<Value&>(*this)[key]; }

    [[nodiscard]] Pair* findPair(const std::string& key) {
        for (Pair& p: *std::get<ObjectPtr>(data)) {
            if (*p.first == key) return &p;
        }
        return nullptr;
    }

    [[nodiscard]] const Pair* findPair(const std::string& key) const { return const_cast<Value&>(*this).findPair(key); }

    template<typename T>
    requires std::convertible_to<T, std::variant<bool, int32_t, int64_t, double>>
    void addPair(std::string&& key, T&& value) {
        std::get<ObjectPtr>(data)->emplace_back(key, value);
    };

    void addPair(Value&& key, Value&& value) {
        std::get<ObjectPtr>(data)->emplace_back(std::get<StringPtr>(key.data), value);
    };

    template<typename T>
    requires std::convertible_to<T, std::variant<bool, int32_t, int64_t, double>>
    void addValue(T value) {
        std::get<ArrayPtr>(data)->emplace_back(value);
    }

    void addValue(Value&& value) {
        std::get<ArrayPtr>(data)->emplace_back(std::forward<Value>(value));
    }

    const Value& operator[](size_t i) const {
        return (*std::get<ArrayPtr>(data))[i];
    }

    Value& operator[](size_t i) {
        return (*std::get<ArrayPtr>(data))[i];
    }

    template<typename Handler>
    bool writeTo(Handler& handler) const;

private:

    std::variant<std::monostate, bool, int32_t, int64_t, double, StringPtr, ArrayPtr, ObjectPtr> data;
};

#define CALL(expr) do { if (!(expr)) return false; } while(false)

template<typename Handler>
inline bool Value::writeTo(Handler& handler) const {
    std::visit([&](auto& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            CALL(handler.Null());
        } else if constexpr(std::is_same_v<T, bool>) {
            CALL(handler.Bool(data));
        } else if constexpr(std::is_same_v<T, int32_t>) {
            CALL(handler.Bool(data));
        } else if constexpr(std::is_same_v<T, int64_t>) {
            CALL(handler.Bool(data));
        } else if constexpr(std::is_same_v<T, double>) {
            CALL(handler.Bool(data));
        } else if constexpr(std::is_same_v<T, StringPtr>) {
            CALL(handler.String(*getData<StringPtr>()));
        } else if constexpr(std::is_same_v<T, ArrayPtr>) {
            CALL(handler.StartArray());
            for (auto& val: *getData<ArrayPtr>()) {
                CALL(val.writeTo(handler));
            }
            CALL(handler.EndArray());
        } else if constexpr(std::is_same_v<T, ObjectPtr>) {
            for (auto& pair: *getData<ObjectPtr>()) {
                handler.Key(*pair.first);
                CALL(pair.second.writeTo(handler));
            }
            CALL(handler.EndObject());
        } else {
            assert(false && "non-exhaustive visitor!");
        }
    }, data);
//    switch (type_) {
//        case TYPE_NULL:
//            CALL(handler.Null());
//            break;
//        case TYPE_BOOL:
//            CALL(handler.Bool(b_));
//            break;
//        case TYPE_INT32:
//            CALL(handler.Int32(i32_));
//            break;
//        case TYPE_INT64:
//            CALL(handler.Int64(i64_));
//            break;
//        case TYPE_DOUBLE:
//            CALL(handler.Double(d_));
//            break;
//        case TYPE_STRING:
//            CALL(handler.String(getStringView()));
//            break;
//        case TYPE_ARRAY:
//            CALL(handler.StartArray());
//            for (auto& val: getArray()) {
//                CALL(val.writeTo(handler));
//            }
//            CALL(handler.EndArray());
//            break;
//        case TYPE_OBJECT:
//            CALL(handler.StartObject());
//            for (auto& member: getObject()) {
//                handler.Key(member.key.getStringView());
//                CALL(member.value.writeTo(handler));
//            }
//            CALL(handler.EndObject());
//            break;
//        default:
//            assert(false && "bad type");
//    }
    return true;
}

#undef CALL

}

#endif //JSON_VALUE_H
