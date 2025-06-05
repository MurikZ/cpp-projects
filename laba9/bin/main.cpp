#include <iostream>
#include <vector>
#include <map>
#include "lib/lazy_adapters.h"

int main() {
    std::vector<int> v = {1,2,3,4,5,6};
    
    // Фильтр + трансформация (квадраты нечётных)
    auto pipeline = v 
        | filter([](int x) { return x % 2 == 1; })
        | transform([](int x) { return x * x; });
    std::cout << "Filter + Transform: ";
    for(int x : pipeline) {
        std::cout << x << " "; // 1 9 25
    }

    // Take — взять первые 4 элемента исходного
    std::cout << "\nTake 4: ";
    for(auto x : v | take(4)) {
        std::cout << x << " "; // 1 2 3 4
    }

    // Drop — пропустить первые 2 элемента
    std::cout << "\nDrop 2: ";
    for(auto x : v | drop(2)) {
        std::cout << x << " "; // 3 4 5 6
    }

    // Reverse — реверс
    std::cout << "\nReverse: ";
    for(auto x : v | reverse()) {
        std::cout << x << " "; // 6 5 4 3 2 1
    }

    // Комбинация: реверс + take
    std::cout << "\nReverse + Take 3: ";
    for(auto x : v | reverse() | take(3)) {
        std::cout << x << " "; // 6 5 4
    }

    // Словарь: ключи и значения
    std::map<int, std::string> m = {{1, "one"}, {2, "two"}, {3, "three"}};

    std::cout << "\nKeys: ";
    for(auto k : m | keys()) {
        std::cout << k << " "; // 1 2 3
    }

    std::cout << "\nValues: ";
    for(auto val : m | values()) {
        std::cout << val << " "; // one two three
    }

    std::cout << "\n";

    return 0;
}
