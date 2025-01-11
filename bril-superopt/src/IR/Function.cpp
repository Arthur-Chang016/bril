#include <IR/BasicBlock.h>
#include <IR/Function.h>
#include <IR/Instruction.h>
#include <IR/Type.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace ir {

void Function::ConstructCFG(std::vector<InstPtr>& instrs) {
    // find all used labels
    std::unordered_set<std::string> usedLabels;
    for (const auto& instr : instrs) {
        if (auto branch = std::dynamic_pointer_cast<Branch>(instr)) {
            usedLabels.insert(branch->ifTrue);
            usedLabels.insert(branch->ifFalse);
        } else if (auto jump = std::dynamic_pointer_cast<Jump>(instr)) {
            usedLabels.insert(jump->target);
        }
    }
    // construct basic blocks
    std::unordered_map<std::string, BBWPtr> label2bb;
    BBPtr curBlock = std::make_shared<BasicBlock>();
    for (const auto& instr : instrs) {
        auto label = std::dynamic_pointer_cast<Label>(instr);
        if (label && usedLabels.contains(label->name)) {
            if (!curBlock->instrs.empty()) {  // create a new block if it starts with a used label
                this->basicBlocks.push_back(curBlock);
                curBlock = std::make_shared<BasicBlock>();
            }
        }
        if (label && curBlock->instrs.empty())  // tag the block with the label
            label2bb[label->name] = curBlock;
        curBlock->instrs.push_back(instr);

        if (instr->isTerminator()) {  // create a new block if it ends with a terminator
            this->basicBlocks.push_back(curBlock);
            curBlock = std::make_shared<BasicBlock>();
        }
    }
    if (!curBlock->instrs.empty()) this->basicBlocks.push_back(curBlock);
    // connect branch and jump instructions to their corresponding basic blocks
    for (auto bb : this->basicBlocks) {
        assert(!bb->instrs.empty() && "Basic block is empty");
        if (auto branch = std::dynamic_pointer_cast<Branch>(bb->instrs.back())) {
            bb->taken = label2bb.at(branch->ifTrue);
            bb->notTaken = label2bb.at(branch->ifFalse);
        } else if (auto jump = std::dynamic_pointer_cast<Jump>(bb->instrs.back())) {
            bb->taken = label2bb.at(jump->target);
        }
    }
    // connect fall-through basic blocks
    for (size_t i = 0; i < this->basicBlocks.size() - 1; i++) {
        auto cur = this->basicBlocks[i], next = this->basicBlocks[i + 1];
        if (cur->instrs.back()->isTerminator() == false) {
            cur->notTaken = next;
        }
    }
    if (!this->basicBlocks.empty()) this->entryBB = this->basicBlocks.front();
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
    if (funcJson.contains("type") && funcJson["type"] != "void") {
        this->retType = ParseType(funcJson["type"]);
    }
    // parse instructions
    std::vector<InstPtr> instrs;
    for (const auto& instr : funcJson["instrs"]) {
        instrs.push_back(ParseInstr(instr));
    }
    ConstructCFG(instrs);
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
    // print basic blocks
    os << " {\n";
    for (const auto& bb : func.basicBlocks) {
        os << *bb;
    }
    os << "}\n";
    return os;
}

std::optional<int64_t> Function::execute(std::unordered_map<std::string, RuntimeVal>& vars) {
    // TODO
    // BBPtr curBB = this->entryBB;
    // while (curBB) {
    //     auto nextBB = curBB->execute(vars);
    // }

    // for (const auto& bb : basicBlocks) {
    //     for (const auto& instr : bb->instrs) {
    //         instr->execute(vars);
    //     }
}

}  // namespace ir
