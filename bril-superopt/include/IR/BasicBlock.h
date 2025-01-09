#ifndef IR_BASICBLOCK_H
#define IR_BASICBLOCK_H

#include <IR/Instruction.h>

#include <memory>
#include <vector>

namespace ir {

class BasicBlock;
using BBPtr = std::shared_ptr<BasicBlock>;
using BBWPtr = std::weak_ptr<BasicBlock>;

class BasicBlock {
   public:
    // jmp: taken; fall-through: notTaken
    BBWPtr taken, notTaken;
    std::vector<InstPtr> instrs;

    BasicBlock() = default;
    BasicBlock(std::vector<InstPtr>&& instrs);
    ~BasicBlock() = default;

    friend std::ostream& operator<<(std::ostream& os, const BasicBlock& bb);

   private:
};

}  // namespace ir

#endif  // IR_BASICBLOCK_H