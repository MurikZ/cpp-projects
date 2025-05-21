#include "../lib/parser.h"
#include <gtest/gtest.h>
#include <sstream>

using namespace confx;

TEST(ParserTestSuite, EmptyInput) {
    Configuration root = Load("");
    ASSERT_TRUE(root.Valid());
}

TEST(ParserTestSuite, IntegerParsing) {
    std::string data = R"(
        key1 = 100500
        key2 = -22
        key3 = +28)";
    Configuration root = Load(data);
    ASSERT_TRUE(root.Valid());
    EXPECT_TRUE(root.Find("key1").IsInteger());
    EXPECT_TRUE(root.Find("key2").IsInteger());
    EXPECT_TRUE(root.Find("key3").IsInteger());
    EXPECT_EQ(root.Find("key1").ToInteger(), 100500);
    EXPECT_EQ(root.Find("key2").ToInteger(), -22);
    EXPECT_EQ(root.Find("key3").ToInteger(), 28);
}

TEST(ParserTestSuite, InvalidIntegerAccess) {
    std::string data = R"(
        key1 = true
        key2 = -22.1
        key3 = "ITMO")";
    Configuration root = Load(data);
    ASSERT_TRUE(root.Valid());
    EXPECT_FALSE(root.Find("key1").IsInteger());
    EXPECT_FALSE(root.Find("key2").IsInteger());
    EXPECT_FALSE(root.Find("key3").IsInteger());
    EXPECT_THROW(root.Find("key1").ToInteger(), std::runtime_error);
    EXPECT_THROW(root.Find("key2").ToInteger(), std::runtime_error);
    EXPECT_THROW(root.Find("key3").ToInteger(), std::runtime_error);
}

TEST(ParserTestSuite, RealParsing) {
    std::string data = R"(
        key1 = 2.1
        key2 = -3.14
        key3 = -0.001)";
    Configuration root = Load(data);
    ASSERT_TRUE(root.Valid());
    EXPECT_TRUE(root.Find("key1").IsReal());
    EXPECT_TRUE(root.Find("key2").IsReal());
    EXPECT_TRUE(root.Find("key3").IsReal());
    EXPECT_FLOAT_EQ(root.Find("key1").ToReal(), 2.1f);
    EXPECT_FLOAT_EQ(root.Find("key2").ToReal(), -3.14f);
    EXPECT_FLOAT_EQ(root.Find("key3").ToReal(), -0.001f);
}

TEST(ParserTestSuite, InvalidRealAccess) {
    std::string data = R"(
        key1 = true
        key2 = -2
        key3 = "ITMO")";
    Configuration root = Load(data);
    ASSERT_TRUE(root.Valid());
    EXPECT_FALSE(root.Find("key1").IsReal());
    EXPECT_FALSE(root.Find("key2").IsReal());
    EXPECT_FALSE(root.Find("key3").IsReal());
    EXPECT_THROW(root.Find("key1").ToReal(), std::runtime_error);
    EXPECT_THROW(root.Find("key2").ToReal(), std::runtime_error);
    EXPECT_THROW(root.Find("key3").ToReal(), std::runtime_error);
}

TEST(ParserTestSuite, TextParsing) {
    std::string data = R"(
        key = "value"
        key1 = "value1")";
    Configuration root = Load(data);
    ASSERT_TRUE(root.Valid());
    EXPECT_TRUE(root.Find("key").IsText());
    EXPECT_TRUE(root.Find("key1").IsText());
    EXPECT_EQ(root.Find("key").ToString(), "value");
    EXPECT_EQ(root.Find("key1").ToString(), "value1");
}

TEST(ParserTestSuite, InvalidTextAccess) {
    std::string data = R"(
        key = true
        key1 = [ "1", "2", "3" ])";
    Configuration root = Load(data);
    ASSERT_TRUE(root.Valid());
    EXPECT_FALSE(root.Find("key").IsText());
    EXPECT_FALSE(root.Find("key1").IsText());
    EXPECT_THROW(root.Find("key").ToString(), std::runtime_error);
    EXPECT_THROW(root.Find("key1").ToString(), std::runtime_error);
}

TEST(ParserTestSuite, ListParsing) {
    std::string data = R"(
        key1 = [1, 2, 3, 4, 5, 6])";
    Configuration root = Load(data);
    ASSERT_TRUE(root.Valid());
    EXPECT_TRUE(root.Find("key1").IsList());
    const auto& arr = root.Find("key1").ToList();
    EXPECT_EQ(arr.at(0).ToInteger(), 1);
    EXPECT_EQ(arr.at(1).ToInteger(), 2);
    EXPECT_EQ(arr.at(2).ToInteger(), 3);
    EXPECT_THROW(arr.at(100500).ToInteger(), std::out_of_range);
}

TEST(ParserTestSuite, MixedTypeList) {
    std::string data = R"(
        key1 = [1, true, 3.14, "ITMO", [1, 2, 3], ["a", "b", 28]])";
    Configuration root = Load(data);
    ASSERT_TRUE(root.Valid());
    const auto& arr = root.Find("key1").ToList();
    EXPECT_TRUE(arr[0].IsInteger()); EXPECT_EQ(arr[0].ToInteger(), 1);
    EXPECT_TRUE(arr[1].IsBoolean()); EXPECT_EQ(arr[1].ToBoolean(), true);
    EXPECT_TRUE(arr[2].IsReal());    EXPECT_FLOAT_EQ(arr[2].ToReal(), 3.14f);
    EXPECT_TRUE(arr[3].IsText());    EXPECT_EQ(arr[3].ToString(), "ITMO");
    const auto& sub1 = arr[4].ToList();
    EXPECT_EQ(sub1[0].ToInteger(), 1);
    const auto& sub2 = arr[5].ToList();
    EXPECT_EQ(sub2[0].ToString(), "a");
}

TEST(ParserTestSuite, CommentHandling) {
    std::string data = R"(
        key1 = 100500  # some important value

        # end of config)";
    Configuration root = Load(data);
    ASSERT_TRUE(root.Valid());
    EXPECT_EQ(root.Find("key1").ToInteger(), 100500);
}

TEST(ParserTestSuite, SectionBasic) {
    std::string data = R"(
        [section1]
        key1 = 1
        key2 = true

        [section1]
        key3 = "value")";
    Configuration root = Load(data);
    ASSERT_TRUE(root.Valid());
    const Node& sec = root.root_.Find("section1").AsNode();
    EXPECT_EQ(sec.Find("key1").ToInteger(), 1);
    EXPECT_TRUE(sec.Find("key2").IsBoolean());
    EXPECT_EQ(sec.Find("key3").ToString(), "value");
}

TEST(ParserTestSuite, NestedSections) {
    std::string data = R"(
        [level1]
        key1 = 1
        [level1.level2-1]
        key2 = 2

        [level1.level2-2]
        key3 = 3)";
    Configuration root = Load(data);
    ASSERT_TRUE(root.Valid());
    const Node& lvl1 = root.root_.Find("level1").AsNode();
    EXPECT_EQ(lvl1.Find("key1").ToInteger(), 1);
    EXPECT_EQ(lvl1.Find("level2-1").AsNode().Find("key2").ToInteger(), 2);
    EXPECT_EQ(lvl1.Find("level2-2").AsNode().Find("key3").ToInteger(), 3);
}

TEST(ParserTestSuite, DeepNestedSection) {
    std::string data = R"(
        [a.b.c.d]
        key1 = 1)";
    Configuration root = Load(data);
    ASSERT_TRUE(root.Valid());
    const Node& deep = root.root_.Find("a").AsNode()
                            .Find("b").AsNode()
                            .Find("c").AsNode()
                            .Find("d").AsNode();
    EXPECT_EQ(deep.Find("key1").ToInteger(), 1);
}
