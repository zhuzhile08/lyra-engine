#include <Common/Dynarray.h>

#include <vector>

#include <iostream>

int main() {
    lyra::Dynarray<int, 32> foo;
    foo.resize(10);
    foo.fill(4);

    for (const auto& it : foo) std::cout << it << " ";
    std::cout << std::endl << "After insert: " << std::endl;

    foo.insert(foo.begin() + 5, 5);

    for (const auto& it : foo) std::cout << it << " ";
    std::cout << std::endl << "After inserting multiple elements: " << std::endl;

    foo.insert(foo.begin() + 6, 2, 6);

    for (const auto& it : foo) std::cout << it << " ";
    std::cout << std::endl << "After erasing one element: " << std::endl;

    foo.erase(foo.begin() + 9);

    for (const auto& it : foo) std::cout << it << " ";
    std::cout << std::endl << "After erasing multiple elements: " << std::endl;

    foo.erase(foo.begin() + 6, foo.end() - 1);
    for (const auto& it : foo) std::cout << it << " ";

    return 0;
}