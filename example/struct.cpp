#include "Value.h"
#include "Document.h"
#include "WriteStream.h"
#include "Writer.h"
#include "WriteStream.h"

#include <string>

using namespace json;

struct Person
{
    explicit Person(const Value& value) {
        switch (value.getType()) {
            case TYPE_OBJECT_PTR:
                name = *value["name"].getData<StringPtr>();
                age = value["age"].getData<int32_t>();
                break;
            case TYPE_ARRAY_PTR:
                name = *value[0].getData<StringPtr>();
                age = value[1].getData<int32_t>();
                break;
            default:
                assert(false && "bad type");
                break;
        }
    }

    Value toJsonObject() const {
        Value value = Value::emptyObject();
        value.addPair("name", name);
        value.addPair("age", age);
        return value;
    }

    Value toJsonArray() const {
        Value value = Value::emptyArray();
        value.addToArray(name);
        value.addToArray(age);
        return value;
    }

    std::string name;
    int32_t age;
};

int main() {
    Value value = Value::emptyObject();
    value.addPair("name", "123");
    value.addPair("age", 123);

    Person p(value);

    value = p.toJsonArray();
    FileWriteStream os(stdout);
    Writer writer(os);
    value.writeTo(writer);
}