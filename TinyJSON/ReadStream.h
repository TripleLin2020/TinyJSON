#ifndef TINY_JSON_READ_STREAM_H
#define TINY_JSON_READ_STREAM_H

#include "noncopyable.h"

#include <cstdio>
#include <cassert>
#include <vector>
#include <string_view>

namespace json
{

template<typename BUFFER_TYPE> requires std::ranges::contiguous_range<BUFFER_TYPE>
class ReadStream : noncopyable
{
public:
    using Buffer_Type = BUFFER_TYPE;
    using Iterator = BUFFER_TYPE::iterator;

public:
    ReadStream() = default;

    explicit ReadStream(BUFFER_TYPE _json) : buffer(_json), iter(_json.begin()) {}

    [[nodiscard]] bool hasNext() const { return iter != buffer.end(); }

    [[nodiscard]] char peek() { return hasNext() ? *iter : '\0'; }

    char next() {
        if (hasNext()) {
            char ch = *iter;
            iter++;
            return ch;
        }
        return '\0';
    };

    [[nodiscard]] Iterator getIter() const { return iter; }

    void assertNext(char ch) {
        assert(peek() == ch);
        next();
    }

protected:
    BUFFER_TYPE buffer;
    Iterator iter;
};

class StringReadStream : public ReadStream<std::string_view>
{
public:
    explicit StringReadStream(std::string_view _json) : ReadStream(_json) {}
};

class FileReadStream : public ReadStream<std::vector<char>>
{
public:
    explicit FileReadStream(FILE* input) {
        readStream(input);
        iter = buffer.begin();
    }

private:
    void readStream(FILE* input) {
        char buf[65536];
        while (true) {
            size_t n = fread(buf, 1, sizeof(buf), input);
            if (n == 0)
                break;
            buffer.insert(buffer.end(), buf, buf + n);
        }
    }
};

}

#endif //TINY_JSON_READ_STREAM_H
