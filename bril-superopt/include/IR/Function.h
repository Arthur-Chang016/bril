#ifndef IR_FUNCTION_H
#define IR_FUNCTION_H

#include <IR/BasicBlock.h>
#include <IR/Instruction.h>
#include <IR/Type.h>

#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <vector>
using json = nlohmann::json;

namespace ir {

class Function {
   public:
    Function(const json& funcJson);
    ~Function() = default;
    friend std::ostream& operator<<(std::ostream& os, const Function& func);

   private:
    std::string name;
    std::vector<VarPtr> args;
    std::vector<BBPtr> basicBlocks;
    TypePtr retType = nullptr;
};

using FunctionPtr = std::shared_ptr<Function>;

}  // namespace ir

#endif  // IR_FUNCTION_H