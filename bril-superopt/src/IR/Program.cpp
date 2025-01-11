#include <IR/Function.h>
#include <IR/Heap.h>
#include <IR/Program.h>
#include <IR/Type.h>

#include <cassert>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace ir {

Program::Program(const json& progJson) {
    if (!progJson.contains("functions"))
        throw std::runtime_error("progJson does not contain 'functions'");
    for (const auto& function : progJson["functions"]) {
        functions.push_back(std::make_shared<Function>(function));
    }
}

std::ostream& operator<<(std::ostream& os, const Program& prog) {
    for (const auto& func : prog.functions)
        os << *func << std::endl;
    return os;
}

void Program::execute(varContext& vars, HeapManager& heap) {
    for (const auto& func : functions)
        func->execute(vars, heap);
}

}  // namespace ir
