#include "TinyJSON/Document.h"
#include "TinyJSON/ReadStream.h"
#include "TinyJSON/WriteStream.h"
#include "TinyJSON/Writer.h"

#include <cstdio>
#include <iostream>

using namespace json;

int main() {
    // 1.把JSON解析到DOM
    Document document;
    if (ParseError err = document.parse(R"({"Zip": "94107"})"); err != PARSE_OK) {
        std::cout << parseErrorStr(err) << std::endl;
        return 1;
    }

    // 2.利用DOM进行修改：Key"Zip"存储的Value从字符串"94107"变为了i32类型的66666
    Value& zip = document["Zip"];
    zip = 66666;

    // 3.把DOM转化回JSON并输出
    FileWriteStream os(stdout);
    Writer writer(os);
    document.writeTo(writer);
}