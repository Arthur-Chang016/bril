#ifndef IR_PROGRAM_H
#define IR_PROGRAM_H

#include <IR/Function.h>

#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <vector>
using json = nlohmann::json;

namespace ir {

class Program {
   public:
    Program(const json& progJson);
    ~Program() = default;
    friend std::ostream& operator<<(std::ostream& os, const Program& prog);

   private:
    std::vector<FunctionPtr> functions;
};

using ProgramPtr = std::shared_ptr<Program>;

}  // namespace ir

#endif  // IR_PROGRAM_H
