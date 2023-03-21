#ifndef TINY_JSON_VALUE_H
#define TINY_JSON_VALUE_H

#include "noncopyable.h"

#include <cassert>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <algorithm>
#include <atomic>
#include <variant>

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


//Must be consistent with the order defined by the member variable "data" type
enum ValueType : size_t
{
    TYPE_NULL = 0,
    TYPE_BOOL,
    TYPE_INT32,
    TYPE_INT64,
    TYPE_DOUBLE,
    TYPE_STRING_PTR,
    TYPE_ARRAY_PTR,
    TYPE_OBJECT_PTR,
};

class Document;

class Value
{
    friend class Document;

    friend std::ostream& operator<<(std::ostream& os, const Value& v);

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

    Value& operator=(const Value& rhs) = default;

    ~Value() = default;

    template<typename T>
    requires std::convertible_to<T, std::variant<bool, int32_t, int64_t, double>>
    Value& operator=(const T rhs) {
        data = rhs;
        return *this;
    }

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
    // Get the value saved by the class member variable "data",
    // which can be bool, int32_t, int64_t, double, or a smart pointer of type StringPtr, ArrayPtr, ObjectPtr
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
    requires std::convertible_to<T, std::variant<bool, int32_t, int64_t, double, String>>
    void addPair(String&& key, T&& value) {
        assert(data.index() == TYPE_OBJECT_PTR && "Non-object types cannot add key-value pairs");
        std::get<ObjectPtr>(data)->emplace_back(std::make_shared<String>(key), value);
    };

    void addPair(Value&& key, Value&& value) {
        assert(data.index() == TYPE_OBJECT_PTR && "Non-object types cannot add key-value pairs");
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
    bool res = std::visit([&](auto& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            CALL(handler.Null());
        } else if constexpr (std::is_same_v<T, bool>) {
            CALL(handler.Bool(getData<bool>()));
        } else if constexpr (std::is_same_v<T, int32_t>) {
            CALL(handler.Int32(getData<int32_t>()));
        } else if constexpr (std::is_same_v<T, int64_t>) {
            CALL(handler.Int64(getData<int64_t>()));
        } else if constexpr (std::is_same_v<T, double>) {
            CALL(handler.Double(getData<double>()));
        } else if constexpr (std::is_same_v<T, StringPtr>) {
            CALL(handler.String(*getData<StringPtr>()));
        } else if constexpr (std::is_same_v<T, ArrayPtr>) {
            CALL(handler.StartArray());
            for (auto& val: *getData<ArrayPtr>()) {
                CALL(val.writeTo(handler));
            }
            CALL(handler.EndArray());
        } else if constexpr (std::is_same_v<T, ObjectPtr>) {
            CALL(handler.StartObject());
            for (auto& pair: *getData<ObjectPtr>()) {
                handler.Key(*pair.first);
                CALL(pair.second.writeTo(handler));
            }
            CALL(handler.EndObject());
        } else {
            assert(false && "non-exhaustive visitor!");
            return false;
        }
        return true;
    }, data);
    return res;
}

#undef CALL

std::ostream& operator<<(std::ostream& os, const Value& v) {
    std::visit([&](auto& data) {
        using T = std::decay_t<decltype(data)>;
        if constexpr (std::convertible_to<T, std::variant<bool, int32_t, int64_t, double>>) {
            os << data;
        } else if constexpr (std::is_same_v<T, StringPtr>) {
            os << *data;
        } else {
            assert(false && "unsupported type");
        }

    }, v.data);
    return os;
}

}

#endif //TINY_JSON_VALUE_H
