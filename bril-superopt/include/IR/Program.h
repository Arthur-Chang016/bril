#ifndef IR_PROGRAM_H
#define IR_PROGRAM_H

#include <memory>

namespace ir {

class Program {
   public:
    Program();
    ~Program();
};

using ProgramPtr = std::shared_ptr<Program>;

}  // namespace ir

#endif  // IR_PROGRAM_H
