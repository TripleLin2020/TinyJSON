#include "TinyJSON/Reader.h"
#include "TinyJSON/Writer.h"
#include "TinyJSON/FileReadStream.h"
#include "TinyJSON/FileWriteStream.h"

#include <cstdio>

using namespace json;

int main()
{
    FileReadStream is(stdin);
    FileWriteStream os(stdout);
    Writer writer(os);

    ParseError err = Reader::parse(is, writer);
    if (err != PARSE_OK) {
        printf("%s\n", parseErrorStr(err));
    }
}