#ifndef JSON_FILE_WRITE_STREAM_H
#define JSON_FILE_WRITE_STREAM_H

#include "TinyJSON/noncopyable.h"

#include <string>
#include <cstdio>

namespace json
{

class FileWriteStream : noncopyable
{
public:
    explicit FileWriteStream(FILE* _output) : output(_output) {}

    void put(char c) { putc(c, output); }

    void put(const char* str) { fputs(str, output); }

    void put(const std::string_view& str) {
        fprintf(output, "%.*s", static_cast<int>(str.length()), str.data());
    }

private:
    FILE* output;
};

}

#endif //JSON_FILE_WRITE_STREAM_H
