#ifndef IR_INSTRUCTION_H
#define IR_INSTRUCTION_H

#include <memory>

namespace ir {

// TODO move functions to .cpp

class Instruction {
   public:
    Instruction() = default;
    virtual ~Instruction() = default;

    // Add pure virtual member functions here
    virtual void execute() = 0;
};

using InstPtr = std::shared_ptr<Instruction>;

}  // namespace ir

#endif  // IR_INSTRUCTION_H