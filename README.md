# TinyJSON: a simple JSON parser/generator
## 简介
TinyJson是使用c++20编写的一个简单的JSON解析器/生成器, 已实现了：<br>
1. ReadStream：用于读取数据流。使用FILE对象输入的FileReadString和从内存读取的StringReadStream。
2. WriteStream：用于输出数据流。使用FILE对象输出的FileWriteStream和输出到内存的StringWriteString。
3. Reader：用于解析JSON。
4. Document：用于构建树形存储结构
5. Writer：用于输出JSON。

## 用法
解析一个JSON字符串到DOM，对DOM进行简单修改，最终把DOM转化为JSON字符串
```c++
#include "TinyJSON/Document.h"
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
```
还可以不使用DOM存储输入的JSON，避免内存占用大的问题。
如下面的例子所示，使用ReadStream读取JSON，使用Reader解析JSON，使用Writer输出JSON。
使用自定义类AddOne将Reader解析后传递过来的数字加1。
```c++
#include "sample.h"

#include "TinyJSON/Writer.h"
#include "TinyJSON/Reader.h"
#include "TinyJSON/ReadStream.h"
#include "TinyJSON/WriteStream.h"

#include <iostream>

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
        std::cout << parseErrorStr(err) << std::endl;
    }
}
```
如果想直接生成JSON，如下面例子所示
```c++
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
```
输出结果：
```
[Infinity,NaN,true,"\u0000蛤蛤蛤\u0000",true]
```
## 安装
```shell
# 编译器必须支持c++20
git clone git@github.com:TripleLin2020/TinyJSON.git
cd TinyJSON
git submodule update --init --recursive
./build.sh
./build.sh install
./build.sh test
```
## 参考
+ [JSON tutorial](https://github.com/miloyip/json-tutorial): 从零开始的 JSON 库教程.
+ [RapidJSON](https://github.com/Tencent/rapidjson): A fast JSON parser/generator for C++ with both SAX/DOM style API.
+ [jackson](https://github.com/guangqianpeng/jackson#jackson-a-simple-and-fast-json-parsergenerator): A simple and fast JSON parser/generator