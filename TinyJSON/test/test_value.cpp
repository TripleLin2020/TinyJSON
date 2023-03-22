#include <TinyJSON/Document.h>
#include <gtest/gtest.h>
#include <string_view>

using namespace json;
using namespace std::string_view_literals;

#define TEST_BOOL(type, value, json)      \
    do {                                  \
        Document doc;                     \
        ParseError err = doc.parse(json); \
        EXPECT_EQ(err, PARSE_OK);         \
        EXPECT_EQ(doc.getType(), type);   \
        EXPECT_EQ(doc.getData<bool>(), value);  \
    } while (false)

#define TEST_NULL(json)                      \
    do {                                     \
        Document doc;                        \
        ParseError err = doc.parse(json);    \
        EXPECT_EQ(err, PARSE_OK);            \
        EXPECT_EQ(doc.getType(), TYPE_NULL); \
    } while (false)

#define TEST_DOUBLE(num, json)                        \
    do {                                              \
        Document doc;                                 \
        ParseError err = doc.parse(json);             \
        EXPECT_EQ(err, PARSE_OK);                     \
        EXPECT_EQ(doc.getType(), TYPE_DOUBLE);        \
        if (std::isnan(num)) {                        \
            EXPECT_TRUE(std::isnan(doc.getData<double>())); \
        } else {                                      \
            EXPECT_EQ(doc.getData<double>(), num);          \
        }                                             \
    } while (false)

#define TEST_INT32(num, json)                 \
    do {                                      \
        Document doc;                         \
        ParseError err = doc.parse(json);     \
        EXPECT_EQ(err, PARSE_OK);             \
        EXPECT_EQ(doc.getType(), TYPE_INT32); \
        EXPECT_EQ(doc.getData<int32_t>(), num);       \
    } while (false)

#define TEST_INT64(num, json)                 \
    do {                                      \
        Document doc;                         \
        ParseError err = doc.parse(json);     \
        EXPECT_EQ(err, PARSE_OK);             \
        EXPECT_EQ(doc.getType(), TYPE_INT64); \
        EXPECT_EQ(doc.getData<int64_t>(), num);       \
    } while (false)

#define TEST_STRING(str, json)                 \
    do {                                       \
        Document doc;                          \
        ParseError err = doc.parse(json);      \
        EXPECT_EQ(err, PARSE_OK);              \
        EXPECT_EQ(doc.getType(), TYPE_STRING_PTR); \
        EXPECT_EQ(*doc.getData<StringPtr>(), str);       \
    } while (false)

TEST(json_value, null) {
    TEST_NULL("null");
    TEST_NULL(" null ");
}

TEST(json_value, bool_) {
    TEST_BOOL(TYPE_BOOL, true, "true");
    TEST_BOOL(TYPE_BOOL, true, "\r\ntrue ");
    TEST_BOOL(TYPE_BOOL, false, " false");
    TEST_BOOL(TYPE_BOOL, false, "false ");
}

TEST(json_value, number) {
    TEST_DOUBLE(0.0, "0.0");
    TEST_DOUBLE(0.0, "-0.0e000");
    TEST_DOUBLE(876.543e21, "876.543e+00021");
    TEST_DOUBLE(-111.000e-010, "-111.000e-010");
    /* https://en.wikipedia.org/wiki/Double-precision_floating-point_format */
    TEST_DOUBLE(1.0000000000000002, "1.0000000000000002");
    TEST_DOUBLE(-1.0000000000000002, "-1.0000000000000002");
    TEST_DOUBLE(4.9406564584124654e-323, "4.9406564584124654E-323");
    TEST_DOUBLE(4.9406564584124654e-324, "4.9406564584124654e-324");
    TEST_DOUBLE(2.2250738585072009e-308, "2.2250738585072009e-308");
    TEST_DOUBLE(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    TEST_DOUBLE(2.2250738585072014e-308, "2.2250738585072014e-308");
    TEST_DOUBLE(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    TEST_DOUBLE(1.7976931348623157e308, "1.7976931348623157e308");
    TEST_DOUBLE(-1.7976931348623157e308, "-1.7976931348623157e308");
    TEST_DOUBLE(INFINITY, "Infinity");
    TEST_DOUBLE(NAN, "NaN");

    TEST_INT32(0, "0");
    TEST_INT32(-0, "0");
    TEST_INT32(1, "1");
    TEST_INT32(-1, "-1");
    TEST_INT32(2147483647, "2147483647");
    TEST_INT32(-2147483648, "-2147483648");
    TEST_INT32(0, "0i32");
    TEST_INT32(-0, "0i32");
    TEST_INT32(1, "1i32");
    TEST_INT32(-1, "-1i32");
    TEST_INT32(2147483647, "2147483647i32");
    TEST_INT32(-2147483648, "-2147483648i32");

    TEST_INT64(2147483648LL, "2147483648");
    TEST_INT64(-2147483649LL, "-2147483649");
    TEST_INT64(std::numeric_limits<int64_t>::max(), "9223372036854775807");
    TEST_INT64(std::numeric_limits<int64_t>::min(), "-9223372036854775808");

    TEST_INT64(2147483648LL, "2147483648i64");
    TEST_INT64(-2147483649LL, "-2147483649i64");
    TEST_INT64(std::numeric_limits<int64_t>::max(), "9223372036854775807i64");
    TEST_INT64(std::numeric_limits<int64_t>::min(), "-9223372036854775808i64");

    TEST_INT64(0, "0i64");
    TEST_INT64(-0, "0i64");
    TEST_INT64(1, "1i64");
    TEST_INT64(-1, "-1i64");
    TEST_INT64(2147483647, "2147483647i64");
    TEST_INT64(-2147483648, "-2147483648i64");
}

TEST(json_value, string_) {
    TEST_STRING("", "\"\"");
    TEST_STRING("abcd", "\"abcd\"");
    TEST_STRING("\\", "\"\\\\\"");
    TEST_STRING("\"", "\"\\\"\"");
    TEST_STRING("/\b\f\n\r\t", "\"\\/\\b\\f\\n\\r\\t\"");
    TEST_STRING("蛤\0蛤\0蛤"sv, "\"蛤\\u0000蛤\\u0000蛤\"");
    TEST_STRING("蛤\0蛤\0蛤"sv, "\"蛤\0蛤\0蛤\""sv);
    TEST_STRING("\0"sv, "\"\\u0000\"");
    TEST_STRING("\x24", "\"\\u0024\"");                    /* Dollar $ */
    TEST_STRING("\xC2\xA2", "\"\\u00A2\"");                /* Cents ¢ */
    TEST_STRING("\xE2\x82\xAC", "\"\\u20AC\"");            /* Euro € */
    TEST_STRING("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\""); /* G clef  𝄞 */
    TEST_STRING("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\""); /* G clef  𝄞 */
}

TEST(json_value, array) {
    ParseError err;
    {
        Document doc;
        err = doc.parse("[]");
        EXPECT_EQ(err, PARSE_OK);
        EXPECT_EQ(doc.getType(), TYPE_ARRAY_PTR);
        EXPECT_TRUE(doc.getData<ArrayPtr>()->empty());
    }
    {
        Document doc;
        err = doc.parse("[[]]");
        EXPECT_EQ(err, PARSE_OK);
        EXPECT_EQ(doc.getType(), TYPE_ARRAY_PTR);

        auto arrPtr = doc.getData<ArrayPtr>();
        EXPECT_EQ(arrPtr->size(), 1);
        EXPECT_EQ((*arrPtr)[0].getType(), TYPE_ARRAY_PTR);
        EXPECT_TRUE((*arrPtr)[0].getData<ArrayPtr>()->empty());
    }
    {
        Document doc;
        err = doc.parse("[0, 1, 2, 3, 4]");
        EXPECT_EQ(err, PARSE_OK);
        EXPECT_EQ(doc.getType(), TYPE_ARRAY_PTR);

        auto arrPtr = doc.getData<ArrayPtr>();

        EXPECT_EQ(5, arrPtr->size());
        for (size_t i = 0; i < arrPtr->size(); i++) {
            EXPECT_EQ((*arrPtr)[i].getType(), TYPE_INT32);
            EXPECT_EQ((*arrPtr)[i].getData<int32_t>(), i);
        }
    }
    {
        Document doc;
        err = doc.parse("[ { } , { } , { } , { } , { } ]");
        EXPECT_EQ(err, PARSE_OK);
        EXPECT_EQ(doc.getType(), TYPE_ARRAY_PTR);

        auto arrPtr = doc.getData<ArrayPtr>();

        EXPECT_EQ(5, arrPtr->size());
        for (size_t i = 0; i < arrPtr->size(); i++) {
            EXPECT_EQ((*arrPtr)[i].getType(), TYPE_OBJECT_PTR);
            EXPECT_EQ((*arrPtr)[i].getData<ObjectPtr>()->size(), 0);
        }
    }
    {
        Document doc;
        err = doc.parse("[\"hehe\", true, false, null, 0.0]");
        EXPECT_EQ(err, PARSE_OK);
        EXPECT_EQ(doc.getType(), TYPE_ARRAY_PTR);

        auto arrPtr = doc.getData<ArrayPtr>();
        EXPECT_EQ(5, arrPtr->size());
        EXPECT_EQ((*arrPtr)[0].getType(), TYPE_STRING_PTR);
        EXPECT_EQ((*arrPtr)[1].getType(), TYPE_BOOL);
        EXPECT_EQ((*arrPtr)[2].getType(), TYPE_BOOL);
        EXPECT_EQ((*arrPtr)[3].getType(), TYPE_NULL);
        EXPECT_EQ((*arrPtr)[4].getType(), TYPE_DOUBLE);

        EXPECT_TRUE(*(*arrPtr)[0].getData<StringPtr>() == "hehe");
        EXPECT_EQ((*arrPtr)[4].getData<double>(), 0.0);
    }
}

TEST(json_value, object) {
    Document doc;
    ParseError err = doc.parse(
            " { "
            "\"n\" : null , "
            "\"f\" : false , "
            "\"t\" : true , "
            "\"i\" : 123 , "
            "\"s\" : \"abc\", "
            "\"a\" : [ 1, 2, 3 ],"
            "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
            " } ");

    EXPECT_EQ(err, PARSE_OK);
    EXPECT_EQ(doc.getType(), TYPE_OBJECT_PTR);

    EXPECT_EQ(doc.getData<ObjectPtr>()->size(), 7);
    EXPECT_EQ(doc["n"].getType(), TYPE_NULL);
    EXPECT_EQ(doc["f"].getType(), TYPE_BOOL);
    EXPECT_EQ(doc["t"].getType(), TYPE_BOOL);
    EXPECT_EQ(doc["i"].getType(), TYPE_INT32);
    EXPECT_EQ(doc["s"].getType(), TYPE_STRING_PTR);
    EXPECT_EQ(doc["a"].getType(), TYPE_ARRAY_PTR);
    EXPECT_EQ(doc["o"].getType(), TYPE_OBJECT_PTR);

    EXPECT_EQ(doc["i"].getData<int32_t>(), 123);
    EXPECT_EQ(*doc["s"].getData<StringPtr>(), "abc");

    auto arrPtr = doc["a"].getData<ArrayPtr>();
    EXPECT_EQ(arrPtr->size(), 3);
    for (size_t i = 0; i < 3; i++) {
        EXPECT_EQ((*arrPtr)[i].getType(), TYPE_INT32);
        EXPECT_EQ((*arrPtr)[i].getData<int32_t>(), i + 1);
    }

    auto& obj = doc["o"];
    EXPECT_EQ(obj["1"].getData<int32_t>(), 1);
    EXPECT_EQ(obj["2"].getData<int32_t>(), 2);
    EXPECT_EQ(obj["3"].getData<int32_t>(), 3);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}