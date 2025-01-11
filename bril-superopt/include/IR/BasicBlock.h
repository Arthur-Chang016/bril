#ifndef IR_BASICBLOCK_H
#define IR_BASICBLOCK_H

#include <IR/Instruction.h>
#include <IR/Type.h>

#include <memory>
#include <unordered_map>
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
    ctrlStatus execute(std::unordered_map<std::string, RuntimeVal>& vars);

   private:
};

}  // namespace ir

#endif  // IR_BASICBLOCK_H