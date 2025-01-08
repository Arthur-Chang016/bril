#include <IR/BasicBlock.h>
#include <IR/Function.h>
#include <IR/Instruction.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <iostream>
#include <memory>

namespace ir {

TypePtr ParseType(const json& typeJson) {
    if (typeJson.is_string()) {
        std::string type = typeJson;
        if (type == "int")
            return std::make_shared<IntType>();
        else if (type == "bool")
            return std::make_shared<BoolType>();
        else
            throw std::runtime_error("Unknown type: " + type);
    } else if (typeJson.is_object()) {
        if (!typeJson.contains("ptr")) throw std::runtime_error("typeJson does not contain 'ptr'");
        return std::make_shared<PointerType>(ParseType(typeJson["ptr"]));
    } else {
        throw std::runtime_error("Unknown type: " + typeJson.dump());
    }
}

Function::Function(const json& funcJson) {
    if (!funcJson.contains("name")) throw std::runtime_error("funcJson does not contain 'name'");
    this->name = funcJson["name"];
    if (!funcJson.contains("instrs")) throw std::runtime_error("funcJson does not contain 'instrs'");

    if (funcJson.contains("args")) {
        for (const auto& arg : funcJson["args"]) {
            if (!arg["name"].is_string()) throw std::runtime_error("arg does not contain 'name'");
            std::string name = arg["name"];
            TypePtr type = ParseType(arg["type"]);
            this->args.push_back(std::make_shared<Variable>(std::move(name), type));
        }
    }
    if (funcJson.contains("type")) {
        this->retType = ParseType(funcJson["type"]);
    }

    // TODO args, doens't have to exist
    std::vector<InstPtr> instrs;
    // for (const auto& instr : funcJson["instrs"]) {
    //     instrs.push_back(std::make_shared<Instruction>(instr));
    // }

    // construct basic blocks
    BBPtr currentBlock = std::make_shared<BasicBlock>(std::move(instrs));
    // for (const auto& instr : instrs) {
    //     currentBlock->addInstruction(instr);
    //     if (instr->isTerminator()) {
    //         basicBlocks.push_back(currentBlock);
    //         currentBlock = std::make_shared<BasicBlock>();
    //     }
    // }
}

std::ostream& operator<<(std::ostream& os, const Function& func) {
    os << "@" << func.name;
    if (func.args.size() > 0) {
        os << "(";
        for (size_t i = 0; i < func.args.size(); i++) {
            os << *func.args[i];
            if (i < func.args.size() - 1)
                os << ", ";
        }
        os << ")";
    }
    if (func.retType)
        os << ": " << *func.retType;

    // TODO
    // print basic blocks
    os << " {\n";
    // for (const auto& bb : func.BBs) {
    //     os << "  ";
    //     bb->print(os);
    //     os << "\n";
    // }
    os << "}\n";
    return os;
}

}  // namespace ir
