#include <IR/Function.h>
#include <IR/Program.h>

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

}  // namespace ir
