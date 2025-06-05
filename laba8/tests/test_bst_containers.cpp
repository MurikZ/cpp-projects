#include <gtest/gtest.h>
#include "lib/BST_containers.h"
#include <vector>
#include <algorithm>

using namespace bst;

template <typename It>
std::vector<typename It::value_type> collect(It begin, It end) {
    std::vector<typename It::value_type> result;
    for (auto it = begin; it != end; ++it) {
        result.push_back(*it);
    }
    return result;
}

TEST(BinarySearchTreeTest, EmptyTree) {
    BinarySearchTree<int> bst;
    EXPECT_TRUE(bst.empty());
    EXPECT_EQ(bst.size(), 0);
    EXPECT_EQ(collect(bst.begin(), bst.end()), std::vector<int>{});
}

TEST(BinarySearchTreeTest, InsertAndFind) {
    BinarySearchTree<int> bst;
    auto r1 = bst.insert(5);
    EXPECT_TRUE(r1.second);
    auto r2 = bst.insert(3);
    EXPECT_TRUE(r2.second);
    auto r3 = bst.insert(7);
    EXPECT_TRUE(r3.second);
    auto r4 = bst.insert(5);
    EXPECT_FALSE(r4.second);

    EXPECT_NE(bst.find(3), bst.end());
    EXPECT_EQ(bst.find(42), bst.end());
    EXPECT_EQ(bst.size(), 3);
}

TEST(BinarySearchTreeTest, InOrderTraversal) {
    BinarySearchTree<int> bst;
    std::vector<int> values = { 5, 3, 7, 2, 4, 6, 8 };
    for (int v : values) bst.insert(v);
    auto res = collect(bst.begin(), bst.end());
    std::vector<int> expected = { 2,3,4,5,6,7,8 };
    EXPECT_EQ(res, expected);

    // Reverse in-order
    auto res_rev = collect(bst.rbegin(), bst.rend());
    std::vector<int> expected_rev = { 8,7,6,5,4,3,2 };
    EXPECT_EQ(res_rev, expected_rev);
}

TEST(BinarySearchTreeTest, PreOrderTraversal) {
    BinarySearchTree<int> bst;
    for (int v : {5, 3, 7, 2, 4, 6, 8}) bst.insert(v);
    auto res = collect(bst.pre_begin(), bst.pre_end());
    std::vector<int> expected = { 5,3,2,4,7,6,8 };
    EXPECT_EQ(res, expected);
}

TEST(BinarySearchTreeTest, PostOrderTraversal) {
    BinarySearchTree<int> bst;
    for (int v : {5, 3, 7, 2, 4, 6, 8}) bst.insert(v);
    auto res = collect(bst.post_begin(), bst.post_end());
    std::vector<int> expected = { 2,4,3,6,8,7,5 };
    EXPECT_EQ(res, expected);
}

TEST(BinarySearchTreeTest, EraseLeafNode) {
    BinarySearchTree<int> bst;
    for (int v : {5, 3, 7}) bst.insert(v);
    EXPECT_EQ(bst.erase(3), 1);
    EXPECT_EQ(bst.size(), 2);
    EXPECT_EQ(collect(bst.begin(), bst.end()), std::vector<int>({ 5,7 }));
}

TEST(BinarySearchTreeTest, EraseNodeWithOneChild) {
    BinarySearchTree<int> bst;
    for (int v : {5, 3, 7, 2}) bst.insert(v);
    EXPECT_EQ(bst.erase(3), 1);
    EXPECT_EQ(bst.size(), 3);
    EXPECT_EQ(collect(bst.begin(), bst.end()), std::vector<int>({ 2,5,7 }));
}

TEST(BinarySearchTreeTest, EraseNodeWithTwoChildren) {
    BinarySearchTree<int> bst;
    for (int v : {5, 3, 7, 2, 4}) bst.insert(v);
    EXPECT_EQ(bst.erase(3), 1);
    EXPECT_EQ(bst.size(), 4);
    auto res = collect(bst.begin(), bst.end());
    std::vector<int> expected = { 2,4,5,7 };
    EXPECT_EQ(res, expected);
}

TEST(BinarySearchTreeTest, Clear) {
    BinarySearchTree<int> bst;
    for (int v : {1, 2, 3, 4, 5}) bst.insert(v);
    EXPECT_FALSE(bst.empty());
    bst.clear();
    EXPECT_TRUE(bst.empty());
    EXPECT_EQ(bst.size(), 0);
    EXPECT_EQ(collect(bst.begin(), bst.end()), std::vector<int>{});
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}