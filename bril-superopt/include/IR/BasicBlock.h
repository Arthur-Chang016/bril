#ifndef IR_BASICBLOCK_H
#define IR_BASICBLOCK_H

#include <IR/Instruction.h>

#include <memory>
#include <vector>

namespace ir {

class BasicBlock;
using BBPtr = std::shared_ptr<BasicBlock>;
using BBEdge = std::weak_ptr<BasicBlock>;

class BasicBlock {
   public:
    BasicBlock(std::vector<InstPtr> &&instrs);
    ~BasicBlock() = default;

   private:
    std::vector<InstPtr> instrs;
    BBEdge taken, notTaken;
};

}  // namespace ir

#endif  // IR_BASICBLOCK_H