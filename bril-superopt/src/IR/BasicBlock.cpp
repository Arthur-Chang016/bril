#include <IR/BasicBlock.h>

namespace ir {

BasicBlock::BasicBlock(std::vector<InstPtr> &&instrs) : instrs(std::move(instrs)) {}

}  // namespace ir
