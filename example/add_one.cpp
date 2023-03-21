#include "sample.h"

#include "Writer.h"
#include "Reader.h"
#include "ReadStream.h"
#include "WriteStream.h"

using namespace json;

template<typename Handler>
class AddOne : noncopyable
{
public:
    explicit AddOne(Handler& _handler) : handler(_handler) {}

    bool Null() { return handler.Null(); }

    bool Bool(bool b) { return handler.Bool(b); }

    bool Int32(int32_t i32) { return handler.Int32(i32 + 1); }

    bool Int64(int64_t i64) { return handler.Int64(i64 + 1); }

    bool Double(double d) { return handler.Double(d + 1); }

    bool String(std::string_view s) { return handler.String(s); }

    bool StartObject() { return handler.StartObject(); }

    bool Key(std::string_view s) { return handler.Key(s); }

    bool EndObject() { return handler.EndObject(); }

    bool StartArray() { return handler.StartArray(); }

    bool EndArray() { return handler.EndArray(); }

private:
    Handler& handler;
};

int main() {
    StringReadStream is(sample[0]);
    FileWriteStream os(stdout);
    Writer writer(os);
    AddOne addOne(writer);

    if (ParseError err = Reader::parse(is, addOne); err != PARSE_OK) {
        printf("%s\n", parseErrorStr(err));
    }
}