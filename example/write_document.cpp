#include "Document.h"
#include "Writer.h"
#include "WriteStream.h"

#include <iostream>

using namespace json;

int main() {
    Document document;
    ParseError err = document.parse(R"(
{
    "precision": "zip",
    "Latitude": 37.766800000000003,
    "Longitude": -122.3959,
    "Address": "tiny",
    "City": "SAN FRANCISCO",
    "State": "CA",
    "Zip": "94107",
    "Country": "US",
    "Number": 0i32,
    "Number2": 0i64
}
)");

    if (err != PARSE_OK) {
        puts(parseErrorStr(err));
        return 1;
    }

    // 获取Key"State"对应的Value
    Value& state = document["State"];
    std::cout << "State: " << state << '\n';

    // 获取Key"Zip"对应的Value
    Value& zip = document["Zip"];
    std::cout << "Zip: " << zip << "\n";

    zip = 9527;
    std::cout << "Zip: " << zip << "\n";

    document.addPair("123", "456");
    std::cout << "123: " << document["123"] << "\n";
}