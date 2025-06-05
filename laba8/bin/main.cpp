#include <iostream>
#include "lib/BST_containers.h"

int main() {
    bst::BinarySearchTree<int> tree;
    int values[] = { 5, 3, 7, 2, 4, 6, 8 };
    for (int v : values) tree.insert(v);

    std::cout << "In-order traversal: ";
    for (auto it = tree.begin(); it != tree.end(); ++it)
        std::cout << *it << ' ';
    std::cout << '\n';

    std::cout << "Reverse in-order: ";
    for (auto it = tree.rbegin(); it != tree.rend(); ++it)
        std::cout << *it << ' ';
    std::cout << '\n';

    std::cout << "Pre-order traversal: ";
    for (auto it = tree.pre_begin(); it != tree.pre_end(); ++it)
        std::cout << *it << ' ';
    std::cout << '\n';

    std::cout << "Post-order traversal: ";
    for (auto it = tree.post_begin(); it != tree.post_end(); ++it)
        std::cout << *it << ' ';
    std::cout << '\n';

    return 0;
}