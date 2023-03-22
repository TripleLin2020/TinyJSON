#include "TinyJSON/Writer.h"
#include "TinyJSON/WriteStream.h"

#include <cstdio>
#include <string_view>

using namespace json;
using namespace std::string_view_literals;

int main()
{
    FileWriteStream os(stdout);
    Writer writer(os);

    writer.StartArray();
    writer.Double(INFINITY);
    writer.Double(NAN);
    writer.Bool(true);
    writer.String("\0蛤蛤蛤\0"sv);
    writer.Bool(true);
    writer.EndArray();
}