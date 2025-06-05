#include <gtest/gtest.h>
#include <vector>
#include <map>
#include <string>
#include "lib/lazy_adapters.h"

// Helper to collect range into vector
template<typename Range>
auto to_vector(Range&& range) {
    using ValueType = std::decay_t<decltype(*std::begin(range))>;
    std::vector<ValueType> result;
    for (auto&& x : range) result.push_back(x);
    return result;
}

TEST(FilterTest, FiltersOddNumbers) {
    std::vector<int> v{1,2,3,4,5,6};
    auto result = to_vector(v | filter([](int i){ return i % 2 != 0; }));
    std::vector<int> expected{1,3,5};
    EXPECT_EQ(result, expected);
}

TEST(TransformTest, SquaresElements) {
    std::vector<int> v{1,2,3};
    auto result = to_vector(v | transform([](int i){ return i * i; }));
    std::vector<int> expected{1,4,9};
    EXPECT_EQ(result, expected);
}

TEST(TakeTest, TakesFirstN) {
    std::vector<int> v{10,20,30,40,50};
    auto result = to_vector(v | take(3));
    std::vector<int> expected{10,20,30};
    EXPECT_EQ(result, expected);
}

TEST(DropTest, DropsFirstN) {
    std::vector<int> v{10,20,30,40,50};
    auto result = to_vector(v | drop(2));
    std::vector<int> expected{30,40,50};
    EXPECT_EQ(result, expected);
}

TEST(ReverseTest, ReversesSequence) {
    std::vector<int> v{1,2,3,4};
    auto result = to_vector(v | reverse());
    std::vector<int> expected{4,3,2,1};
    EXPECT_EQ(result, expected);
}

TEST(KeysTest, ExtractsMapKeys) {
    std::map<int, std::string> m{{1, "a"}, {2, "b"}, {3, "c"}};
    auto result = to_vector(m | keys());
    std::vector<int> expected{1,2,3};
    EXPECT_EQ(result, expected);
}

TEST(ValuesTest, ExtractsMapValues) {
    std::map<int, std::string> m{{1, "a"}, {2, "b"}, {3, "c"}};
    auto result = to_vector(m | values());
    std::vector<std::string> expected{"a","b","c"};
    EXPECT_EQ(result, expected);
}

TEST(PipelineTest, FilterThenTransform) {
    std::vector<int> v{1,2,3,4,5};
    auto pipeline = v | filter([](int i){ return i % 2 != 0; })
                       | transform([](int i){ return i + 1; });
    auto result = to_vector(pipeline);
    std::vector<int> expected{2,4,6};
    EXPECT_EQ(result, expected);
}