#include <IR/Heap.h>
#include <IR/Parser.h>

#include <iostream>

int main(int argc, char **argv) {
    auto program = ir::parse(std::cin);
    auto heap = ir::HeapManager();
    auto vars = program->SetupVarContext(argc, argv);

    program->execute(vars, heap);

    return 0;
}