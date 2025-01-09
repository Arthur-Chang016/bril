#include <IR/BasicBlock.h>

#include <iostream>
#include <memory>

namespace ir {

BasicBlock::BasicBlock(std::vector<InstPtr> &&instrs) : instrs(std::move(instrs)) {}

std::ostream &operator<<(std::ostream &os, const BasicBlock &bb) {
    for (auto instr : bb.instrs) {
        auto label = std::dynamic_pointer_cast<Label>(instr);
        os << (!label ? "  " : "") << *instr << std::endl;
    }
    return os;
}

}  // namespace ir
