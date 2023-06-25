#include <Common/Dynarray.h>
#include <Common/FunctionPointer.h>

#include <iostream>

namespace {

int add(int a, int b) {
    return a + b;
}

}

int main() {
    lyra::Function<int(int, int)> addFunction(add);
    std::cout << "Result of an addition function stored in a function pointer (w. args): " << addFunction(2, 3) << std::endl;

    lyra::Dynarray<int, 16> foo;
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
    std::cout << std::endl << "After inserting multiple elements and causing an exception: " << std::endl;

    foo.insert(foo.begin(), 30, 8);

    return 0;
}