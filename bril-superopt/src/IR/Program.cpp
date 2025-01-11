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

void Program::ConstructCallLink(const std::unordered_map<std::string, FuncWPtr>& name2func) {
    for (auto& func : this->functions) {
        for (auto& bb : func->basicBlocks) {
            for (auto& inst : bb->instrs) {
                if (auto callInst = std::dynamic_pointer_cast<Call>(inst)) {
                    if (auto it = name2func.find(callInst->funcName); it != name2func.end()) {
                        callInst->func = it->second;
                    } else {
                        throw std::runtime_error("Function " + callInst->funcName + " not found");
                    }
                }
            }
        }
    }
}

Program::Program(const json& progJson) {
    if (!progJson.contains("functions"))
        throw std::runtime_error("progJson does not contain 'functions'");
    std::unordered_map<std::string, FuncWPtr> name2func;
    for (const auto& funcJson : progJson["functions"]) {
        functions.push_back(std::make_shared<Function>(funcJson));
        name2func[functions.back()->name] = functions.back();
    }
    ConstructCallLink(name2func);
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
