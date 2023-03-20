#ifndef TINY_JSON_WRITE_STREAM_H
#define TINY_JSON_WRITE_STREAM_H

#include "noncopyable.h"

#include <cstdio>
#include <vector>
#include <cstring>
#include <string_view>

namespace json
{

class WriteStream : noncopyable
{
public:
    WriteStream() = default;

    virtual void put(char c) = 0;

    virtual void put(const char* str) = 0;

    virtual void put(const std::string_view&) = 0;
};

class StringWriteStream : public WriteStream
{
public:
    void put(char c) override { buffer.push_back(c); }

    void put(const char* str) override {
        buffer.insert(buffer.end(), str, str + std::strlen(str));
    }

    void put(const std::string_view& str) override {
        buffer.insert(buffer.end(), str.begin(), str.end());
    }

    [[nodiscard]] std::string_view get() const {
        return std::string_view(&*buffer.begin(), buffer.size());
    }

private:
    std::vector<char> buffer;
};


class FileWriteStream : public WriteStream
{
public:
    explicit FileWriteStream(FILE* _output) : output(_output) {}

    void put(char c) override { putc(c, output); }

    void put(const char* str) override { fputs(str, output); }

    void put(const std::string_view& str) override {
        fprintf(output, "%.*s", static_cast<int>(str.length()), str.data());
    }

private:
    FILE* output;
};

}

#endif //TINY_JSON_WRITE_STREAM_H
