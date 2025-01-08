#include <IR/BasicBlock.h>
#include <IR/Function.h>
#include <IR/Instruction.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace ir {

Function::Function(const json& funcJson) {
    if (!funcJson.contains("name")) throw std::runtime_error("funcJson does not contain 'name'");
    if (!funcJson.contains("args")) throw std::runtime_error("funcJson does not contain 'args'");
    if (!funcJson.contains("instrs")) throw std::runtime_error("funcJson does not contain 'instrs'");
    // TODO args
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

}  // namespace ir
