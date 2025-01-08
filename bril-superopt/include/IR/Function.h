#ifndef IR_FUNCTION_H
#define IR_FUNCTION_H

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

    // Add member functions and variables here

   private:
    // Add private member functions and variables here
};

using FunctionPtr = std::shared_ptr<Function>;

}  // namespace ir

#endif  // IR_FUNCTION_H