#ifndef JSON_NONCOPYABLE_H
#define JSON_NONCOPYABLE_H

namespace json
{

class noncopyable
{
public:
    noncopyable(const noncopyable&) = delete;
    void operator=(const noncopyable&) = delete;
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

}

#endif //JSON_NONCOPYABLE_H
