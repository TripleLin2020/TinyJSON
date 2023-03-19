#ifndef JSON_STRING_READ_STREAM_H
#define JSON_STRING_READ_STREAM_H

#include "TinyJSON/noncopyable.h"

#include <string>
#include <cassert>

namespace json
{
class StringReadStream : noncopyable
{
public:
    using Iterator = std::string_view::iterator;

public:
    explicit StringReadStream(std::string_view& _json) : json(_json), iter(_json.begin()) {}

    [[nodiscard]] bool hasNext() const { return iter != json.end(); }

    char peek() { return hasNext() ? *iter : '\0'; }

    [[nodiscard]] Iterator getIter() const { return iter; }

    char next() {
        if (hasNext()) {
            char ch = *iter;
            iter++;
            return ch;
        }
        return '\0';
    };

    void assertNext(char ch) {
        assert(peek() == ch);
        next();
    }

private:
    std::string_view json;
    Iterator iter;
};


}
#endif //JSON_STRING_READ_STREAM_H
