#include <cstdio>
#include <string_view>

#include <TinyJSON/Writer.h>
#include <TinyJSON/FileWriteStream.h>

using namespace json;
using namespace std::string_view_literals;

int main()
{
    FileWriteStream os(stdout);
    Writer writer(os);

    printf("%d\n", INFINITY == INFINITY);

    writer.StartArray();
    writer.Double(INFINITY);
    writer.Double(NAN);
    writer.Bool(true);
    writer.String("\0蛤蛤蛤\0"sv);
    writer.Bool(true);
    writer.EndArray();
}