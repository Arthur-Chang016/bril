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

void Program::SetupMainFunc(const std::unordered_map<std::string, FuncWPtr>& name2func) {
    if (auto it = name2func.find("main"); it != name2func.end()) {
        this->mainFunc = it->second.lock();
    } else {
        throw std::runtime_error("Function 'main' not found");
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
    SetupMainFunc(name2func);
}

varContext Program::SetupVarContext(int argc, char** argv) {
    assert(this->mainFunc && "mainFunc should be setup before setting up varContext");
    if (this->mainFunc->args.size() != static_cast<std::size_t>(argc - 1)) throw std::runtime_error("error: mismatched main argument arity");

    varContext vars;
    for (int i = 1; i < argc; i++) {
        VarPtr symbol = this->mainFunc->args[i - 1];
        auto valStr = std::string(argv[i]);
        if (valStr == "true") {
            if (auto boolType = std::dynamic_pointer_cast<BoolType>(symbol->type)) {
                vars[symbol->name] = RuntimeVal(boolType, 1);
            } else
                throw std::runtime_error("error: invalid argument type, should be int: " + valStr);
        } else if (valStr == "false") {
            if (auto boolType = std::dynamic_pointer_cast<BoolType>(symbol->type)) {
                vars[symbol->name] = RuntimeVal(boolType, 0);
            } else
                throw std::runtime_error("error: invalid argument type, should be int: " + valStr);
        } else if (std::all_of(valStr.begin() + (valStr[0] == '-' ? 1 : 0), valStr.end(), ::isdigit)) {
            if (auto intType = std::dynamic_pointer_cast<IntType>(symbol->type)) {
                vars[symbol->name] = RuntimeVal(std::make_shared<IntType>(), std::stoll(valStr));
            } else
                throw std::runtime_error("error: invalid argument type, should be bool" + valStr);
        } else {
            throw std::runtime_error("error: invalid argument: " + valStr);
        }
    }
    return vars;
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
