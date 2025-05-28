#include "../lib/parser.h"
#include <gtest/gtest.h>
#include <sstream>
#include <iterator>
#include <vector>

using namespace confx;

class ValidFormatTestSuite : public testing::TestWithParam<const char*> {};
class InvalidFormatTestSuite : public testing::TestWithParam<const char*> {};

TEST_P(ValidFormatTestSuite, AcceptsValidFormats) {
    const char* param = GetParam();
    Configuration cfg = Load(param);
    ASSERT_TRUE(cfg.Valid());
}

TEST_P(InvalidFormatTestSuite, RejectsInvalidFormats) {
    const char* param = GetParam();
    Configuration cfg = Load(param);
    ASSERT_FALSE(cfg.Valid());
}

// Key syntax
INSTANTIATE_TEST_SUITE_P(
    KeyValidation,
    ValidFormatTestSuite,
    testing::Values(
        "key = \"value\"",
        " key      =  \"value\"   ",
        " key=  \"value\"",
        " key123 =  \"value\"",
        " key_1-23-abcd =  \"value\"",
        " 23key_ =  \"value\""
    )
);
INSTANTIATE_TEST_SUITE_P(
    KeyValidation,
    InvalidFormatTestSuite,
    testing::Values(
        "key = \n\"value\"",
        "      =  \"value\"   ",
        "=  \"value\""
    )
);

// Value presence
INSTANTIATE_TEST_SUITE_P(
    ValueValidation,
    ValidFormatTestSuite,
    testing::Values(
        "key = \"value\""
    )
);
INSTANTIATE_TEST_SUITE_P(
    ValueValidation,
    InvalidFormatTestSuite,
    testing::Values(
        "key = ",
        "key = abcd"
    )
);

TEST(FormatTestSuite, DuplicateKeyFails) {
    std::string data = R"(
        key = 1
        key = 2)";
    Configuration cfg = Load(data);
    ASSERT_FALSE(cfg.Valid());
}

// Integer value syntax
INSTANTIATE_TEST_SUITE_P(
    IntValueValidation,
    ValidFormatTestSuite,
    testing::Values(
        "key1 = 2",
        "key2 = -22",
        "key3 = +48",
        "key4 = 2-2",
        "key5 = 4+8"
    )
);
INSTANTIATE_TEST_SUITE_P(
    IntValueValidation,
    InvalidFormatTestSuite,
    testing::Values(
        "key1 = -+",
        "key2 = +"
    )
);

// Real (float) value syntax
INSTANTIATE_TEST_SUITE_P(
    RealValueValidation,
    ValidFormatTestSuite,
    testing::Values(
        "key1 = 3.14",
        "key2 = -3.14",
        "key3 = +0.00001"
    )
);
INSTANTIATE_TEST_SUITE_P(
    RealValueValidation,
    InvalidFormatTestSuite,
    testing::Values(
        "key1 = +.",
        "key2 = ."
    )
);

// Text (string) value syntax
INSTANTIATE_TEST_SUITE_P(
    TextValueValidation,
    ValidFormatTestSuite,
    testing::Values(
        "key1 = \"Hello world\"",
        "key2 = \"1, 2, 3, 4, 5\"",
        "key3 = \"3.14\"",
        "key4 = \"1\t2\t3\"",
        "key5 = \"[1,2,3,4,5]\""        
    )
);
INSTANTIATE_TEST_SUITE_P(
    TextValueValidation,
    InvalidFormatTestSuite,
    testing::Values(
        "key1 = \"Hello world",
        "key2 = \"Bjarne\" \"stroustrup"
    )
);

// Boolean value syntax
INSTANTIATE_TEST_SUITE_P(
    BooleanValueValidation,
    ValidFormatTestSuite,
    testing::Values(
        "key1 = true",
        "key2 = false"
    )
);
INSTANTIATE_TEST_SUITE_P(
    BooleanValueValidation,
    InvalidFormatTestSuite,
    testing::Values(
        "key1 = tru",
        "key2 = alse",
        "key3 = true true",
        "key4 = fal se",
        "key5 = truefalse"
    )
);

// List (array) syntax
INSTANTIATE_TEST_SUITE_P(
    ListValueValidation,
    ValidFormatTestSuite,
    testing::Values(
        "key1 = []",
        "key2 = [1,2,3,4,5]",
        "key3 = [1, -3.14, true, \"ITMO\"]",
        "key4 = [[1,2],[2,[3,4,5]]]"
    )
);
INSTANTIATE_TEST_SUITE_P(
    ListValueValidation,
    InvalidFormatTestSuite,
    testing::Values(
        "key1 = [",
        "key2 = ]",
        "key3 = [1,2,3,4",
        "key4 = [[1,2],[2,[3,4,5]]"
    )
);

// Section headings
INSTANTIATE_TEST_SUITE_P(
    SectionValueValidation,
    ValidFormatTestSuite,
    testing::Values(
        "[section-1]",
        "[section-1.section-2]",
        "[a.b.c.d]"
    )
);
INSTANTIATE_TEST_SUITE_P(
    SectionValueValidation,
    InvalidFormatTestSuite,
    testing::Values(
        "[}",
        "{section-1}",
        "[section-1.}",
        "[.section-1}"
    )
);

// Comments
INSTANTIATE_TEST_SUITE_P(
    CommentValueValidation,
    ValidFormatTestSuite,
    testing::Values(
        "key1 = 1 # comment",
        "# some text"
    )
);
INSTANTIATE_TEST_SUITE_P(
    CommentValueValidation,
    InvalidFormatTestSuite,
    testing::Values(
        "# comment \n newline comment"
    )
);

TEST(ExtraFormatTests, EmptyDocumentIsValid) {
    Configuration cfg = Load("");
    ASSERT_TRUE(cfg.Valid());
}

TEST(ExtraFormatTests, OnlyCommentIsValid) {
    Configuration cfg = Load("# configuration header");
    ASSERT_TRUE(cfg.Valid());
}

TEST(ExtraFormatTests, ZeroIntegerIsValid) {
    Configuration cfg = Load("key0 = 0");
    ASSERT_TRUE(cfg.Valid());
    EXPECT_TRUE(cfg.Find("key0").IsInteger());
    EXPECT_EQ(cfg.Find("key0").ToInteger(), 0);
}

TEST(ExtraFormatTests, PositiveRealIsValid) {
    Configuration cfg = Load("pi = 0.1");
    ASSERT_TRUE(cfg.Valid());
    EXPECT_TRUE(cfg.Find("pi").IsReal());
    EXPECT_FLOAT_EQ(cfg.Find("pi").ToReal(), 0.1f);
}

TEST(ExtraFormatTests, NestedEmptyList) {
    Configuration cfg = Load("arr = [[]]");
    ASSERT_TRUE(cfg.Valid());
    const auto& outer = cfg.Find("arr").ToList();
    ASSERT_EQ(outer.size(), 1u);
    EXPECT_TRUE(outer[0].IsList());
    EXPECT_TRUE(outer[0].ToList().empty());
}

TEST(ExtraFormatTests, HashInsideText) {
    Configuration cfg = Load("s = \"#notacomment\"");
    ASSERT_TRUE(cfg.Valid());
    EXPECT_EQ(cfg.Find("s").ToString(), "#notacomment");
}

TEST(ExtraFormatTests, EmptyListTest) {
    Configuration cfg = Load("a = []");
    ASSERT_TRUE(cfg.Valid());
    EXPECT_TRUE(cfg.Find("a").IsList());
    EXPECT_TRUE(cfg.Find("a").ToList().empty());
}

TEST(ExtraFormatTests, MultipleComments) {
    Configuration cfg = Load("# first comment # second comment # third comment");
    ASSERT_TRUE(cfg.Valid());
}

TEST(ExtraFormatTests, NumericTextValue) {
    Configuration cfg = Load("num = \"12345\"");
    ASSERT_TRUE(cfg.Valid());
    EXPECT_TRUE(cfg.Find("num").IsText());
    EXPECT_EQ(cfg.Find("num").ToString(), "12345");
}

TEST(ExtraFormatTests, SectionThenKeyTest) {
    Configuration cfg = Load("[sec] sub = 5");
    ASSERT_TRUE(cfg.Valid());
}
