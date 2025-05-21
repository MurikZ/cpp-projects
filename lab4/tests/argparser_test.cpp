#include <./lib/ArgParser.h>
#include <gtest/gtest.h>
#include <sstream>
#include <iterator>
#include <vector>

using namespace CommandLine;

static std::vector<std::string> SplitString(const std::string& str) {
    std::istringstream iss(str);
    return { std::istream_iterator<std::string>(iss),
             std::istream_iterator<std::string>() };
}

TEST(CmdParserTestSuite, EmptyTest) {
    CmdParser p("Empty");
    ASSERT_TRUE(p.Parse(SplitString("app")));
}

TEST(CmdParserTestSuite, StringTest) {
    CmdParser p("Str");
    p.AddStringParam("param1");
    ASSERT_TRUE(p.Parse(SplitString("app --param1=value1")));
    ASSERT_EQ(p.FetchString("param1"), "value1");
}

TEST(CmdParserTestSuite, ShortNameTest) {
    CmdParser p("Short");
    p.AddStringParam('p', "param1");
    ASSERT_TRUE(p.Parse(SplitString("app -p=value1")));
    ASSERT_EQ(p.FetchString("param1"), "value1");
}

TEST(CmdParserTestSuite, DefaultTest) {
    CmdParser p("Def");
    p.AddStringParam("param1").WithDefault("value1");
    ASSERT_TRUE(p.Parse(SplitString("app --param1=value1")));
    ASSERT_EQ(p.FetchString("param1"), "value1");
}

TEST(CmdParserTestSuite, NoDefaultTest) {
    CmdParser p("NoDef");
    p.AddStringParam("param1");
    ASSERT_FALSE(p.Parse(SplitString("app")));
}

TEST(CmdParserTestSuite, StoreValueTest) {
    CmdParser p("Store");
    std::string v;
    p.AddStringParam("param1").SaveValue(v);
    ASSERT_TRUE(p.Parse(SplitString("app --param1=value1")));
    ASSERT_EQ(v, "value1");
}

TEST(CmdParserTestSuite, MultiStringTest) {
    CmdParser p("MultiStr");
    std::string v;
    p.AddStringParam("param1").SaveValue(v);
    p.AddStringParam('a', "param2");
    ASSERT_TRUE(p.Parse(SplitString("app --param1=value1 --param2=value2")));
    ASSERT_EQ(p.FetchString("param2"), "value2");
}

TEST(CmdParserTestSuite, IntTest) {
    CmdParser p("Int");
    p.AddIntParam("param1");
    ASSERT_TRUE(p.Parse(SplitString("app --param1=100500")));
    ASSERT_EQ(p.FetchInt("param1"), 100500);
}

TEST(CmdParserTestSuite, MultiValueTest) {
    CmdParser p("MultiInt");
    std::vector<int> iv;
    p.AddIntParam('p', "param1").Multiple().SaveValues(iv);
    ASSERT_TRUE(p.Parse(SplitString("app --param1=1 --param1=2 --param1=3")));
    ASSERT_EQ(p.FetchInt("param1", 0), 1);
    ASSERT_EQ(iv[1], 2);
    ASSERT_EQ(iv[2], 3);
}

TEST(CmdParserTestSuite, MinCountMultiValueTest) {
    CmdParser p("MinCountInt");
    std::vector<int> iv;
    const size_t Min = 10;
    p.AddIntParam('p', "param1").Multiple(Min).SaveValues(iv);
    ASSERT_FALSE(p.Parse(SplitString("app --param1=1 --param1=2 --param1=3")));
}

TEST(CmdParserTestSuite, FlagTest) {
    CmdParser p("Flag");
    p.AddFlagParam('f', "flag1");
    ASSERT_TRUE(p.Parse(SplitString("app --flag1")));
    ASSERT_TRUE(p.FetchFlag("flag1"));
}

TEST(CmdParserTestSuite, FlagsTest) {
    CmdParser p("Flags");
    bool f3;
    p.AddFlagParam('a', "flag1");
    p.AddFlagParam('b', "flag2").WithDefault(true);
    p.AddFlagParam('c', "flag3").SaveValue(f3);
    ASSERT_TRUE(p.Parse(SplitString("app -ac")));
    ASSERT_TRUE(p.FetchFlag("flag1"));
    ASSERT_TRUE(p.FetchFlag("flag2"));
    ASSERT_TRUE(f3);
}

TEST(CmdParserTestSuite, PositionalArgTest) {
    CmdParser p("Pos");
    std::vector<int> vals;
    p.AddIntParam("Param1").Multiple(1).PositionalParam().SaveValues(vals);
    ASSERT_TRUE(p.Parse(SplitString("app 1 2 3 4 5")));
    ASSERT_EQ(vals[0], 1);
    ASSERT_EQ(vals[2], 3);
    ASSERT_EQ(vals.size(), 5);
}

TEST(CmdParserTestSuite, HelpTest) {
    CmdParser p("Help");
    p.AddHelpParam('h', "help", "Some Description");
    ASSERT_TRUE(p.Parse(SplitString("app --help")));
    ASSERT_TRUE(p.ShowHelp());
}

// Test for Usage string is omitted as its exact formatting is implementation-defined
