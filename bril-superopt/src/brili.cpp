#include <IR/Parser.h>

#include <iostream>

int main() {
    auto program = ir::parse(std::cin);

    std::cout << *program << std::endl;

    // std::cout << "Hello, World!" << std::endl;
    return 0;
}