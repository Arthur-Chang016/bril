#ifndef IR_FUNCTION_H
#define IR_FUNCTION_H

#include <IR/BasicBlock.h>
#include <IR/Instruction.h>

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
    void print(std::ostream& os) const;

   private:
    std::string name;
    std::vector<VarPtr> args;
    std::vector<BBPtr> BBs;
    TypePtr retType = nullptr;
};

using FunctionPtr = std::shared_ptr<Function>;

}  // namespace ir

#endif  // IR_FUNCTION_H