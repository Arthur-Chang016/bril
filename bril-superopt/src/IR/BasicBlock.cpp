#include <IR/BasicBlock.h>
#include <IR/Heap.h>
#include <IR/Instruction.h>
#include <IR/Type.h>

#include <cassert>
#include <iostream>
#include <memory>
#include <unordered_map>

namespace ir {

BasicBlock::BasicBlock(std::vector<InstPtr> &&instrs) : instrs(std::move(instrs)) {}

std::ostream &operator<<(std::ostream &os, const BasicBlock &bb) {
    for (auto instr : bb.instrs) {
        auto label = std::dynamic_pointer_cast<Label>(instr);
        os << (!label ? "  " : "") << *instr << std::endl;
    }
    return os;
}

ctrlStatus BasicBlock::execute(varContext &vars, HeapManager &heap) {
    ctrlStatus status = false;  // default fall-through for empty BB
    for (const auto &instr : instrs) {
        status = instr->execute(vars, heap);
        if (instr->isTerminator())
            break;
    }
    return status;
}

}  // namespace ir
