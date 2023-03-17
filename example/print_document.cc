//
// Created by frank on 17-12-26.
//

#include <TinyJSON/Document.h>
#include <TinyJSON/Writer.h>
#include <TinyJSON/FileWriteStream.h>

#include "sample.h"

using namespace json;

int main()
{
    Document document;
    ParseError err = document.parse(sample[0]);
    if (err != PARSE_OK) {
        puts(parseErrorStr(err));
        return 1;
    }

    FileWriteStream os(stdout);
    Writer writer(os);
    document.writeTo(writer);
}