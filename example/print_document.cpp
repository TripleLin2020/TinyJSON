#include "sample.h"

#include "Document.h"
#include "Writer.h"
#include "WriteStream.h"

using namespace json;

int main()
{
    Document document;
    if (ParseError err = document.parse(sample[0]); err != PARSE_OK) {
        puts(parseErrorStr(err));
        return 1;
    }

    FileWriteStream os(stdout);
    Writer writer(os);
    document.writeTo(writer);
}