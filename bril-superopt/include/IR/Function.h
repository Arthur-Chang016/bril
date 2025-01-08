#ifndef IR_FUNCTION_H
#define IR_FUNCTION_H

#include <IR/BasicBlock.h>

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

   private:
    std::string name;
    std::vector<BBPtr> BBs;
    // TODO return type
    // TODO args
};

using FunctionPtr = std::shared_ptr<Function>;

}  // namespace ir

#endif  // IR_FUNCTION_H